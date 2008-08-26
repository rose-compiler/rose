(module bitblast mzscheme
  (require "dimacs.ss")
  (require scheme/match)
  
  ; Words are LSB first, and outputs are truncated at length of output-bits
  (define (make-number-vars! count) (variables! count))
  
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
  
  (define (adder! output-bits in1-bits in2-bits carry-in-bit-or-false)
    (let loop ((outputs output-bits) (in1 in1-bits) (in2 in2-bits) (carry-opt carry-in-bit-or-false))
      (if (null? outputs)
          (void)
          (let ((all-inputs `(,@(if (null? in1) '() (list (car in1))) ,@(if (null? in2) '() (list (car in2))) ,@(if carry-opt (list carry-opt) '())))
                (carry-out (variable!)))
            (match all-inputs
              (`() (force-0! carry-out) (force-0! (car outputs)))
              (`(,in) (force-0! carry-out) (identity-gate! (car outputs) in))
              (`(,i1 ,i2) (and-gate! carry-out i1 i2) (xor-gate! (car outputs) i1 i2))
              (`(,i1 ,i2 ,i3)
               (majority-gate! carry-out i1 i2 i3)
               (xor-gate! (car outputs) i1 i2 i3))
              (_ (error "Too many inputs" all-inputs)))
            (loop (cdr outputs) (if (null? in1) in1 (cdr in1)) (if (null? in2) in2 (cdr in2)) carry-out)))))
  
  (define (incrementer! output-bits input-bits)
    (let loop ((outputs output-bits) (inputs input-bits) (first #t) (carry-in #f))
      (if (null? outputs)
          (void)
          (let ((carry-out (variable!)))
            (if (null? inputs)
                (begin (force-0! carry-out) (if first (force-1! (car outputs)) (force-0! (car outputs))))
                (if first
                    (begin (identity-gate! carry-out (car inputs))
                           (not-gate! (car outputs) (car inputs)))
                    (begin (and-gate! carry-out carry-in (car inputs))
                           (xor-gate! (car outputs) (car inputs) carry-in))))
            (loop (cdr outputs) (if (null? inputs) '() (cdr inputs)) #f carry-out)))))
  
  (define (neg-words! output-bits input-bits)
    (incrementer! output-bits (map - input-bits)))
  (define (not-words! output-bits input-bits)
    (must-have-same-length output-bits input-bits)
    (map not-gate! output-bits input-bits))
  
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
  
  (provide adder!
           incrementer!
           neg-words!
           not-words!
           set-number!
           get-number
           make-number-vars!
           and-words!
           or-words!
           nand-words!
           nor-words!
           xor-words!
           xnor-words!
           equal-words!)
  
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
