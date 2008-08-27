(module bitblast mzscheme
  (require "dimacs.ss")
  (require scheme/match)
  (require scheme/pretty)
  (require srfi/1)
  
  ; Words are LSB first, and outputs are truncated at length of output-bits
  (define (make-number-vars! count) (variables! count))
  
  (define (as-bits count n)
    (let loop ((i 0) (n n))
      (if (= i count)
          (if (zero? n) '() (error "Too few bits used to represent number" n))
          (cons (odd? n) (loop (add1 i) (quotient n 2))))))

  (define (set-number! output-bits n) ; output-bits must be large enough to represent n
    (let loop ((output-bits output-bits) (n n))
      (if (null? output-bits)
          (if (zero? n) (void) (error "Too few bits used to represent number" n))
          (begin
            (if (odd? n) (force-1! (car output-bits)) (force-0! (car output-bits)))
            (loop (cdr output-bits) (quotient n 2))))))
  
  (define (get-number input-bits al) ; al is result from from-picosat or run-picosat
    (let loop ((input-bits input-bits))
      (if (null? input-bits)
          0
          (let ((p (assq (car input-bits) al)))
            (if p
                (let ((rest (* 2 (loop (cdr input-bits)))))
                  (if (cdr p) (add1 rest) rest))
                (error "Trying get-number on unspecified bit" (car input-bits)))))))
  
  (define (adder! output-bits in1-bits in2-bits carry-in-bit)
    (let loop ((outputs output-bits) (in1 in1-bits) (in2 in2-bits) (carry-in carry-in-bit))
      (if (null? outputs)
          (void)
          (let ((all-inputs `(,@(if (null? in1) '() (list (car in1))) ,@(if (null? in2) '() (list (car in2))) ,carry-in))
                (carry-out #f))
            (match all-inputs
              (`() (force-0! (car outputs)))
              (`(,in) (identity-gate! (car outputs) in))
              (`(,i1 ,i2) (set! carry-out (variable!)) (and-gate! carry-out i1 i2) (xor-gate! (car outputs) i1 i2))
              (`(,i1 ,i2 ,i3)
               (set! carry-out (variable!))
               (majority-gate! carry-out i1 i2 i3)
               (xor-gate! (car outputs) i1 i2 i3))
              (_ (error "Too many inputs" all-inputs)))
            (loop (cdr outputs) (if (null? in1) in1 (cdr in1)) (if (null? in2) in2 (cdr in2)) carry-out)))))
  
  (define (incrementer! output-bits input-bits)
    (let loop ((outputs output-bits) (inputs input-bits) (carry-in #t))
      (if (null? outputs)
          (void)
          (let ((carry-out (variable!)))
            (if (null? inputs)
                (begin (force-0! carry-out) (identity-gate! (car outputs) carry-in))
                (begin (and-gate! carry-out carry-in (car inputs))
                       (xor-gate! (car outputs) (car inputs) carry-in)))
            (loop (cdr outputs) (if (null? inputs) '() (cdr inputs)) carry-out)))))
  
  (define (comparator! less-output-bit in1 in2)
    (must-have-same-length in1 in2)
    (let loop ((bits1 (reverse in1)) (bits2 (reverse in2)) (prev-less-bit #f))
      (if (null? bits1)
          (identity-gate! less-output-bit prev-less-bit)
          (let ((this-pair-less (variable!))
                (this-pair-not-equal (variable!))
                (new-less-bit (variable!)))
            (and-gate! this-pair-less (inv (car bits1)) (car bits2))
            (xor-gate! this-pair-not-equal (car bits1) (car bits2))
            (mux! new-less-bit this-pair-not-equal this-pair-less prev-less-bit)
            (loop (cdr bits1) (cdr bits2) new-less-bit)))))
  
  (define (neg-words! output-bits input-bits)
    (incrementer! output-bits (map inv input-bits)))
  (define (not-words! output-bits input-bits)
    (must-have-same-length output-bits input-bits)
    (map not-gate! output-bits input-bits))
  (define (identity-words! output-bits input-bits)
    (must-have-same-length output-bits input-bits)
    (map identity-gate! output-bits input-bits))
  
  (define (must-have-same-length ls1 . lists)
    (let ((len (length ls1)))
      (for-each (lambda (ls) (unless (= (length ls) len) (error "must-have-same-length failed"))) lists)))
  
  (define (and-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map and-gate! output-bits inputs))
  (define (or-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map or-gate! output-bits inputs))
  (define (nand-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map nand-gate! output-bits inputs))
  (define (nor-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map nor-gate! output-bits inputs))
  (define (xor-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map xor-gate! output-bits inputs))
  (define (xnor-words! output-bits . inputs)
    (apply must-have-same-length output-bits inputs)
    (apply map xnor-gate! output-bits inputs))
  
  (define (equal-words! out-bit in1-bits in2-bits)
    (must-have-same-length in1-bits in2-bits)
    (let ((out-vars (variables! (length in1-bits))))
      (xnor-words! out-vars in1-bits in2-bits)
      (apply and-gate! out-bit out-vars)))
  
  (define (mux-words! output-bits sel-bit in-true in-false)
    (must-have-same-length output-bits in-true in-false)
    (for-each (lambda (out in-t in-f) (mux! out sel-bit in-t in-f)) output-bits in-true in-false))
  
  (define (constraints-satisfiable? cl)
    (reset!)
    (let* ((vars (make-hash-table))
           (get-var (lambda (v nbits)
                      (let ((p (hash-table-get vars v #f)))
                        (if p
                            p
                            (let ((bits (make-number-vars! nbits)))
                              ;(pretty-print `(making var ,v))
                              (hash-table-put! vars v bits)
                              bits))))))
      (letrec ((convert-expr (lambda (e)
                               (cond
                                 ((number? e) (as-bits 32 e))
                                 ((symbol? e) (get-var e 32))
                                 ((list? e)
                                  (let* ((arg-bits (map convert-expr (cdr e)))
                                         (tmpl (cons (car e) arg-bits)))
                                    (match tmpl
                                      (`(and . ,rest)
                                        (apply must-have-same-length rest)
                                        (apply map (lambda bits
                                                     (match (filter (lambda (x) x) bits)
                                                       (`() #f)
                                                       (`(,x) x)
                                                       (other (let ((v (variable!))) (apply and-gate! v other) v))))
                                               rest))
                                      (`(or . ,rest) (let ((result (make-number-vars! 32))) (apply or-words! result rest) result))
                                      (`(xor . ,rest) (let ((result (make-number-vars! 32))) (apply xor-words! result rest) result))
                                      (`(not ,a) (map inv a))
                                      (`(negate ,a) (let ((result (make-number-vars! 32))) (neg-words! result a) result))
                                      (`(add . ,rest)
                                       (let loop ((ls rest))
                                         (if (null? ls)
                                             (as-bits 32 0)
                                             (if (null? (cdr ls))
                                                 (car ls)
                                                 (let ((new-vars (make-number-vars! 32)))
                                                   (adder! new-vars (car ls) (cadr ls) #f)
                                                   (loop (cons new-vars (cddr ls))))))))
                                      (`(if-e ,p ,a ,b)
                                       (let ((p-bool (variable!)) (result (make-number-vars! 32)))
                                         (apply or-gate! p-bool p)
                                         (mux-words! result p-bool a b)
                                         result))
                                      (`(logical-and ,a ,b)
                                       (let ((v (variable!)) (v1 (variable!)) (v2 (variable!)))
                                         (apply or-gate! v1 a)
                                         (apply or-gate! v2 b)
                                         (and-gate! v v1 v2)
                                         (cons v (make-list 31 #f))))
                                      (`(logical-or ,a ,b)
                                        (let ((v (variable!)))
                                          (apply or-gate! v (append a b))
                                          (cons v (make-list 31 #f))))
                                      (`(logical-not ,a)
                                        (let ((v (variable!)))
                                          (apply nor-gate! v a)
                                          (cons v (make-list 31 #f))))
                                      (`(greater-than-or-equal ,a ,b)
                                        (let ((v (variable!)))
                                          (comparator! (inv v) a b)
                                          (cons v (make-list 31 #f))))
                                      (`(less-than ,a ,b)
                                        (let ((v (variable!)))
                                          (comparator! v a b)
                                          (cons v (make-list 31 #f))))
                                      (`(equal ,a ,b)
                                        (let ((v (variable!)))
                                          (equal-words! v a b)
                                          (cons v (make-list 31 #f))))
                                      (`(parity ,a)
                                        (let ((v (variable!)))
                                          (apply xnor-gate! v (take a 8)) ; 8 LSBs
                                          (cons v (make-list 31 #f))))
                                      (`(memoryReadByte ,mem ,addr)
                                       (append (variables! 8) (make-list 24 #f))) ; Force everything beyond lowest byte to 0, no constraints otherwise
                                      (`(memoryWriteByte ,mem ,addr ,data) (variables! 32)) ; No constraints, result will not be used
                                      (`(multiply ,a ,b)
                                       (cond
                                         ((number? (caddr e))
                                          (let make-mult ((n (caddr e)) (a a))
                                            (cond
                                              ((zero? n)
                                               (as-bits (length a) 0))
                                              ((= n 1) a)
                                              (else
                                                (let ((recur (make-mult (quotient n 2) (drop-right a 1))))
                                                  (if (odd? n)
                                                    (let ((temp (make-number-vars! (sub1 (length a)))))
                                                      ; temp = recur + (a / 2)
                                                      (adder! temp recur (cdr a) #f)
                                                      (cons (car a) temp)) ; temp * 2 + (a & 1)
                                                    (cons #f recur)))))))
                                         (else (error "multiply with non-constant RHS" e))))
                                      (`(mulhi32 ,a ,b)
                                       (variables! 32)) ; No constraints FIXME
                                      (`(left-shift ,a ,b)
                                       (cond
                                         ((number? (caddr e)) ; Constant count
                                          (let ((count (caddr e)))
                                            (append (as-bits count 0) (drop-right a count))))
                                         (else (error "left-shift with non-constant count" e))))
                                      (`(right-shift ,a ,b)
                                       (cond
                                         ((number? (caddr e)) ; Constant count
                                          (let ((count (caddr e)))
                                            (append (drop a count) (as-bits count 0))))
                                         (else (error "right-shift with non-constant count" e))))
                                      (_ (error "Bad operator" (car e))))))
                                 (else (error "convert-expr" e))))))
        (for-each (lambda (c)
                    ;(pretty-print `(converting ,c))
                    (match c
                      (`(not-equal ,a ,b)
                        (equal-words! #f (convert-expr a) (convert-expr b)))
                      (`(equal ,a ,b)
                       (if (or (not (symbol? a)) (hash-table-get vars a #f)) ; Shortcut for lets
                         (identity-words! (convert-expr a) (convert-expr b))
                         (hash-table-put! vars a (convert-expr b))))
                      (_ (error "Cannot handle constraint" c))))
                  (reverse cl))))
    (pretty-print `(running picosat with ,(variable!) variables))
    (let ((picosat-result
           (and (run-picosat) #t)))
      ;(pretty-print `(--> ,picosat-result))
      picosat-result))
  
  (provide adder!
           incrementer!
           comparator!
           neg-words!
           not-words!
           identity-words!
           as-bits
           set-number!
           get-number
           make-number-vars!
           and-words!
           or-words!
           nand-words!
           nor-words!
           xor-words!
           xnor-words!
           equal-words!
           mux-words!
           constraints-satisfiable?)
  
  (define (adder-test)
    (reset!)
    (let* ((n1 (make-number-vars! 5))
           (n2 (make-number-vars! 4))
           (n3 (make-number-vars! 3))
           (ci (variable!))
           (_ (adder! n1 n2 n3 ci))
           (_ (set-number! n1 15))
           (al (run-picosat))
           (addend1 (get-number n2 al))
           (addend2 (get-number n3 al))
           (ci-result (get-number (list ci) al)))
      (list addend1 addend2 ci-result)))

  )
