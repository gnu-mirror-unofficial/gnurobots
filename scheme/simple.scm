;;; simple.scm
;;; Sample robot provided by Jim Hall <jhall1@isd.net>
;;; This robot will simply hunt down and grab any prizes in its direct
;;; line of sight.  If it runs into an obstacle, it turns right and
;;; continues from there.  When it has turned 360-degrees, it stops.

;;; Define a function to feel for prize (wrapper)
(define (feel-prize)
  (robot-feel "prize"))

;;; Define a function to grab a single prize
(define (grab-prize)
  (robot-grab)
  (robot-move 1))

;;; Define a function to grab all prizes
(define (grab-all-prizes)
  (do () (not (feel-prize)) (grab-prize)))

;;; The program starts here: hunt for all prizes

(grab-all-prizes)
(robot-turn 1)

(grab-all-prizes)
(robot-turn 1)

(grab-all-prizes)
(robot-turn 1)

(grab-all-prizes)
(sleep 1)
