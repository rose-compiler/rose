(module bitblast mzscheme
  (require "dimacs.ss")
  (require scheme/match)
  (require scheme/pretty)
  (require srfi/1)
  
  ; Words are LSB first, and outputs are truncated at length of output-bits
  
  (define (as-bits count n)
    (let loop ((i 0) (n n))
      (if (= i count)
          (if (zero? n) '() (error "Too few bits used to represent number" n))
          (cons (odd? n) (loop (add1 i) (quotient n 2))))))
  
  (define (get-number input-bits al) ; al is result from from-picosat or run-picosat
    (let loop ((input-bits input-bits))
      (if (null? input-bits)
          0
          (let ((p (assq (car input-bits) al)))
            (if p
                (let ((rest (* 2 (loop (cdr input-bits)))))
                  (if (cdr p) (add1 rest) rest))
                (error "Trying get-number on unspecified bit" (car input-bits)))))))
  
  (define (set-number-size bits len)
    (let ((actual-len (length bits)))
      (cond
        ((= len actual-len) bits)
        ((> len actual-len) (append bits (make-list (- len actual-len) #f)))
        ((< len actual-len) (take bits len)))))
  
  (define (adder! in1-bits in2-bits carry-in-bit)
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
         (let ((carry-out (majority-gate! (car in1) (car in2) carry-in))
               (sum-out (xor-gate! (car in1) (car in2) carry-in)))
           (cons sum-out (loop (cdr in1) (cdr in2) carry-out))))))))
  
  (define (incrementer! input-bits)
    (adder! input-bits '() #t))
  
  (define (multiplier! in1-bits in2-bits)
    (let loop ((m in2-bits))
      (cond
        ((null? m) '())
        ((eq? (car m) #t)
         (adder! in1-bits (loop (cons #f (cdr m))) #f))
        ((eq? (car m) #f)
         (cons #f (loop (cdr m))))
        (else
         (adder! (map (lambda (in1b) (and-gate! (car m) in1b)) in1-bits)
                 (cons #f (loop (cdr m))))))))
  
  (define (comparator! in1 in2)
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
            (mux! (xor-gate! (car in1) (car in2))
                  (and-gate! (inv (car in1)) (car in2))
                  lower-bits-less)))))))
  
  (define (neg-words! input-bits)
    (incrementer! (map inv input-bits)))
  (define (not-words! input-bits) (map inv input-bits))
  
  (define (must-have-same-length ls1 . lists)
    (let ((len (length ls1)))
      (for-each (lambda (ls) (unless (= (length ls) len) (error "must-have-same-length failed"))) lists)))
  
  (define (and-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map and-gate! inputs))
  (define (or-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map or-gate! inputs))
  (define (nand-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map nand-gate! inputs))
  (define (nor-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map nor-gate! inputs))
  (define (xor-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map xor-gate! inputs))
  (define (xnor-words! . inputs)
    (apply must-have-same-length inputs)
    (apply map xnor-gate! inputs))
  
  (define (equal-words! in1-bits in2-bits)
    (must-have-same-length in1-bits in2-bits)
    (apply and-gate! (xnor-words! in1-bits in2-bits)))
  
  (define (mux-words! sel-bit in-true in-false)
    (must-have-same-length in-true in-false)
    (map (lambda (in-t in-f) (mux! sel-bit in-t in-f)) in-true in-false))
  
  (define (bits-in-type t)
    (match t
      ('uint32_t 32)
      ('int32_t 32)
      ('uint16_t 16)
      ('uint8_t 8)
      ('bool 1)
      (`(memory ,n) 0)
      (_ (error "bits-in-type" t))))
  
  (define (constraints-satisfiable? cl)
    (reset!)
    (let* ((vars (make-hash-table))
           (definitions (make-hash-table)))
      (letrec ((get-var (lambda (v t)
                          (let ((p (hash-table-get vars v #f)))
                            (if p
                                p
                                (let* ((def (hash-table-get definitions v #f))
                                       (bits
                                        (if def
                                            (convert-expr def)
                                            (variables! (bits-in-type t)))))
                                  (hash-table-put! vars v bits)
                                  bits)))))
               (convert-expr (lambda (p)
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
                                          (apply and-words! (map set-len rest)))
                                        (`(or . ,rest)
                                         (apply or-words! (map set-len rest)))
                                        (`(xor . ,rest)
                                         (apply xor-words! (map set-len rest)))
                                        (`(not ,a) (not-words! (set-len a)))
                                        (`(negate ,a) (set-number-size (neg-words! (set-len a)) (bits-in-type t)))
                                        (`(minus ,a) (set-number-size (neg-words! (set-len a)) (bits-in-type t)))
                                        (`(add . ,rest)
                                         (set-number-size
                                          (let loop ((ls (map set-len rest)))
                                            (if (null? ls)
                                                (as-bits (bits-in-type t) 0)
                                                (if (null? (cdr ls))
                                                    (car ls)
                                                    (loop (cons (adder! (car ls) (cadr ls) #f) (cddr ls))))))
                                          (bits-in-type t)))
                                        (`(subtract ,a ,b)
                                         (set-number-size
                                          (adder! (set-len a) (not-words! (set-len b)) #t)
                                          (bits-in-type t)))
                                        (`(if-e ,p ,a ,b)
                                         (mux-words! (apply or-gate! p) (set-len a) (set-len b)))
                                        (`(logical-and ,a ,b)
                                         (list (and-gate! (apply or-gate! a) (apply or-gate! b))))
                                        (`(logical-or ,a ,b)
                                         (list (apply or-gate! (append a b))))
                                        (`(logical-not ,a)
                                         (list (apply nor-gate! a)))
                                        (`(greater-than-or-equal ,a ,b)
                                         (list (inv (comparator! (set-len a) (set-len b)))))
                                        (`(less-than ,a ,b)
                                         (list (comparator! (set-len a) (set-len b))))
                                        (`(equal ,a ,b)
                                         (list (equal-words! (set-len a) (set-len b))))
                                        (`(not-equal ,a ,b)
                                         (list (inv (equal-words! (set-len a) (set-len b)))))
                                        (`(parity ,a)
                                         (list (apply xnor-gate! (take a 8)))) ; 8 LSBs
                                        (`(memoryReadByte ,mem ,addr)
                                         (set-number-size (variables! 8) (bits-in-type t)))
                                        (`(memoryWriteByte ,mem ,addr ,data) '())
                                        (`(multiply ,a ,b)
                                         (set-number-size (multiplier! a b) (bits-in-type t)))
                                        (`(mulhi32 ,a ,b)
                                         (set-number-size (drop (multiplier! a b) 32) 32))
                                        (`(imulhi32 ,a ,b)
                                         (variables! 32)) ; No constraints
                                        (`(left-shift ,a ,b)
                                         (cond
                                           ((number? (cdr (caddr e))) ; Constant count
                                            (let ((count (cdr (caddr e))))
                                              (append (as-bits count 0) (drop-right a count))))
                                           (else (error "left-shift with non-constant count" e))))
                                        (`(right-shift ,a ,b)
                                         (cond
                                           ((number? (cdr (caddr e))) ; Constant count
                                            (let ((count (cdr (caddr e))))
                                              (append (drop a count) (as-bits count 0))))
                                           (else (error "right-shift with non-constant count" e))))
                                        (_ (error "Bad operator" (car e))))))
                                   (else (error "convert-expr" e))))))
                                     ;(pretty-print `(convert-expr -> ,result))
                                     (set-number-size result (bits-in-type t))))))))
        (for-each (lambda (c)
                    ;(pretty-print `(converting ,c))
                    (match c
                      (`(define ,a ,b)
                       (unless (and (symbol? a)
                                    (not (hash-table-get vars a #f))
                                    (not (hash-table-get definitions a #f)))
                         (error "define symbol invalid or already exists" a))
                       (hash-table-put! definitions a b))
                      (`(check ,e)
                       (force-1! (apply or-gate! (convert-expr e))))
                      (_ (error "Cannot handle constraint" c))))
                  (reverse cl)))
      (display `(running picosat with ,(variable!) variables ,(hash-table-count vars) of ,(hash-table-count definitions) user-vars))
      (set! vars '()) ; Save space
      (set! definitions '())
      (let ((picosat-result
             (and (run-picosat) #t)))
        (pretty-print `(--> ,(if picosat-result 'SAT 'UNSAT)))
        picosat-result)))

  (define (possible-boolean-values constraints to-test)
    (let ((can-be-true (constraints-satisfiable? (cons `(check ,to-test) constraints)))
          (can-be-false (constraints-satisfiable? (cons `(check (bool logical-not ,to-test)) constraints))))
      (if can-be-true
          (if can-be-false
              '*
              #t)
          (if can-be-false
              #f
              (error "Inconsistent constraints" `(,to-test . ,constraints))))))

  (define (is-constant? constraints t typed-expr)
    ;(pretty-print `(is-constant? ,t ,typed-expr))
    (let ((nbits (bits-in-type t)))
      (let loop ((shift-amount 0))
        (if (= shift-amount nbits)
            0
            (let ((vals (possible-boolean-values constraints
                                                 `(bool not-equal (,t . 0) (,t and (,t . ,(expt 2 shift-amount)) ,typed-expr)))))
              (if (boolean? vals)
                  (let ((rest (loop (add1 shift-amount))))
                    (if (number? rest)
                        (+ (if vals 1 0) (* 2 rest))
                        #f))
                  #f))))))
  
  (provide adder!
           multiplier!
           incrementer!
           comparator!
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
           constraints-satisfiable?
           possible-boolean-values
           is-constant?)

  )
