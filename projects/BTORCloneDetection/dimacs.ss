(module dimacs scheme
  
  (require srfi/1)
  (require srfi/13)
  (require scheme/system)
  (require scheme/pretty)
  
  (define-struct state
    ((variable-counter)
     (clauses)
     (definitions)
     (known-unsatisfiable))
    #:mutable #:transparent)

  (define (make-empty-state)
    (make-state 1 (make-immutable-hash '()) (make-immutable-hasheq '()) #f))

  (define (variable! st)
    (let ((c (state-variable-counter st)))
      (set-state-variable-counter! st (add1 c))
      c))
  (define (inv v)
    (cond
      ((number? v) (- v))
      ((boolean? v) (not v))
      (else (error "inv" v))))
  (define (variables! st n) (list-tabulate n (lambda _ (variable! st))))
  (define (sort-numbers ls) ; There doesn't seem to be a good way to import sort
    (if (null? ls)
        '()
        (let ((m (apply min ls)))
          (cons m (sort-numbers (remove-one m ls))))))
  (define (add-clause! st cl)
    (let loop ((cl cl))
      (cond
        ((state-known-unsatisfiable st) (set-state-clauses! st (make-immutable-hash (list (cons '() #t)))))
        ((memq #t cl) (void))
        ((has-inverses? cl) (void))
        (else
          (let ((cl (sort-numbers (filter (lambda (x) (not (eq? x #f))) cl))))
            (if (null? cl)
              (set-state-known-unsatisfiable! st #t) ; fail
              (begin
                (for-each (lambda (lit)
                            (create-gate! st #f lit '()))
                          cl)
                (set-state-clauses! st (hash-set (state-clauses st) cl #t)))))))))

  (define (simplify-state! st) st)
  (define (clause-to-string cl)
    (format "~a 0~n" (string-join (map number->string cl) " ")))

  (define (variable-count st) (state-variable-counter st))
  (define (clause-count st) (if (state-known-unsatisfiable st) #f (hash-count (state-clauses st))))
  
  (define (to-dimacs st)
    (format "p cnf ~a ~a~n~a"
            (variable-count st)
            (clause-count st)
            (string-concatenate (hash-map (state-clauses st) (lambda (cl _) (clause-to-string cl))))))
  
  (define (from-picosat output)
    (let* ((tokens (string-tokenize output))
           ;(_ (display `(tokens . ,tokens)))
           ;(_ (newline))
           (first-token (car tokens))
           (second-token (cadr tokens))
           (output-valid? (equal? first-token "s"))
           (is-satisfiable? (equal? second-token "SATISFIABLE"))
           (is-unsatisfiable? (equal? second-token "UNSATISFIABLE")))
      (cond
        ((not output-valid?) (error "Invalid first token" first-token))
        (is-satisfiable? 
         (let* ((third-token (caddr tokens))
                (other-tokens (cdddr tokens)))
           (if (equal? third-token "v")
               (filter-map (lambda (var-str)
                             (if (equal? var-str "v")
                                 #f
                                 (let ((n (string->number var-str)))
                                   (cond
                                     ((< n 0) `(,(inv n) . #f))
                                     ((> n 0) `(,n . #t))
                                     ((zero? n) (error "End of variable list reached too early"))))))
                           (drop-right other-tokens 1))
               (error "Invalid third token" third-token))))
        (is-unsatisfiable? #f)
        (else (error "Invalid second token" second-token)))))
  
  (define (from-minisat output)
    (let* (;(_ (write `(output . ,output)))
           ;(_ (newline))
           (tokens (string-tokenize output))
           ;(_ (display `(tokens . ,tokens)))
           ;(_ (newline))
           (first-token (car tokens))
           (is-satisfiable? (equal? first-token "SAT"))
           (is-unsatisfiable? (equal? first-token "UNSAT")))
      (cond
        (is-satisfiable? 
         (let* ((other-tokens (cdr tokens)))
           (filter-map (lambda (var-str)
                         (let ((n (string->number var-str)))
                           (cond
                             ((< n 0) `(,(inv n) . #f))
                             ((> n 0) `(,n . #t))
                             ((zero? n) (error "End of variable list reached too early")))))
                       (drop-right other-tokens 2))))
        (is-unsatisfiable? #f)
        (else (error "Invalid first token" first-token)))))
  
  (define (run-process-raw proc-name input)
    (let* ((proc (process proc-name))
           (stdout (car proc))
           (stdin (cadr proc))
           (pid (caddr proc))
           (stderr (cadddr proc))
           (control (fifth proc)))
      (write-string input stdin)
      (close-output-port stdin)
      (close-input-port stderr)
      (let ((output
             (let loop ((output ""))
               (let ((result (read-string 4096 stdout)))
                 (if (eof-object? result)
                     output
                     (loop (string-append output result)))))))
        (close-input-port stdout)
        (control 'wait)
        output)))
  
  (define (run-picosat st)
    (cond 
      ((state-known-unsatisfiable st) #;(pretty-print `(known unsat)) #f)
      ((zero? (hash-count (state-clauses st))) '())
      (else (from-picosat (run-process-raw "/home/willcock2/picosat-632/picosat" (to-dimacs st))))))
  
  (define (run-minisat st)
    (cond 
      ((state-known-unsatisfiable st) #;(pretty-print `(known unsat)) #f)
      ((zero? (hash-count (state-clauses st))) '())
      (else
        (let* ((dimacs (to-dimacs st))
               ;(_ (when (>= (hash-count (state-clauses st)) 600) (with-output-to-file "foo.dimacs" (lambda () (write-string dimacs)) #:exists 'replace) (exit 0)))
               (minisat-result (run-process-raw "/home/willcock2/minisat/simp/minisat_static -verbosity=0 /dev/stdin /dev/stdout 2>/dev/null" dimacs)))
          ;(pretty-print (to-dimacs st))
          ;(pretty-print minisat-result)
          (from-minisat minisat-result)))))

  (define run-solver run-minisat)
  
  (define (make-reduction-tree st operation-size f output inputs)
    (if (<= (length inputs) operation-size)
        (apply f st output inputs)
        (let* ((subtrees
                (let loop ((ls inputs))
                  (if (<= (length ls) operation-size)
                      (list ls)
                      (cons (take ls operation-size) (loop (drop ls operation-size))))))
               (new-vars (map (lambda (s) (if (= (length s) 1) #f (variable! st))) subtrees)))
          (for-each (lambda (chunk var) (if var (apply f st var chunk) (void))) subtrees new-vars)
          (make-reduction-tree st operation-size f output
                               (map (lambda (nv s) (or nv (car s)))
                                    new-vars subtrees)))))
  
  ; Helpers and logic gates
  ; All of these can have inputs and/or outputs inverted using the inv function
  
  (define (force-1! st var)
    (cond
      ((eq? var #f) (unsatisfiable! st))
      ((eq? var #t) (void))
      (else (create-gate! st #f var `((,var))))))
  (define (force-0! st var) (force-1! st (inv var)))
  (define (unsatisfiable! st)
    #;(pretty-print `(unsatisfiable!))
    (set-state-known-unsatisfiable! st #t)
    (set-state-clauses! st (make-immutable-hash (list (cons '() #t)))))
  
  (define (has-inverses? vars)
    (if (null? vars)
        #f
        (if (memv (inv (car vars)) (cdr vars))
            #t
            (has-inverses? (cdr vars)))))

  (define (create-gate! st force-generation output clauses)
    ; If output is already defined, insert its definitions
    (let* ((var (abs output))
           (def (hash-ref (state-definitions st) var #f)))
      (if (or force-generation def)
        (begin
          (set-state-definitions! st (hash-set (state-definitions st) var #t)) ; Mark this variable as being used
          (when (list? def) (for-each (lambda (cl) (add-clause! st cl)) (append def clauses))))
        (set-state-definitions! st (hash-set (state-definitions st) var clauses)))))
  
  (define (or-gate-raw! st output . inputs)
    ;(pretty-print `(or-gate-raw! ,@inputs -> ,output))
    (create-gate! st
                  #f
                  output
                  `(,@(map (lambda (in) `(,(inv in) ,output)) inputs)
                    (,(inv output) ,@inputs))))
  
  (define (or-gate! st . inputs)
    ;(pretty-print `(or-gate! . ,inputs))
    (if (or (memq #t inputs) (has-inverses? inputs))
        #t
        (let ((inputs (sort-numbers (delete-duplicates (filter (lambda (x) (not (eq? x #f))) inputs) eqv?))))
          (cond
            ((null? inputs) #f)
            ((null? (cdr inputs)) (car inputs))
            (else
              (let ((output (variable! st)))
                (apply or-gate-raw! st output inputs)
                output))))))
  
  (define (nor-gate! st . inputs) (inv (apply or-gate! st inputs)))
  (define (and-gate! st . inputs) (inv (apply or-gate! st (map inv inputs))))
  (define (nand-gate! st . inputs) (apply or-gate! st (map inv inputs)))
  (define (identify! st . vars)
    (cond
      ((has-inverses? vars) (unsatisfiable! st))
      ((memq #t vars)
       (for-each (lambda (v) (force-1! st v)) vars))
      ((memq #f vars)
       (for-each (lambda (v) (force-0! st v)) vars))
      (else
       (let ((v1 (car vars)))
         (let loop ((vars (cdr vars)))
           (if (null? vars)
               (void)
               (let ((v2 (car vars)))
                 (create-gate! st #f v2 `((,v1 ,(inv v2)) (,(inv v1) ,v2)))
                 (create-gate! st #f v1 `((,v1 ,(inv v2)) (,(inv v1) ,v2)))
                 (loop (cdr vars)))))))))
  
  (define (mux! st sel in-true in-false)
    (cond
      ((eq? sel #t) in-true)
      ((eq? sel #f) in-false)
      ((eq? in-false #f) (and-gate! st sel in-true))
      ((eq? in-true #f) (and-gate! st (inv sel) in-false))
      ((eq? in-false #t) (or-gate! st (inv sel) in-true))
      ((eq? in-true #t) (or-gate! st sel in-false))
      ((eqv? in-true in-false) in-true)
      ((< in-false in-true) (mux! st (inv sel) in-false in-true))
      (else
       (let ((output (variable! st)))
         ;(pretty-print `(mux! ,sel ,in-true ,in-false -> ,output))
         (create-gate!
           st
           #f
           output
           `((,sel ,in-false ,(inv output))
             (,sel ,(inv in-false) ,output)
             (,(inv sel) ,in-true ,(inv output))
             (,(inv sel) ,(inv in-true) ,output)))
         output))))
  
  (define (xor2-gate! st in1 in2)
    (and-gate! st (or-gate! st in1 in2) (nand-gate! st in1 in2)))
  
  (define (remove-one elt ls)
    (cond
      ((null? ls) '())
      ((eqv? elt (car ls)) (cdr ls))
      (else (cons (car ls) (remove-one elt (cdr ls))))))
  
  (define (xor-gate! st . inputs)
    (let loop ((inputs (filter (lambda (x) (not (eq? x #f))) inputs)))
      (cond
        ((null? inputs) #f)
        ((eq? (car inputs) #t) (inv (loop (cdr inputs))))
        ((memv (inv (car inputs)) (cdr inputs)) (inv (loop (remove-one (inv (car inputs)) (cdr inputs)))))
        ((memv (car inputs) (cdr inputs)) (loop (remove-one (car inputs) (cdr inputs))))
        (else
         (xor2-gate! st (car inputs) (loop (cdr inputs)))))))
  
  (define (xnor-gate! st . inputs) (inv (apply xor-gate! st inputs)))
  
  (define (threshold-gate-chain! st k . inputs) ; Returns list of at-least-0-of-n, at-least-1-of-n, ..., at-least-k-of-n
    (cond
      ((zero? k) '())
      ((null? inputs) (list #t))
      ((memq #f inputs) (take (append (apply threshold-gate-chain! st k (remove-one #f inputs)) (list #f)) k))
      ((memq #t inputs) (cons #t (apply threshold-gate-chain! st (sub1 k) (remove-one #t inputs))))
      (else (let* ((rest (apply threshold-gate-chain! st k (cdr inputs)))
                   (chain-for-false (take (append rest (list #f)) k))
                   (chain-for-true (cons #t (take rest (sub1 k)))))
              (map (lambda (cf ct)
                     (mux! st (car inputs) ct cf))
                   chain-for-false chain-for-true)))))
  
  (define (threshold-gate! st required-count . inputs)
    (last (apply threshold-gate-chain! st required-count inputs)))

  (define (majority-gate! st in1 in2 in3)
    (threshold-gate! st 2 in1 in2 in3))
  
  (provide state)
  (provide state-variable-counter)
  (provide state-clauses)
  (provide state-definitions)
  (provide state-known-unsatisfiable)
  (provide make-empty-state)
  (provide variable!)
  (provide variable-count)
  (provide clause-count)
  (provide inv)
  (provide variables!)
  ; (provide add-clause!)
  (provide simplify-state!)
  (provide to-dimacs)
  (provide run-solver)
  (provide force-0!)
  (provide force-1!)
  (provide unsatisfiable!)
  (provide or-gate!)
  (provide nor-gate!)
  (provide and-gate!)
  (provide nand-gate!)
  (provide identify!)
  (provide mux!)
  (provide xor-gate!)
  (provide xnor-gate!)
  (provide threshold-gate!)
  (provide threshold-gate-chain!)
  (provide majority-gate!)
  
  )
