;;; stop.scm
;;; Sample robot provided by Jim Hall <jhall1@isd.net>
;;; THIS ROBOT IS NOT REALLY INTENDED FOR PUBLIC CONSUMPTION!
;;; Tests my new `stop' and `quit' primitives for GNU Robots

;;; Define a function to make a beep
(define (beep) (display "\a"))

;;; The program starts here:

(beep)
(sleep 1)

;;; Test my new `stop' and `quit' primitives:
;(stop)
(quit)
