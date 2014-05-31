;spojeni seznamu

(defun spoj (L1 L2) 
  (cond ((NULL L1) L2)
        (T (cons (car L1) (spoj (cdr L1) L2)))  
  )
)

; vytvorte
;obraceni obecneho seznamu obratOb(seznam)
;obratOb
(defun obratOb (L1)
  (cond ((NULL L1) L1)
        ((atom (car L1)) (spoj (obratOb (cdr L1)) (cons (car L1) nil )))
        (t (spoj (obratOb (cdr L1)) (cons (obratOb (car L1)) nil ) ))
  )
)

; test zda je prvek obsazen v obecnem seznamu jePrvekOb(seznam, prvek)
;jePrvekOb
(defun jePrvekOb (L A)
  (cond ((NULL L) nil )
        ((atom L) (= L A))
        (t (or (jePrvekOb (car L) A) (jePrvekOb (cdr L) A))) 
  )
)

; prumer prvku obecneho seznamu prumerPrvkuOb(seznam)
;prumerPrvkuOb
(defun prumerPrvkuOb (L)
  (cond ((NULL L) nil )
        ((> (pocetPrvkuOb L) 0) (/ (soucetPrvkuOb L) (pocetPrvkuOb L)))
        (t nil)
  )
)

; soucet prvku obecneho seznamu soucetPrvkuOb(seznam)
;soucetPrvkuOb
(defun soucetPrvkuOb (L)
  (cond ((NULL L) 0)
        ;((NULL (car L)) (soucetPrvkuOb(cdr L)))
        ((atom L) L)
        ;((atom (car L)) (+ (car L) (soucetPrvkuOb (cdr L))))
        (t (+ (soucetPrvkuOb (car L)) (soucetPrvkuOb (cdr L))))
  )
)


(defun pocetPrvkuOb (L) 
  (cond ((NULL L) 0)
        ((atom L) 1)
        ;((atom (car L)) (+ 1 (pocetPrvkuOb (cdr L))))
        (t (+ (pocetPrvkuOb (car L)) (pocetPrvkuOb (cdr L))))
  )
)

; overeni monotonnosti linearniho seznamu monotonnostLin(seznam)
;monotonnostLin 
(defun monotonnostLin (L)
  (cond ((NULL L) t)
        (t (or (or (rostouci L) (klesajici L) (konstantni L))))
  )
)

(defun rostouci (L)
  (cond ((NULL L) nil)
        ((NULL (cdr L)) t)
        (t (and (< (car L) (car (cdr L))) (rostouci (cdr L))))
  )
)

(defun klesajici (L)
  (cond ((NULL L) nil)
        ((NULL (cdr L)) t)
        (t (and (> (car L) (car (cdr L))) (klesajici (cdr L))))
  )
)

(defun konstantni (L)
  (cond ((NULL L) nil)
        ((NULL (cdr L)) t)
        (t (and (= (car L) (car (cdr L))) (konstantni (cdr L))))
  )
)
