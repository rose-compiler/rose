

;Based on the paper Efficient Evaluation of Pointer Predicates
; with Z3 SMT Solver in SLAM2, Buonimova, Moura, Ball, Levin

; initial translation
; f,v,x: fields, variables, location terms
; x' stands for the result of the translation for the
; term x by applying the translation rules recursively
; f' denotes encoding for the field index f
; B is the maximum number of basic locations in the program
; 1) Basic Location encoding: each variable v is replaced with a
; unique positive integer n, n < B
; 2) Dereference *x is replaced with !S(x',D)
; 3) Direct field access x.f is replaced with !S(x',f'),
; where f' is a non-negative integer n, n < N
; (not sure what big N is, possibly the number of fields of x)
; 4) Indirect field access x->f is replaced with !S(!S(x',D),f')

; Implementation Axioms (I.b in the paper)

; Address relation, true when the first int is Var, 
; the second is the address of Var
(declare-rel A (Int Int))

;S(x,f,v) is v = x->f

(declare-rel !S (Int Int Int))
(declare-rel !A (Int Int))
(declare-rel !V (Int Int))
(declare-rel !D (Int))
(rule (!D -1))
;1 forall X > 0, !A(X) > 0
(rule (=> (and (> X 0) (!A X V)) (> V 0)))

;2 forall X,Y: !A(X) = !A(Y) => X = Y
(rule (=> (and (!A X V) (!A Y V)) (= X Y)))

; 2a, 2b not amenable to rule/query formation so used 2 
; 2a forall X > B !A(X) = !A(!S(X,0))
;(rule (=> (> X B) (and (and (!A X Q) (!S X 0 Z)) (!A Z Q)))) 
; 2b forall X>B Y != (!S(X,0) => !A(X) = !A(Y) => (X = Y))

;3 forall X, F>=0: !S(X,F)>B
(rule (=> (and (!S X F B) (>= F 0)) (> B 0)))
;4 forall X, !A(!S(X,!D)) = !V(X)
(rule (=> ((!S X !D Y) (!A Y Z)) (!V X Z)))
; modified 5 from Axioms II in paper

;5a forall X,F >= 0: !Sm1(!S(X,F)) = X
;5b !Sm2(!S(X,F)) = F
(rule (=> (and (S X F Z) (> F 0)) (Sm1 Z X)))
(rule (=> (and (S X F Z) (> F 0)) (Sm2 Z F)))

;6 forall X,Y,F >= 0: !V(X) = !V(Y) => !V(!S(X,F)) = !V(!S(Y,F))
(rule (=> (and (!V X Z) (!V Y Z)) (and (and (!S X F Z) (!V Z Q)) (and (!S Y F Zp) (!V Zp Q)))))

;7 
;X is an aggregate location
; T the relation data type of a field can be used to determine 
; the address of the next field
; forall F, 0 <= F < N-1, A(S(X,F+1)) = A(S(X,F)) + T(X,F)
; NOTE: in the paper it reads
; forall F, 0 <= F < N-1, A(X,F+1) = A(X,F) + T(X,F) 
; this must be a typo because A only takes one argument

; there are no negative fields
;(<= 0 F)
; there is at least one more field
;(< F (- N 1))
; The fth element of S is Q1
;(S X F Q1)
; The fth plus 1 element of S is Q2
;(S X (+ F 1) Q2)
; the address of Q1 is Z
;(A Q1 Z)
; the address of Q2 is Zprime
;(A Q2 Zprime)
; The length of the fth field (determined by type) is Zt
;(T X F Zt)
; The address of the element in the the next field is 
; the address of the current element plus the length of the 
; fth field determined by type

(rule (=> (and (and (and (and (and (and (<= 0 F) (< F (- N 1))) (!S X F Q1)) (!S X (+ F 1) Q2)) (!A Q1 Z)) (!A Q2 Zprime)) (!T X F Zt)) (= Zprime (+ Z Zt))))

; 8 
; locations as field indices
; translation change
; x[k], k is a variable or a location term
; if k is a variable, k' is an integer that encodes k as a basic location
; if k is a location term, k' is the translation of k by 
; the (modified) translation rules, then x[k] is translated into 
; !S(x',!V(n')), assume integers belong to the interval 0..N-1
; axiom change
; new axiom, forall X > 0: !V(X) != !D
(rule (=> (and (> X 0) (!V X Q)) (not (= Q !D))))



