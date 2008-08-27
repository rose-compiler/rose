(require scheme/pretty)
(require scheme/match)
(require srfi/1)
(require "bitblast.ss")

(define data (with-input-from-file "/home/willcock2/rose-svn-checkout/to-build/projects/assemblyToSourceAst/fnord.ss" read))

(define (simplify-begins t)
  (match t
    (`(begin . ,ls)
     (let* ((fixed-ls (map simplify-begins ls))
            (fixed-ls2 (append-map (match-lambda (`(begin . ,ls2) ls2) (x (list x))) fixed-ls)))
       (if (= (length fixed-ls2) 1)
           (car fixed-ls2)
           `(begin . ,fixed-ls2))))
    (`(expr . ,_) t)
    (`(parallel-assign . ,_) t)
    (`(goto . ,_) t)
    (`(label ,_) t)
    (`(label ,l ,body) `(label ,l ,(simplify-begins body)))
    (`(if ,p ,a ,b) `(if ,p ,(simplify-begins a) ,(simplify-begins b)))
    (`(if ,p ,a) `(if ,p ,(simplify-begins a) (begin)))
    (`(let ((,t ,var ,val)) . ,body*)
     (let ((fixed-body (simplify-begins `(begin . ,body*))))
       `(let ((,t ,var ,val)) ,fixed-body)))
    (`(case ,expr . ,body*) `(case ,expr . ,body*))
    (_ (error "Bad input" t))))

(set! data (simplify-begins data))

(define (add-bodies-to-labels e)
  (match e
    (`(begin . ,ls)
     `(begin . 
             ,(let loop ((ls ls))
                (if (null? ls)
                    '()
                    (match (car ls)
                      (`(label ,l)
                       (let-values (((this-label rest)
                                     (let loop2 ((ls (cdr ls)))
                                       (if (null? ls)
                                           (values '() '())
                                           (match (car ls)
                                             (`(label ,l2)
                                              (values `((goto ,l2)) ls))
                                             (x
                                              (let-values (((this-label2 rest2)
                                                            (loop2 (cdr ls))))
                                                (values (cons x this-label2) rest2))))))))
                         `((label ,l (begin . ,this-label)) . ,(loop rest))))
                      (x `(,x . ,(loop (cdr ls)))))))))
    (_ (error "Bad e" e))))

(set! data (simplify-begins (add-bodies-to-labels data)))

(define (forbid-labels e)
  (match e
    (`(label . ,_) (error "forbid-labels" e))
    (`(goto . ,_) (void))
    (`(expr . ,_) (void))
    (`(if ,p . ,rest) (for-each forbid-labels rest))
    (`(let ,vars ,body) (forbid-labels body))
    (`(begin . ,rest) (for-each forbid-labels rest))
    (`(case ,e . ,rest) (for-each (match-lambda (`(,keys ,body) (forbid-labels body))) rest))
    (_ (error "forbid-errors unrecognized" e))))

(match data (`(begin . ,rest)
             (for-each (match-lambda
                         (`(label ,l ,body) (forbid-labels body))
                         (e (forbid-labels e)))
                       rest)))

(define (conditionalize e cond negate)
  (match e
    (`(begin . ,ls) `(begin . ,(map (lambda (e2) (conditionalize e2 cond negate)) ls)))
    (`(expr (assign ,e1 ,e2))
     (if negate
         `(expr (assign ,e1 (if-e ,cond ,e1 ,e2)))
         `(expr (assign ,e1 (if-e ,cond ,e2 ,e1)))))
    (_ (error "Cannot conditionalize" e))))

(define (split-ifs e)
  (match e
    (`(if ,_ (goto ,_) (goto ,_)) e)
    (`(if ,_ (goto ,_)) e)
    (`(if (expr ,p) ,a ,b)
     (let ((var (gensym 'pred)))
       `(let ((bool ,var ,p))
          (begin
            ,(conditionalize a var #f)
            ,(conditionalize b var #t)))))
    (`(if (expr ,p) ,a)
     (let ((var (gensym 'pred)))
       `(let ((bool ,var ,p))
          ,(conditionalize a var #f))))
    (`(begin . ,rest) `(begin . ,(map split-ifs rest)))
    (`(expr . ,_) e)
    (`(let ,vars ,body) `(let ,vars ,(split-ifs body)))
    (`(label ,l ,body) `(label ,l ,(split-ifs body)))
    (`(goto ,l) e)
    (`(case ,k . ,body*) `(case ,k . ,(map (match-lambda (`(,keys ,body) `(,keys ,(split-ifs body)))) body*)))
    (_ (error "split-ifs" e))))

;(set! data (simplify-begins (split-ifs data)))

(set! data (cdr data)) ; Remove starting begin

(define (change-to-pure-memory e)
  (letrec ((change-expr
            (lambda (e)
              (cond
                ((symbol? e) e)
                ((number? e) e)
                ((boolean? e) e)
                ((list? e)
                 (match e
                   (`(memoryReadDWord ,a)
                    `(or (memoryReadByte memory ,(change-expr a))
                         (left-shift (memoryReadByte memory (add 1 ,(change-expr a))) 8)
                         (left-shift (memoryReadByte memory (add 2 ,(change-expr a))) 16)
                         (left-shift (memoryReadByte memory (add 3 ,(change-expr a))) 24)))
                   (`(memoryReadWord ,a)
                    `(or (memoryReadByte memory ,(change-expr a))
                         (left-shift (memoryReadByte memory (add 1 ,(change-expr a))) 8)))
                   (`(memoryReadByte ,a) `(memoryReadByte memory ,(change-expr a)))
                   (`(,f . ,args) `(,f . ,(map change-expr args)))))
                (else (error "change-expr" e))))))
  (match e
    (`(label ,l ,body)
     `(label ,l ,(change-to-pure-memory body)))
    (`(goto ,l) `(goto ,l))
    (`(if ,p . ,rest) `(if ,(change-expr p) . ,rest))
    (`(case ,key . ,rest) `(case ,(change-expr key) . ,rest))
    (`(begin . ,rest) `(begin . ,(map change-to-pure-memory rest)))
    (`(let ((,t ,var ,val)) ,body) `(let ((,t ,var ,(change-expr val))) ,(change-to-pure-memory body)))
    (`(expr (memoryWriteDWord ,a ,d))
     `(begin
        (expr (assign memory (memoryWriteByte memory ,(change-expr a) (and 255 ,(change-expr d)))))
        (expr (assign memory (memoryWriteByte memory (add 1 ,(change-expr a)) (and 255 (right-shift ,(change-expr d) 8)))))
        (expr (assign memory (memoryWriteByte memory (add 2 ,(change-expr a)) (and 255 (right-shift ,(change-expr d) 16)))))
        (expr (assign memory (memoryWriteByte memory (add 3 ,(change-expr a)) (and 255 (right-shift ,(change-expr d) 24)))))))
    (`(expr (memoryWriteWord ,a ,d))
     `(begin
        (expr (assign memory (memoryWriteByte memory ,(change-expr a) (and 255 ,(change-expr d)))))
        (expr (assign memory (memoryWriteByte memory (add 1 ,(change-expr a)) (and 255 (right-shift ,(change-expr d) 8)))))))
    (`(expr (memoryWriteByte ,a ,d))
     `(expr (assign memory (memoryWriteByte memory ,(change-expr a) ,(change-expr d)))))
    (`(expr ,e) `(expr ,(change-expr e)))
    (_ (error "change-to-pure-memory" e)))))

(set! data (map change-to-pure-memory data))

(define (infer-type expr tenv)
  (let ((arith-combine-2 (match-lambda*
                           (`(,x ,x) x)
                           (`(int32_t uint32_t) 'uint32_t)
                           (`(uint32_t int32_t) 'uint32_t)
                           (`(uint64_t uint32_t) 'uint64_t)
                           (`(uint32_t uint64_t) 'uint64_t)
                           (`(uint32_t uint8_t) 'uint32_t)
                           (`(uint32_t uint16_t) 'uint32_t)
                           (`(uint8_t uint32_t) 'uint32_t)
                           (`(uint16_t uint32_t) 'uint32_t)
                           (`(uint8_t uint16_t) 'uint16_t)
                           (`(uint16_t uint8_t) 'uint16_t)
                           (pr (error "arith-combine-2" pr))))
        (type-of-register (lambda (r)
                            (case r
                              ((rax rbx rcx rdx rsi rdi rbp rsp) 'uint32_t)
                              ((df sf of pf cf af zf sf_xor_of zf_or_cf) 'bool)
                              ((memory) '(memory 0))
                              (else (error "type-of-register" r))))))
    (letrec ((arith-combine (lambda t*
                              (cond
                                ((null? t*) (error "arith-combine empty list"))
                                ((null? (cdr t*)) (car t*))
                                (else (arith-combine-2 (car t*) (apply arith-combine (cdr t*))))))))
      (let loop ((expr expr))
        (match expr
          ((? symbol?)
           (let ((p (assq expr tenv)))
             (if p
                 (cdr p)
                 (type-of-register expr))))
          ((? number?) 'uint32_t)
          ((? boolean?) 'bool)
          (`(logical-not ,_) 'bool)
          (`(logical-and . ,_) 'bool)
          (`(logical-or . ,_) 'bool)
          (`(add . ,terms) (apply arith-combine (map loop terms)))
          (`(subtract ,a ,b) (arith-combine (loop a) (loop b)))
          (`(multiply ,a ,b) (arith-combine (loop a) (loop b)))
          (`(mulhi32 ,_ ,_) 'uint32_t)
          (`(imulhi32 ,_ ,_) 'uint32_t)
          (`(div32 ,_ ,_ ,_) 'uint32_t)
          (`(mod32 ,_ ,_ ,_) 'uint32_t)
          (`(idiv32 ,_ ,_ ,_) 'uint32_t)
          (`(imod32 ,_ ,_ ,_) 'uint32_t)
          (`(mulhi16 ,_ ,_) 'uint16_t)
          (`(imulhi16 ,_ ,_) 'uint16_t)
          (`(div16 ,_ ,_ ,_) 'uint16_t)
          (`(mod16 ,_ ,_ ,_) 'uint16_t)
          (`(idiv16 ,_ ,_ ,_) 'uint16_t)
          (`(imod16 ,_ ,_ ,_) 'uint16_t)
          (`(mulhi8 ,_ ,_) 'uint8_t)
          (`(imulhi8 ,_ ,_) 'uint8_t)
          (`(div8 ,_ ,_) 'uint8_t)
          (`(mod8 ,_ ,_) 'uint8_t)
          (`(idiv8 ,_ ,_) 'uint8_t)
          (`(imod8 ,_ ,_) 'uint8_t)
          (`(int32_t ,_) 'int32_t)
          (`(and . ,terms) (apply arith-combine (map loop terms)))
          (`(or . ,terms) (apply arith-combine (map loop terms)))
          (`(xor . ,terms) (apply arith-combine (map loop terms)))
          (`(bsr ,_) 'uint32_t)
          (`(bsf ,_) 'uint32_t)
          (`(not ,a) (loop a))
          (`(minus ,a) (loop a))
          (`(left-shift ,a ,_) (loop a))
          (`(right-shift ,a ,_) (loop a))
          (`(equal ,a ,b) 'bool)
          (`(not-equal ,a ,b) 'bool)
          (`(less-than ,a ,b) 'bool)
          (`(greater-than-or-equal ,a ,b) 'bool)
          (`(parity ,_) 'bool)
          (`(memoryReadByte ,_ ,_) 'uint32_t)
          (`(memoryWriteByte ,m ,_ ,_)
           (match (loop m)
             (`(memory ,i) `(memory ,(add1 i)))
             (mt (error "Bad memory type" mt))))
          (`(if-e ,p ,a ,b) (arith-combine (loop a) (loop b)))
          (_ (error "infer-type" expr)))))))

(define (fold-lets e)
  (letrec ((loop
            (lambda (e al as tenv) ; as is list of assignment pairs
              (match e
                (`(begin) (writeout as))
                (`(begin (begin . ,rest1) . ,rest2) (loop `(begin ,@rest1 ,@rest2) al as tenv))
                (`(begin (let ,vars ,body) . ,rest) (loop `(let ,vars (begin ,body . ,rest)) al as tenv))
                (`(begin (if ,p ,a ,b) . ,rest)
                 (loop `(if ,p (begin ,a . ,rest) (begin ,b . ,rest)) al as tenv))
                (`(let ((,t ,var ,val)) ,body)
                 (let ((new-var (gensym var))
                       (actual-val-type (infer-type val tenv)))
                   `(let ((,t ,new-var ,(remap val al)))
                      ,(loop body
                             `((,var . ,new-var) . ,al)
                             as
                             `((,new-var . ,t) (,var . ,t) . ,tenv)))))
                (`(if ,p ,a ,b)
                 `(if ,(remap p al)
                      ,(loop a al as tenv)
                      ,(loop b al as tenv)))
                (`(begin (expr (assign ,a ,b)) . ,rest)
                 (let* ((new-var (gensym a))
                        (new-b (remap b al))
                        (new-type (infer-type new-b tenv)))
                   `(let ((,new-type ,new-var ,new-b))
                      ,(loop `(begin . ,rest)
                             `((,a . ,new-var) . ,al)
                             `((,a . ,new-var) . ,as)
                             `((,new-var . ,new-type) . ,tenv)))))
                (`(begin (parallel-assign ,as2) . ,rest)
                 (let ((new-var* (map (match-lambda (`(,a . ,b) (gensym a))) as2)))
                   (let l ((asx as2) (nv new-var*)
                                     (t* (map (match-lambda (`(,a . ,b) (infer-type a tenv))) as2)))
                     (if (null? asx)
                         (loop `(begin . ,rest)
                               (append (map (match-lambda* (`((,a . ,b) ,n) (cons a n))) as2 new-var*) al)
                               (append (map (match-lambda* (`((,a . ,b) ,n) (cons a n))) as2 new-var*) as)
                               (append (map (match-lambda* (`((,a . ,b) ,t) (cons b t))) as2 t*) tenv))
                         `(let ((,(infer-type (car nv) tenv) ,(car nv) ,(remap (cdar asx) al)))
                            ,(l (cdr asx) (cdr nv)))))))
                (`(expr . ,_) (loop `(begin ,e) al as tenv))
                (`(parallel-assign ,_) (loop `(begin ,e) al as tenv))
                (`(case ,key . ,cases)
                 `(case ,(remap key al)
                    ,@(map (match-lambda (`(,k ,body) `(,k ,(loop body al as tenv)))) cases)))
                (`(begin (expr (abort)) . ,_) `(expr (abort)))
                (`(begin (expr (interrupt ,i)) . ,rest)
                 `(begin (expr (interrupt ,i)) ,(loop `(begin . ,rest) al as tenv)))
                (`(begin (case ,key . ,cases) . ,rest)
                 (loop `(case ,key . ,(map (match-lambda (`(,k ,body) `(,k (begin ,body . ,rest)))) cases))
                       al as tenv))
                (`(goto ,l) `(begin ,(writeout as) (goto ,l)))
                (`(label ,l ,body) `(label ,l ,(loop body al as tenv)))
                (`(begin (goto ,l) . ,_) (loop `(goto ,l) al as tenv))
                (`(begin ,x) (loop x al as tenv))
                (_ (error "fold-lets-3" e)))))
           (remap (lambda (e al)
                    (cond
                      ((symbol? e) (let ((p (assq e al))) (if p (cdr p) e)))
                      ((list? e) (cons (car e) (map (lambda (x) (remap x al)) (cdr e))))
                      (else e))))
           (writeout (lambda (as)
                       (if (null? as)
                           '(begin)
                           `(parallel-assign
                             ,(let l ((as as))
                                (if (null? as)
                                    '()
                                    (let ((a (caar as)) (b (cdar as)))
                                      `((,a . ,b)
                                        .
                                        ,(l (filter (match-lambda (`(,a2 . ,_) (not (eq? a a2))))
                                                    (cdr as))))))))))))
    (loop e '() '() '())))

(set! data (map fold-lets data))

(define (copy-and-constant-prop e al simple?)
  ;(pretty-print `(copy-and-constant-prop ,e ,al ,simple?))
  (letrec ((subst-expr
            (lambda (e al)
              (if (list? e)
                  (cons (car e) (map (lambda (e2) (subst-expr e2 al)) (cdr e)))
                  (if (symbol? e)
                      (let ((p (assq e al))) (if p (cddr p) e))
                      e)))))
  (match e
    (`(begin . ,rest) `(begin . ,(map (lambda (e) (copy-and-constant-prop e al simple?)) rest)))
    (`(let ((,t ,var ,val)) ,body)
     (let ((val (subst-expr val al)))
       (if (simple? val)
           (copy-and-constant-prop body `((,var ,t . ,val) . ,al) simple?)
           `(let ((,t ,var ,val)) ,(copy-and-constant-prop body al simple?)))))
    (`(expr ,e) `(expr ,(subst-expr e al)))
    (`(parallel-assign ,al2) `(parallel-assign ,(map (match-lambda (`(,e1 . ,e2) `(,(subst-expr e1 al) . ,(subst-expr e2 al)))) al2)))
    (`(label ,l ,body) `(label ,l ,(copy-and-constant-prop body al simple?)))
    (`(goto . ,_) e)
    (`(if ,p ,a ,b) `(if ,(subst-expr p al) ,(copy-and-constant-prop a al simple?) ,(copy-and-constant-prop b al simple?)))
    (`(case ,key . ,cases) `(case ,(subst-expr key al)
                              ,@(map (match-lambda (`(,k* ,body) `(,k* ,(copy-and-constant-prop body al simple?))))
                                     cases)))
    (_ (error "copy-and-constant-prop" e)))))

(define (basic-simple? x) (or (number? x) (boolean? x) (symbol? x)))
(define (always-simple? x) #t)

(set! data (map (lambda (e) (copy-and-constant-prop e '() basic-simple?)) data))

(define (change-to-let* e)
  (match e
    (`(begin . ,rest) `(begin . ,(map change-to-let* rest)))
    (`(let ,vars ,body)
     (match (change-to-let* body)
       (`(let* ,vars2 ,body2) `(let* ,(append vars vars2) ,body2))
       (b `(let* ,vars ,b))))
    (`(expr (assign ,a ,b)) e)
    (`(parallel-assign . ,al) e)
    (`(expr (abort)) e)
    (`(expr (interrupt ,_)) e)
    (`(goto . ,_) e)
    (`(if ,p ,a ,b) `(if ,p ,(change-to-let* a) ,(change-to-let* b)))
    (`(case ,key . ,cases) `(case ,key . ,(map (match-lambda (`(,k* ,body) `(,k* ,(change-to-let* body)))) cases)))
    (`(label ,l ,body) `(label ,l ,(change-to-let* body)))
    (_ (error "change-to-let*" e))))

; (set! data (map change-to-let* data))

(define (is-used-variable? var e)
  (letrec ((g (lambda (e)
                (cond
                  ((symbol? e) (eq? e var))
                  ((list? e) (ormap g (cdr e)))
                  (else #f)))))
    (let loop ((e e))
      (match e
        (`(begin . ,rest) (ormap loop rest))
        (`(let ((,t ,var2 ,val)) ,body)
         (or
          (g val)
          (and (not (eq? var var2))
               (loop body))))
        (`(goto ,_) #f)
        (`(if ,p ,a ,b) (or (g p) (loop a) (loop b)))
        (`(case ,key . ,cases)
         (or (g key) (ormap (match-lambda (`(,k ,body) (loop body))) cases)))
        (`(parallel-assign ,al) (ormap g (map cdr al)))
        (`(expr ,e) (g e))
        (_ (error "is-used-variable?" e))))))

(define (remove-unused-variables e)
  (match e
    (`(begin . ,rest) `(begin . ,(map remove-unused-variables rest)))
    (`(let ((,t ,var ,val)) ,body)
     (if (is-used-variable? var body)
         `(let ((,t ,var ,val)) ,(remove-unused-variables body))
         (remove-unused-variables body)))
    (`(goto ,_) e)
    (`(label ,l ,body) `(label ,l ,(remove-unused-variables body)))
    (`(if ,p ,a ,b) `(if ,p ,(remove-unused-variables a) ,(remove-unused-variables b)))
    (`(case ,key . ,cases)
     `(case ,key . ,(map (match-lambda (`(,k ,body) `(,k ,(remove-unused-variables body)))) cases)))
    (`(parallel-assign ,al) e)
    (`(expr ,_) e)
    (_ (error "remove-unused-variables" e))))

(define (split-and-value-number e)
  ;(pretty-print `(split-and-value-number ,e))
  (letrec ((split-expr
            (lambda (expr exprs-known k)
              ;(pretty-print `(split-expr ,expr ,exprs-known))
              (cond
                ((and (list? expr) (ormap list? (cdr expr)))
                 (let* ((first-list (find list? (cdr expr))))
                   (split-expr first-list
                               exprs-known
                               (lambda (fl2 ek2)
                                 (let ((plugged-in (cons (car expr)
                                                         (map (lambda (e)
                                                                (if (equal? e first-list) fl2 e))
                                                              (cdr expr)))))
                                   (split-expr plugged-in ek2 k))))))
                  ((assoc expr exprs-known) => (lambda (p) (k (cddr p) exprs-known)))
                  ((list? expr)
                   (let* ((var (gensym 'temp))
                          (type (infer-type expr
                                            (map (match-lambda (`(,expr ,t . ,name) (cons name t)))
                                                 exprs-known)))
                          (new-exprs-known (cons (cons* expr type var) exprs-known)))
                     `(let ((,type ,var ,expr)) ,(k var new-exprs-known))))
                  (else (k expr exprs-known)))))
             (split-in-stmt
              (lambda (e exprs-known)
                ;(pretty-print `(split-in-stmt ,(length exprs-known)))
                (match e
                  (`(label ,l ,body)
                   `(label ,l ,(split-in-stmt body exprs-known)))
                  (`(let ((,t ,v ,def)) ,body)
                   (split-expr def exprs-known
                               (lambda (def2 ek2)
                                 (let ((exprs-known (cons (cons* def2 t v) ek2)))
                                   `(let ((,t ,v ,def2)) ,(split-in-stmt body exprs-known))))))
                  (`(expr (assign ,a ,b))
                   (split-expr b exprs-known (lambda (b2 _) `(expr (assign ,a ,b2)))))
                  (`(expr (abort)) e)
                  (`(expr (interrupt ,i)) e)
                  (`(parallel-assign ,pairs)
                   (let loop ((pairs pairs) (acc-rev '()) (exprs-known exprs-known))
                     (match pairs
                       (`() `(parallel-assign ,(reverse acc-rev)))
                       (`((,var1 . ,def1) . ,rest)
                        (split-expr def1
                                    exprs-known
                                    (lambda (def2 ek2)
                                      (loop rest `((,var1 . ,def2) . ,acc-rev) ek2)))))))
                  (`(begin . ,ls) `(begin . ,(map (lambda (s) (split-in-stmt s exprs-known)) ls)))
                  (`(if (expr ,p) ,a ,b)
                   (split-expr p exprs-known
                               (lambda (p2 ek2)
                                 `(if (expr ,p2)
                                      ,(split-in-stmt a ek2)
                                      ,(split-in-stmt b ek2)))))
                  (`(case (expr ,p) . ,cases)
                   (split-expr p exprs-known
                               (lambda (p2 ek2)
                                 `(case (expr ,p2)
                                    . ,(map (match-lambda
                                              (`(,keys ,body)
                                               `(,keys ,(split-in-stmt body ek2))))
                                            cases)))))
                  (`(goto ,l) `(goto ,l))
                  (else (error "split-in-stmt" e))))))
      (split-in-stmt e '())))

(define ((term-with-functor? f) t)
  (and (list? t) (eq? (car t) f)))

(define ((associative-list f id) t)
  (if ((term-with-functor? f) t) (cdr t) (if (equal? t id) '() (list t))))

(define ((flatten-assoc f id) t)
  (let ((result
         (if ((term-with-functor? f) t)
             (let ((ls (append-map (associative-list f id) (cdr t))))
               (cond
                 ((null? ls) id)
                 ((null? (cdr ls)) (car ls))
                 (else (cons f ls))))
             t)))
    (if (equal? result t) #f result)))

(define ((flatten-idem f) t)
  (and ((term-with-functor? f) t)
       (let ((tl (delete-duplicates (cdr t) equal?)))
         (if (equal? tl (cdr t))
             #f
             (cons (car t) tl)))))

(define ((flatten-zero f z) t)
  (and ((term-with-functor? f) t)
       (member z (cdr t))
       z))

(define (remove-all ls1 ls2) ; Like lset-difference but handles multisets properly
  (let loop ((ls1 ls1) (ls2 ls2))
    (if (null? ls2)
        ls1
        (loop
         (let remove ((ls ls1))
           (if (null? ls) '() (if (equal? (car ls) (car ls2)) (cdr ls) (cons (car ls) (remove (cdr ls))))))
         (cdr ls2)))))

(define (simplify-expr e)
  (letrec ((simp
            (lambda (e)
              (if (boolean? e)
                  (if e 1 0)
                  (or
                   (match e (`(subtract ,a ,b) `(add ,a (negate ,b))) (_ #f))
                   (match e (`(negate ,x) (and (number? x) (if (zero? x) 0 (- (expt 2 32) x)))) (_ #f))
                   (match e (`(negate (negate ,x)) x) (_ #f))
                   ((flatten-assoc 'add 0) e)
                   ((flatten-assoc 'and (- (expt 2 32) 1)) e)
                   ((flatten-zero 'and 0) e)
                   ((flatten-idem 'and) e)
                   ((flatten-assoc 'or 0) e)
                   ((flatten-idem 'or) e)
                   ((flatten-zero 'or (- (expt 2 32) 1)) e)
                   ((flatten-assoc 'xor 0) e)
                   ((flatten-assoc 'logical-and 1) e)
                   ((flatten-idem 'logical-and) e)
                   ((flatten-zero 'logical-and 0) e)
                   ((flatten-assoc 'logical-or 0) e)
                   ((flatten-idem 'logical-or) e)
                   ((flatten-zero 'logical-or 1) e)
                   (match e (`(logical-not ,x) (and (number? x) (if (zero? x) 1 0))) (_ #f))
                   (match e (`(if-e ,p ,a ,b) (and (number? p) (if (zero? p) b a))) (_ #f))
                   (match e (`(xor ,a ,b) (and (equal? a b) 0)) (_ #f))
                   (match e (`(left-shift ,a ,b) (and (number? a) (number? b) (arithmetic-shift a b))) (_ #f))
                   (match e (`(right-shift ,a ,b) (and (number? a) (number? b) (arithmetic-shift a (- b)))) (_ #f))
                   (match e (`(memoryWriteByte ,m1 ,a (memoryReadByte ,m2 ,b)) (and (equal? m1 m2) (equal? a b) m2)) (_ #f))
                   (match e
                     (`(memoryReadByte (memoryWriteByte ,m ,a ,v) ,b)
                      `(if-e (equal ,a ,b)
                             ,v
                             (memoryReadByte ,m ,b)))
                     (_ #f))
                   (match e
                     (`(add . ,ls) (and (>= (length (filter number? ls)) 2)
                                        `(add ,@(filter (lambda (x) (not (number? x))) ls)
                                              ,(remainder (apply + (filter number? ls)) (expt 2 32)))))
                     (_ #f))
                   (match e
                     (`(and . ,ls) (and (>= (length (filter number? ls)) 2)
                                        `(and ,@(filter (lambda (x) (not (number? x))) ls)
                                              ,(apply bitwise-and (filter number? ls)))))
                     (_ #f))
                   (match e
                     (`(xor . ,ls) (and (>= (length (filter number? ls)) 2)
                                        `(xor ,@(filter (lambda (x) (not (number? x))) ls)
                                              ,(apply bitwise-xor (filter number? ls)))))
                     (_ #f))
                   (match e
                     (`(equal ,x ,y)
                      (let* ((x-ls ((associative-list 'add 0) x))
                             (y-ls ((associative-list 'add 0) y))
                             (isect (lset-intersection equal? x-ls y-ls)))
                        (and (not (null? isect))
                             (let ((x-ls-minus-isect (remove-all x-ls isect))
                                   (y-ls-minus-isect (remove-all y-ls isect)))
                               `(equal (add . ,x-ls-minus-isect) (add . ,y-ls-minus-isect))))))
                     (_ #f))
                   (match e 
                     (`(equal ,a ,b)
                      (cond
                        ((equal? a b) 1)
                        ((and (number? a) (number? b)) (if (= a b) 1 0))
                        (else #f)))
                     (_ #f))
                   (match e 
                     (`(less-than ,a ,b)
                      (cond
                        ((equal? a b) 0)
                        ((and (number? a) (number? b)) (if (< a b) 1 0))
                        ((and (number? b) (zero? b)) 0)
                        (else #f)))
                     (_ #f))
                   (match e (`(not-equal ,a ,b) `(logical-not (equal ,a ,b))) (_ #f))
                   (match e
                     (`(parity ,n) (and (number? n)
                                        (let parity ((n (bitwise-and n 255)))
                                          (cond
                                            ((zero? n) 1)
                                            ((odd? n) (- 1 (parity (/ (sub1 n) 2))))
                                            (else (parity (/ n 2)))))))
                     (_ #f))
                   (if (list? e) (cons (car e) (map simp (cdr e))) e))))))
    (let loop ((e e))
      (let ((new-e (simp e)))
        (if (equal? e new-e) new-e (loop new-e))))))

(define (simplify e)
  (match e
    (`(begin . ,rest) `(begin . ,(map simplify rest)))
    (`(let ((,t ,var ,val)) ,body)
     `(let ((,t ,var ,(simplify-expr val))) ,(simplify body)))
    (`(goto ,_) e)
    (`(label ,l ,body) `(label ,l ,(simplify body)))
    (`(if ,p ,a ,b) `(if ,(simplify p) ,(simplify a) ,(simplify b)))
    (`(case ,key . ,cases)
     `(case ,(simplify key) . ,(map (match-lambda (`(,k ,body) `(,k ,(simplify body)))) cases)))
    (`(parallel-assign ,al)
     `(parallel-assign ,(map (match-lambda (`(,lhs . ,rhs) `(,lhs . ,(simplify-expr rhs)))) al)))
    (`(expr ,e) `(expr ,(simplify-expr e)))
    (_ (error "simplify" e))))

(define (init-code) `(begin . ,(filter (match-lambda (`(label . ,_) #f) (_ #t)) data)))
(define (get-label l) (or (ormap (match-lambda (`(label ,l2 ,body) (if (eq? l l2) body #f)) (_ #f)) data) (error "Label not found" l)))

(define (expand-one e)
  (match e
    (`(goto ,l) (get-label l))
    (`(label ,l ,body) `(label ,l ,(expand-one body)))
    (`(if ,p ,a ,b) `(if ,p ,(expand-one a) ,(expand-one b)))
    (`(case ,k . ,cases) `(case ,k . ,(map (match-lambda (`(,keys ,e) `(,keys ,(expand-one e)))) cases)))
    (`(expr . ,_) e)
    (`(parallel-assign . ,_) e)
    (`(begin . ,rest) `(begin . ,(map expand-one rest)))
    (`(let ,vars ,body) `(let ,vars ,(expand-one body)))
    (_ (error "expand-one" e))))

(define (loop-simplify-and-ccp e)
  (let* ((e1 (simplify e))
         (e2 (copy-and-constant-prop e1 '() basic-simple?)))
    (if (equal? e2 e)
        e
        (loop-simplify-and-ccp e2))))

(define (memory-simple? x)
  (or (basic-simple? x)
      (and (list? x)
           (memq (car x) '(memoryWriteDWord memoryWriteWord memoryWriteByte)))))

(define (simplify-full e)
  (remove-unused-variables
   (copy-and-constant-prop
    (split-and-value-number
     (loop-simplify-and-ccp
      (simplify-begins
       (remove-unused-variables
        (copy-and-constant-prop
         (fold-lets e)
         '()
         always-simple?)))))
    '()
    basic-simple?)))

(define (expand-one-simp e) (simplify-full (expand-one e)))

(define (expand-many n e)
  (if (zero? n) e (expand-one-simp (expand-many (sub1 n) e))))

(define (add-types e tenv)
  `(,(infer-type e tenv) . ,(if (list? e) 
                                (cons (car e) (map (lambda (e2) (add-types e2 tenv)) (cdr e)))
                                e)))

(define (check-constraints e)
  (letrec ((check-statement
            (lambda (e csf tenv)
              ;(pretty-print `(check-constraints ,e ,constraints-so-far))
              (match e
                (`(goto ,l) e)
                (`(begin . ,ls) `(begin . ,(map (lambda (e2) (check-statement e2 csf tenv)) ls)))
                (`(let ((,t ,var ,val)) ,body)
                 (let ((val2 (check-expr val csf tenv)))
                   `(let ((,t ,var ,val2))
                      ,(check-statement body
                                        (match t
                                          (`(memory ,_) csf) ; Do not add this as a constraint
                                          (_ `((define ,var ,(add-types val2 tenv)) . ,csf)))
                                        `((,var . ,t) . ,tenv)))))
                (`(if (expr ,p) ,a ,b)
                 (let* ((p (check-expr p csf tenv))
                        (p-type (infer-type p tenv))
                        (p-with-types (add-types p tenv))
                        (p-true-constraints `((check ,p-with-types) . ,csf))
                        (p-false-constraints `((check (bool logical-not ,p-with-types)) . ,csf))
                        (p-true (constraints-satisfiable? p-true-constraints))
                        (p-false (constraints-satisfiable? p-false-constraints)))
                     (if p-true
                         (if p-false
                             `(if (expr ,p)
                                  ,(check-statement a p-true-constraints tenv)
                                  ,(check-statement b p-false-constraints tenv))
                             (begin 
                               (pretty-print `(removing false branch ,b))
                               (check-statement a csf tenv)))
                         (if p-false
                             (begin
                               (pretty-print `(removing true branch ,a))
                               (check-statement b csf tenv))
                             (begin
                               (pretty-print `(in ,e test can be neither true nor false under assumptions ,@csf))
                               (error "check-constraints"))))))
                (`(case (expr ,key) . ,cases)
                 (let* ((key (check-expr key csf tenv))
                        (key-with-types (add-types key tenv))
                        (key-type (infer-type key tenv)))
                   `(case (expr ,key) . ,(filter-map
                                          (match-lambda
                                            (`((,k) ,body)
                                             (let ((cur-constraints
                                                    `((check (bool equal ,key-with-types (,key-type . ,k)))
                                                      . ,csf)))
                                               (if (constraints-satisfiable? cur-constraints)
                                                   `((,k)
                                                     ,(check-statement body cur-constraints tenv))
                                                   #f))))
                                          cases))))
                (`(expr (abort)) e)
                (`(parallel-assign ,prs)
                 `(parallel-assign
                   ,(map (match-lambda (`(,var . ,val) `(,var . ,(check-expr val csf tenv)))) prs)))
                (_ (error "check-constraints" e)))))
           (check-expr
            (lambda (e csf tenv)
              (match e
                (`(if-e ,p ,a ,b)
                 ;(pretty-print `(checking if-e ,p ,a ,b))
                 (let* ((p (check-expr p csf tenv))
                        (p-type (infer-type p tenv))
                        (p-with-types (add-types p tenv))
                        (p-true-constraints `((check ,p-with-types) . ,csf))
                        (p-false-constraints `((check (bool logical-not ,p-with-types)) . ,csf))
                        (p-true (constraints-satisfiable? p-true-constraints))
                        (p-false (constraints-satisfiable? p-false-constraints)))
                   (if p-true
                       (if p-false
                           `(if-e ,p
                                  ,(check-expr a p-true-constraints tenv)
                                  ,(check-expr b p-false-constraints tenv))
                           (begin
                             (pretty-print `(removing false if-e branch ,b))
                             (check-expr a csf tenv)))
                       (if p-false
                           (begin
                             (pretty-print `(removing true if-e branch ,a))
                             (check-expr b csf tenv))
                           (error "if-e not satisfiable" p)))))
                ((? list?) (cons (car e) (map (lambda (e2) (check-expr e2 csf tenv)) (cdr e))))
                (_ e)))))
    (check-statement e '() '())))

#;(set! data
      (map (lambda (x)
             (let ((result
                    (match (simplify-full x)
                      (`(label ,l ,body)
                       `(label ,l ,(change-to-let* (remove-unused-variables (check-constraints body)))))
                      (e (change-to-let* e)))))
               (pretty-print result)
               '() #;result))
           data))

; (set! data (map (match-lambda (`(label ,l ,body) `(label ,l ,(check-constraints body))) (e e)) data))

; (pretty-print (change-to-let* (map expand-one-simp data)))
; (for-each (lambda (x) (pretty-print (change-to-let* (copy-and-constant-prop x '() always-simple?)))) data)
; (for-each (lambda (x) (pretty-print (change-to-let* (simplify-full x)))) data)
(set! data
      (map (lambda (x)
             (remove-unused-variables
              (copy-and-constant-prop
               (split-and-value-number x)
               '()
               memory-simple?)))
           data))

(define (change-memory-reads s)
  (letrec ((do-expr (lambda (e)
                      (match e
                        (`(memoryReadByte (memoryWriteByte ,m ,addr1 ,d) ,addr2)
                         `(if-e (equal ,(do-expr addr1) ,(do-expr addr2))
                                ,(do-expr d)
                                ,(do-expr `(memoryReadByte ,m ,addr2))))
                        ((? list?) (cons (car e) (map do-expr (cdr e))))
                        (_ e)))))
    (match s
      (`(begin . ,ls) `(begin . ,(map change-memory-reads ls)))
      (`(parallel-assign ,pairs)
       `(parallel-assign ,(map (match-lambda (`(,v . ,val) `(,v . ,(do-expr val)))) pairs)))
      (`(expr ,e) `(expr ,(do-expr e)))
      (`(let ((,t ,v ,val)) ,body)
       `(let ((,t ,v ,(do-expr val))) ,(change-memory-reads body)))
      (`(goto ,l) s)
      (`(if ,p ,a ,b) `(if ,(do-expr p) ,(change-memory-reads a) ,(change-memory-reads b)))
      (`(case ,key . ,cases)
       `(case ,(do-expr key) . ,(map (match-lambda (`(,k ,body) `(,k ,(change-memory-reads body)))) cases)))
      (`(label ,l ,body) `(label ,l ,(change-memory-reads body)))
      (_ (error "change-memory-reads" s)))))

(set! data
      (map change-memory-reads data))

(set! data
      (map (match-lambda (`(label ,l ,body) `(label ,l ,(remove-unused-variables (check-constraints body))))
                         (e e))
           data))

; (pretty-print (map change-to-let* (map simplify-full data)))
;
