(module dimacs mzscheme
  
  (require srfi/1)
  (require srfi/13)
  (require scheme/system)
  (require scheme/pretty)
  
  (define variable-counter 1) ; 0 is not a valid value
  (define clauses '())
  (define known-unsatisfiable #f)
  (define or-gate-cse-table (make-hash-table 'equal))
  
  (define (reset!)
    (set! variable-counter 1)
    (set! clauses '())
    (set! known-unsatisfiable #f)
    (set! or-gate-cse-table (make-hash-table 'equal)))

  (define (variable!) (let ((c variable-counter)) (set! variable-counter (add1 c)) c))
  (define (inv v)
    (cond
      ((number? v) (- v))
      ((boolean? v) (not v))
      (else (error "inv" v))))
  (define (variables! n) (list-tabulate n (lambda _ (variable!))))
  (define (sort-numbers ls) ; There doesn't seem to be a good way to import sort
    (if (null? ls)
        '()
        (let ((m (apply min ls)))
          (cons m (sort-numbers (remove-one m ls))))))
  (define (add-clause! cl)
    (if (memq #t cl)
      (void)
      (let ((cl (filter (lambda (x) x) cl)))
        (if (null? cl)
          (set! known-unsatisfiable #t) ; fail
          (set! clauses (cons cl clauses))))))
  
  (define (to-dimacs)
    (format "p cnf ~a ~a~n~a"
            (sub1 variable-counter)
            (length clauses)
            (string-concatenate
             (map (lambda (cl)
                    (format "~a 0~n" (string-join (map number->string cl) " ")))
                  clauses))))
  
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
  
  (define (run-picosat)
    (if known-unsatisfiable
        (begin #;(pretty-print `(known unsat)) #f)
        (from-picosat (run-process-raw "/home/willcock2/picosat-632/picosat" (to-dimacs)))))
  
  (define (make-reduction-tree operation-size f output inputs)
    (if (<= (length inputs) operation-size)
        (apply f output inputs)
        (let* ((subtrees
                (let loop ((ls inputs))
                  (if (<= (length ls) operation-size)
                      (list ls)
                      (cons (take ls operation-size) (loop (drop ls operation-size))))))
               (new-vars (map (lambda (st) (if (= (length st) 1) #f (variable!))) subtrees)))
          (for-each (lambda (chunk var) (if var (apply f var chunk) (void))) subtrees new-vars)
          (make-reduction-tree operation-size f output
                               (map (lambda (nv st) (or nv (car st)))
                                    new-vars subtrees)))))
  
  ; Helpers and logic gates
  ; All of these can have inputs and/or outputs inverted using the inv function
  
  (define (force-1! var)
    (cond
      ((eq? var #f) (unsatisfiable!))
      ((eq? var #t) (void))
      (else (add-clause! `(,var)))))
  (define (force-0! var) (force-1! (inv var)))
  (define (unsatisfiable!) (add-clause! `()))
  
  (define (has-inverses? vars)
    (if (null? vars)
        #f
        (if (memv (inv (car vars)) (cdr vars))
            #t
            (has-inverses? (cdr vars)))))
  
  (define (or-gate-raw! output . inputs)
    (for-each (lambda (in) (add-clause! `(,(inv in) ,output))) inputs)
    (add-clause! `(,(inv output) ,@inputs)))
  
  (define (or-gate! . inputs)
    (if (or (memq #t inputs) (has-inverses? inputs))
        #t
        (let ((inputs (sort-numbers (delete-duplicates (filter (lambda (x) (not (eq? x #f))) inputs) eqv?))))
          (cond
            ((null? inputs) #f)
            ((null? (cdr inputs)) (car inputs))
            (else
             (let ((cse-entry (hash-table-get or-gate-cse-table inputs #f)))
               (if cse-entry
                   (begin #;(pretty-print `(got ,inputs -> ,cse-entry from table)) cse-entry)
                   (let ((output (variable!)))
                     (make-reduction-tree 5 or-gate-raw! output inputs)
                     (hash-table-put! or-gate-cse-table inputs output)
                     output))))))))
  
  (define (nor-gate! . inputs) (inv (apply or-gate! inputs)))
  (define (and-gate! . inputs) (inv (apply or-gate! (map inv inputs))))
  (define (nand-gate! . inputs) (apply or-gate! (map inv inputs)))
  (define (identify! . vars)
    (cond
      ((has-inverses? vars) (unsatisfiable!))
      ((memq #t vars)
       (for-each force-1! vars))
      ((memq #f vars)
       (for-each force-0! vars))
      (else
       (let ((v1 (car vars)))
         (let loop ((vars (cdr vars)))
           (if (null? vars)
               (void)
               (let ((v2 (car vars)))
                 (add-clause! `(,v1 ,(inv v2)))
                 (add-clause! `(,(inv v1) ,v2))
                 (loop (cdr vars)))))))))
  
  (define (mux! sel in-true in-false)
    (cond
      ((eq? sel #t) in-true)
      ((eq? sel #f) in-false)
      ((eq? in-false #f) (and-gate! sel in-true))
      ((eq? in-true #f) (and-gate! (inv sel) in-false))
      ((eq? in-false #t) (or-gate! (inv sel) in-true))
      ((eq? in-true #t) (or-gate! sel in-false))
      ((eqv? in-true in-false) in-true)
      (else
       (let ((output (variable!)))
         (add-clause! `(,sel ,in-false ,(inv output)))
         (add-clause! `(,sel ,(inv in-false) ,output))
         (add-clause! `(,(inv sel) ,in-true ,(inv output)))
         (add-clause! `(,(inv sel) ,(inv in-true) ,output))
         output))))
  
  (define (xor2-gate! in1 in2)
    (mux! in1 (inv in2) in2))
  
  (define (remove-one elt ls)
    (cond
      ((null? ls) '())
      ((eqv? elt (car ls)) (cdr ls))
      (else (cons (car ls) (remove-one elt (cdr ls))))))
  
  (define (xor-gate! . inputs)
    (let loop ((inputs (filter (lambda (x) (not (eq? x #f))) inputs)))
      (cond
        ((null? inputs) #f)
        ((eq? (car inputs) #t) (inv (loop (cdr inputs))))
        ((memv (inv (car inputs)) (cdr inputs)) (inv (loop (remove-one (inv (car inputs)) (cdr inputs)))))
        ((memv (car inputs) (cdr inputs)) (loop (remove-one (car inputs) (cdr inputs))))
        (else
         (xor2-gate! (car inputs) (loop (cdr inputs)))))))
  
  (define (xnor-gate! . inputs) (inv (apply xor-gate! inputs)))
  
  (define (threshold-gate! required-count . inputs)
    (cond
      ((zero? required-count) #t)
      ((> required-count (length inputs)) #f)
      ((= required-count (length inputs)) (apply and-gate! inputs))
      ((= required-count 1) (apply or-gate! inputs))
      ((memq #f inputs) (apply threshold-gate! required-count (remove-one #f inputs)))
      ((memq #t inputs) (apply threshold-gate! (sub1 required-count) (remove-one #t inputs)))
      (else (mux! (car inputs)
                  (apply threshold-gate! (sub1 required-count) (cdr inputs))
                  (apply threshold-gate! required-count (cdr inputs))))))
  
  (define (majority-gate! in1 in2 in3)
    (threshold-gate! 2 in1 in2 in3))
  
  (provide reset!)
  (provide variable!)
  (provide inv)
  (provide variables!)
  ; (provide add-clause!)
  (provide to-dimacs)
  (provide run-picosat)
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
  (provide majority-gate!)
  
  )
