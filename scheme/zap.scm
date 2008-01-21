;;; zap.scm
;;; Sample robot provided by Jim Hall <jhall1@isd.net>
;;; This is an agressive little robot that will just turn 360-degrees,
;;; and will immediately zap anything that isn't a space.  This builds
;;; on the beep.scm robot program, so it will also beep if it finds a
;;; prize (but then destroys it.)

;;; Define a function to make a beep
(define (beep) (display "\a"))

;;; Define a function to blow away anything that isn't a space
(define (blast-nonspace)
  (if (robot-feel "space") (robot-zap)))

;;; Define a function to turn, then see if a prize is there
(define (turn-and-feel)
  (robot-turn 1)
  (if (robot-feel "prize") (beep)))

;;; The program begins here: make one sweep

(turn-and-feel)
(blast-nonspace)
(sleep 1)

(turn-and-feel)
(blast-nonspace)
(sleep 1)

(turn-and-feel)
(blast-nonspace)
(sleep 1)

(turn-and-feel)
(blast-nonspace)
(sleep 1)
