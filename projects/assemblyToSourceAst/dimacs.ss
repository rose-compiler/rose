(module dimacs mzscheme
  
  (require srfi/1)
  (require srfi/13)
  (require scheme/system)
  
  (define variable-counter 1) ; 0 is not a valid value
  (define clauses '())
  
  (define (reset!) (set! variable-counter 1) (set! clauses '()))
  (define (variable!) (let ((c variable-counter)) (set! variable-counter (add1 c)) c))
  (define (variables! n) (list-tabulate n (lambda _ (variable!))))
  (define (add-clause! cl) (set! clauses (cons cl clauses)))
  
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
           (_ (display `(tokens . ,tokens)))
           (_ (newline))
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
               (map (lambda (var-str)
                      (let ((n (string->number var-str)))
                        (cond
                          ((< n 0) `(,(- n) . #f))
                          ((> n 0) `(,n . #t))
                          ((zero? n) (error "End of variable list reached too early")))))
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
    (from-picosat (run-process-raw "/home/willcock2/picosat-632/picosat" (to-dimacs))))
  
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
  ; All of these can have inputs and/or outputs inverted using the - function
  
  (define (force-1! var) (add-clause! `(,var)))
  (define (force-0! var) (force-1! (- var)))
  
  (define (or-gate-raw! output . inputs)
    (for-each (lambda (in) (add-clause! `(,(- in) ,output))) inputs)
    (add-clause! `(,(- output) ,@inputs)))
  (define (or-gate! output . inputs)
    (make-reduction-tree 5 or-gate-raw! output inputs))
  
  (define (nor-gate! output . inputs) (apply or-gate! (- output) inputs))
  (define (and-gate! output . inputs) (apply or-gate! (- output) (map - inputs)))
  (define (nand-gate! output . inputs) (apply or-gate! output (map - inputs)))
  (define (identity-gate! output input) (or-gate! output input))
  (define (not-gate! output input) (nor-gate! output input))
  (define (xor2-gate! output in1 in2)
    (add-clause! `(,in1 ,in2 ,(- output)))
    (add-clause! `(,(- in1) ,in2 ,output))
    (add-clause! `(,in1 ,(- in2) ,output))
    (add-clause! `(,(- in1) ,(- in2) ,(- output))))
  (define (xor-gate! output . inputs)
    (make-reduction-tree 2 xor2-gate! output inputs))
  (define (xnor-gate! output . inputs) (apply xor-gate! (- output) inputs))
  (define (majority-gate! output in1 in2 in3)
    (add-clause! `(,(- in1) ,(- in2) ,output))
    (add-clause! `(,(- in1) ,(- in3) ,output))
    (add-clause! `(,(- in2) ,(- in3) ,output))
    (add-clause! `(,in1 ,in2 ,(- output)))
    (add-clause! `(,in1 ,in3 ,(- output)))
    (add-clause! `(,in2 ,in3 ,(- output))))
  
  (provide reset!)
  (provide variable!)
  (provide variables!)
  (provide add-clause!)
  (provide to-dimacs)
  (provide run-picosat)
  (provide force-0!)
  (provide force-1!)
  (provide or-gate!)
  (provide nor-gate!)
  (provide and-gate!)
  (provide nand-gate!)
  (provide identity-gate!)
  (provide not-gate!)
  (provide xor-gate!)
  (provide xnor-gate!)
  (provide majority-gate!)
  
  )
