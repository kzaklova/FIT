;faktorial

(defun fact (n)
       (cond ((= n 0) 1)
             ((= n 1) 1)
	     (t (* n (fact (- n 1))))))

;----------------
;fibonacci
; fib (0) = 0
; fib (1) = 1
; fib (n) = fib (n-1) + fib (n-2)
 
(defun fib (n) 
 (cond ((< n 0) nil)
       ((= n 0) 0)
       ((= n 1) 1)
       (t (+ (fib (- n 1)) (fib (- n 2)))))
)
;ackermanova fce
;m >= 0, n >= 0
;ack(0, n) = n+1
;ack(m, 0) = ack(m-1, 1)
;ack(m, n) = ack(m-1, ack(m, n-1))

(defun ack (m n) 
 (cond ((or (< m 0) (< n 0)) nil)
       ((= m 0) (+ n 1))
       ((= n 0) (ack (- m 1) 1))
       (t (ack (- m 1) (ack m (- n 1)))))
)
