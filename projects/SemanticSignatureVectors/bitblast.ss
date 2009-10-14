(module bitblast scheme
  (require "dimacs.ss")
  (require scheme/match)
  (require scheme/pretty)
  (require srfi/1)
  
  ; Words are LSB first, and outputs are truncated at length of output-bits
  
  (define (list-get al key def)
    (let ((p (assq key al)))
      (if p (cdr p) def)))

  (define (list-put al key val) (cons (cons key val) al))
  
  (define (as-bits count n)
    (let loop ((i 0) (n n))
      (if (= i count)
          '() ; (if (zero? n) '() (error "Too few bits used to represent number" n))
          (cons (odd? n) (loop (add1 i) (quotient n 2))))))
  
  (define (get-number input-bits al) ; al is result from from-picosat or run-picosat
    (let/cc return
      (let loop ((input-bits input-bits))
        (if (null? input-bits)
            0
            (let ((p (if (boolean? (car input-bits))
                         (cons 'x (car input-bits))
                         (assq (car input-bits) al))))
              (if p
                  (let ((rest (* 2 (loop (cdr input-bits)))))
                    (if (cdr p) (add1 rest) rest))
                  (return #f)))))))
  
  (define (set-number-size bits len)
    (let ((actual-len (length bits)))
      (cond
        ((= len actual-len) bits)
        ((> len actual-len) (append bits (make-list (- len actual-len) #f)))
        ((< len actual-len) (take bits len)))))
  
  (define (adder! st in1-bits in2-bits carry-in-bit)
    (let* ((max-length (max (length in1-bits) (length in2-bits)))
           (output-size (add1 max-length))
           (in1 (set-number-size in1-bits output-size))
           (in2 (set-number-size in2-bits output-size)))
      (let loop ((in1 in1) (in2 in2) (carry-in carry-in-bit))
        (cond
          ((and (null? in1) (null? in2)) '())
          ((null? in1) (error "Should not happen"))
          ((null? in2) (error "Should not happen"))
          (else
            (let ((sum (xor-gate! st (car in1) (car in2) carry-in))
                  (carry-out (majority-gate! st (car in1) (car in2) carry-in)))
              (cons sum (loop (cdr in1) (cdr in2) carry-out))))))))
  
  (define (incrementer! st input-bits)
    (adder! st input-bits '() #t))
  
  (define (multiplier! st in1-bits in2-bits)
    (let loop ((m in2-bits))
      (cond
        ((null? m) '())
        ((eq? (car m) #t)
         (adder! st in1-bits (loop (cons #f (cdr m))) #f))
        ((eq? (car m) #f)
         (cons #f (loop (cdr m))))
        (else
         (adder! st
                 (map (lambda (in1b) (and-gate! st (car m) in1b)) in1-bits)
                 (cons #f (loop (cdr m)))
                 #f)))))
  
  (define (less-than-comparator! st in1 in2)
    (let* ((max-len (max (length in1) (length in2)))
           (in1 (set-number-size in1 max-len))
           (in2 (set-number-size in2 max-len)))
      (let loop ((in1 in1) (in2 in2) (lower-bits-less #f))
        (cond
          ((and (null? in1) (null? in2)) lower-bits-less)
          ((null? in1) (error "Should not happen"))
          ((null? in2) (error "Should not happen"))
          (else
           (loop
            (cdr in1)
            (cdr in2)
            (mux! st
                  (xor-gate! st (car in1) (car in2))
                  (and-gate! st (inv (car in1)) (car in2))
                  lower-bits-less)))))))
  
  (define (neg-words! st input-bits)
    (incrementer! st (map inv input-bits)))
  (define (not-words! st input-bits) (map inv input-bits))
  
  (define (must-have-same-length ls1 . lists)
    (let ((len (length ls1)))
      (for-each (lambda (ls) (unless (= (length ls) len) (error "must-have-same-length failed"))) lists)))
  
  (define (and-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply and-gate! st ins)) inputs))
  (define (or-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply or-gate! st ins)) inputs))
  (define (nand-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply nand-gate! st ins)) inputs))
  (define (nor-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply nor-gate! st ins)) inputs))
  (define (xor-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply xor-gate! st ins)) inputs))
  (define (xnor-words! st . inputs)
    (apply must-have-same-length inputs)
    (apply map (lambda ins (apply xnor-gate! st ins)) inputs))
  
  (define (equal-words! st in1-bits in2-bits)
    (must-have-same-length in1-bits in2-bits)
    (apply and-gate! st (xnor-words! st in1-bits in2-bits)))
  
  (define (mux-words! st sel-bit in-true in-false)
    (must-have-same-length in-true in-false)
    (map (lambda (in-t in-f) (mux! st sel-bit in-t in-f)) in-true in-false))
  
  (define (bits-in-type t)
    (match t
      ('uint32_t 32)
      ('int32_t 32)
      ('uint16_t 16)
      ('uint8_t 8)
      ('bool 1)
      (`(memory ,n) 0)
      (_ (error "bits-in-type" t))))
  
  (define (set-by-longest w1 w2)
    (let ((max-len (max (length w1) (length w2))))
      (list (set-number-size w1 max-len) (set-number-size w2 max-len))))
  
  (define-struct (bbstate state) (vars definitions) #:mutable #:transparent)
  (define (make-empty-bbstate)
    (make-bbstate 1 ; variable-counter
                  (make-immutable-hash '()) ; clauses
                  (make-immutable-hasheq '()) ; units
                  #f ; known-unsatisfiable
                  (make-immutable-hasheq '()) ; vars
                  (make-immutable-hasheq '()) ; definitions
                  ))
  
  (define (copy-bbstate st)
    (make-bbstate (state-variable-counter st)
                  (state-clauses st)
                  (state-definitions st)
                  (state-known-unsatisfiable st)
                  (bbstate-vars st)
                  (bbstate-definitions st)))
  
  (define (convert-expr st orig-p return)
    (letrec ((get-var (lambda (v t)
                        (let ((p (hash-ref (bbstate-vars st) v #f)))
                          (if p
                              p
                              (let* ((def (hash-ref (bbstate-definitions st) v #f))
                                     (bits
                                      (if def
                                          (convert-expr st def return)
                                          (variables! st (bits-in-type t)))))
                                (set-bbstate-vars! st (hash-set (bbstate-vars st) v bits))
                                bits))))))
      (let convert-expr ((p orig-p))
        (when (state-known-unsatisfiable st) (return #f))
        (match p
          (`(,t . ,e)
           (let* ((set-len (lambda (e) (set-number-size e (bits-in-type t))))
                  (result (begin
                            ;(pretty-print `(convert-expr ,e ,t))
                            (cond
                              ((number? e) (as-bits (bits-in-type t) e))
                              ((symbol? e) (set-number-size (get-var e t) (bits-in-type t)))
                              ((boolean? e) (list e))
                              ((list? e)
                               (let* ((arg-bits (map convert-expr (cdr e)))
                                      (types (map car (cdr e)))
                                      (tmpl (cons (car e) arg-bits)))
                                 ;(pretty-print `(convert-expr inner ,(car tmpl) ,(map length (cdr tmpl)) ,types))
                                 (match tmpl
                                   (`(and . ,rest)
                                    (apply and-words! st (map set-len rest)))
                                   (`(or . ,rest)
                                    (apply or-words! st (map set-len rest)))
                                   (`(xor . ,rest)
                                    (apply xor-words! st (map set-len rest)))
                                   (`(not ,a) (not-words! st (set-len a)))
                                   (`(negate ,a) (set-number-size (neg-words! st (set-len a)) (bits-in-type t)))
                                   (`(minus ,a) (set-number-size (neg-words! st (set-len a)) (bits-in-type t)))
                                   (`(add . ,rest)
                                    (set-number-size
                                     (let loop ((ls (map set-len rest)))
                                       (if (null? ls)
                                           (as-bits (bits-in-type t) 0)
                                           (if (null? (cdr ls))
                                               (car ls)
                                               (loop (cons (adder! st (car ls) (cadr ls) #f) (cddr ls))))))
                                     (bits-in-type t)))
                                   (`(subtract ,a ,b)
                                    (set-number-size
                                     (adder! st (set-len a) (not-words! st (set-len b)) #t)
                                     (bits-in-type t)))
                                   (`(if-e ,p ,a ,b)
                                    (mux-words! st (apply or-gate! st p) (set-len a) (set-len b)))
                                   (`(logical-and ,a ,b)
                                    (list (and-gate! st (apply or-gate! st a) (apply or-gate! st b))))
                                   (`(logical-or ,a ,b)
                                    (list (apply or-gate! st (append a b))))
                                   (`(logical-not ,a)
                                    (list (inv (car a))))
                                   (`(greater-than-or-equal ,a ,b)
                                    (list (inv (apply less-than-comparator! st (set-by-longest a b)))))
                                   (`(less-than ,a ,b)
                                    (list (apply less-than-comparator! st (set-by-longest a b))))
                                   (`(equal ,a ,b)
                                    (list (apply equal-words! st (set-by-longest a b))))
                                   (`(not-equal ,a ,b)
                                    (list (inv (apply equal-words! st (set-by-longest a b)))))
                                   (`(parity ,a)
                                    (list (apply xnor-gate! st (take a 8)))) ; 8 LSBs
                                   (`(memoryReadByte ,mem ,addr)
                                    (set-number-size (variables! st 8) (bits-in-type t)))
                                   (`(memoryWriteByte ,mem ,addr ,data) '())
                                   (`(multiply ,a ,b)
                                    (set-number-size (multiplier! st a b) (bits-in-type t)))
                                   (`(mulhi32 ,a ,b)
                                    (set-number-size (drop (multiplier! st a b) 32) 32))
                                   (`(imulhi32 ,a ,b)
                                    (variables! st 32)) ; No constraints
                                   (`(idiv32 ,a ,b ,c) (variables! st 32))
                                   (`(imod32 ,a ,b ,c) (variables! st 32))
                                   (`(div32 ,a ,b ,c) (variables! st 32))
                                   (`(mod32 ,a ,b ,c) (variables! st 32))
                                   (`(imulhi16 ,a ,b)
                                    (variables! st 16)) ; No constraints
                                   (`(idiv16 ,a ,b ,c) (variables! st 16))
                                   (`(imod16 ,a ,b ,c) (variables! st 16))
                                   (`(div16 ,a ,b ,c) (variables! st 16))
                                   (`(mod16 ,a ,b ,c) (variables! st 16))
                                   (`(imulhi8 ,a ,b)
                                    (variables! st 8)) ; No constraints
                                   (`(idiv8 ,a ,b) (variables! st 8))
                                   (`(imod8 ,a ,b) (variables! st 8))
                                   (`(div8 ,a ,b) (variables! st 8))
                                   (`(mod8 ,a ,b) (variables! st 8))
                                   (`(bsr ,a) (set-number-size (variables! st 5) 8))
                                   (`(bsf ,a) (set-number-size (variables! st 5) 8))
                                   (`(left-shift ,a ,b)
                                    (cond
                                      ((number? (cdr (caddr e))) ; Constant count
                                       (let ((count (cdr (caddr e))))
                                         (append (as-bits count 0) (drop-right a count))))
                                      (else (variables! st (length a)))))
                                   (`(right-shift ,a ,b)
                                    (cond
                                      ((number? (cdr (caddr e))) ; Constant count
                                       (let ((count (cdr (caddr e))))
                                         (append (drop a count) (as-bits count 0))))
                                      (else (variables! st (length a)))))
                                   (_ (error "Bad operator" (car e))))))
                              (else (error "convert-expr" e))))))
             ;(pretty-print `(convert-expr ,e -> ,result))
             (set-number-size result (bits-in-type t))))
          (e (error "No type given" orig-p))))))
  
  (define (convert-constraints st cl return)
    (for-each (lambda (c)
                ;(pretty-print `(converting ,c))
                (match c
                  (`(define ,a ,b)
                   (cond
                     ((not (symbol? a)) (error "Invalid define symbol" a))
                     ((or (hash-ref (bbstate-vars st) a #f)
                          (hash-ref (bbstate-definitions st) a #f))
                      ; (car b) is type of b
                      (convert-constraints st `((check (bool equal (,(car b) . ,a) ,b))) return))
                     (else (set-bbstate-definitions! st (hash-set (bbstate-definitions st) a b)))))
                  (`(check ,e)
                   (force-1! st (apply or-gate! st (convert-expr st e return))))
                  (_ (error "Cannot handle constraint" c))))
              (reverse cl))
    (simplify-state! st))
  
  (define (run-solver-annotated st)
    (simplify-state! st)
    (let ((do-printout (or (and (>= (variable-count st) 100) (not (zero? (clause-count st))))
                           (and (number? (clause-count st)) (>= (clause-count st) 1)))))
      (when do-printout
        (display `(running solver with ,(variable-count st) variables ,(clause-count st) clauses ,(hash-count (bbstate-vars st)) of ,(hash-count (bbstate-definitions st)) user-vars))
        (flush-output (current-output-port)))
      (let ((solver-result (run-solver st)))
        (when do-printout (pretty-print `(--> ,(if solver-result 'SAT 'UNSAT))))
        solver-result)))
  
  (define (constraints-satisfiable? cl typed-vars-to-return)
    (let/cc return
      (let* ((st (make-empty-bbstate)))
        (convert-constraints st cl return)
        (for-each (lambda (var) (convert-expr st var return)) typed-vars-to-return)
        (let ((picosat-result (run-solver-annotated st)))
          (and picosat-result
               (map (lambda (var)
                      (get-number (hash-ref (bbstate-vars st) (cdr var) #f) picosat-result))
                    typed-vars-to-return))))))
  
  (define (possible-boolean-values constraints to-test)
    (let ((st (make-empty-bbstate)))
      (convert-constraints st constraints (lambda x (error "Inconsistent constraints" constraints)))
      (let ((can-be-true
             (let/cc return1
               (let ((st-copy (copy-bbstate st)))
                 (convert-constraints st-copy `((check ,to-test)) return1)
                 (run-solver-annotated st-copy))))
            (can-be-false
             (let/cc return1
               (let ((st-copy (copy-bbstate st)))
                 (convert-constraints st-copy `((check (bool logical-not ,to-test))) return1)
                 (run-solver-annotated st-copy)))))
        (if can-be-true
            (if can-be-false
                '*
                #t)
            (if can-be-false
                #f
                (error "Inconsistent constraints" `(,to-test . ,constraints)))))))
  
  (define (is-constant-base? st t typed-expr)
    ;(pretty-print `(is-constant? ,typed-expr))
    (let/cc return
      (let* ((st (copy-bbstate st))
             (converted-expr (convert-expr st typed-expr return)))
        (if (andmap boolean? converted-expr)
          (get-number converted-expr '())
          (let ((solver-result (run-solver-annotated st)))
            (unless solver-result (return #f)) ; Inconsistent constraints
            (let ((first-result (get-number converted-expr solver-result)))
              (unless first-result (return #f))
              (force-0! st (equal-words! st converted-expr (as-bits (bits-in-type t) first-result)))
              (if (run-solver-annotated st) (return #f) (void))
              (pretty-print `(,typed-expr -> constant ,first-result))
              first-result))))))
      
  (define (is-constant? constraints t typed-expr)
    ;(pretty-print `(is-constant? ,typed-expr))
    (let/cc return
      (let* ((st (make-empty-bbstate)))
        (convert-constraints st constraints return)
        (is-constant-base? st t typed-expr))))
      
  (provide make-empty-bbstate
           copy-bbstate
           adder!
           multiplier!
           incrementer!
           less-than-comparator!
           neg-words!
           not-words!
           as-bits
           get-number
           and-words!
           or-words!
           nand-words!
           nor-words!
           xor-words!
           xnor-words!
           equal-words!
           mux-words!
           convert-constraints
           constraints-satisfiable?
           possible-boolean-values
           is-constant-base?
           is-constant?
           bbstate-definitions)
      
      )
    
