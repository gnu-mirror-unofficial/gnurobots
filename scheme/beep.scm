;;; beep.scm
;;; Sample robot provided by Jim Hall <jhall1@isd.net>
;;; This robot will just turn around 360-degrees, and will beep if it finds
;;; a prize item.  This is similar to a radar.

;;; Define a function that will generate an audible beep

(define (beep) (display "\a"))

;;; Define a function that turns one unit, then feels for a prize.
;;; If we find a prize, make a beep.

(define (turn-and-feel)
  (robot-turn 1)
  (if (robot-feel "prize") (beep))
)

;;; Make one sweep:

(turn-and-feel)
(sleep 1)

(turn-and-feel)
(sleep 1)

(turn-and-feel)
(sleep 1)

(turn-and-feel)
(sleep 1)
