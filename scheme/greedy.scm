;;;
;;; Greedy robot for GNU Robots 0.77
;;; 1998-08-15 by Kyle Hasselbacher <kyle@toehold.com>
;;;

;;; Greedy Robot wanders around looking for food and valuable prizes.
;;; The definitions for thing-one and thing-two determine which it thinks
;;; is more important (thing-one is).
;;; It's not always very efficient in its gathering and can easily walk in
;;; a circle if it can't see anything worth grabbing.  It also assumes that
;;; there is nothing interesting behind it, and that's not always true.
;;; It treats baddies and walls the same way in that they are not
;;; food, prizes, or open space.  It just avoids running into them.

(define thing-one "food")
(define thing-two "prize")

;;; If something interesting has been spotted, in-sights holds the name
;;; of the thing we're headed for.  Once it's grabbed, we set! in-sights
;;; back to #f.  Using the variable saves us a little energy since we don't
;;; have to keep looking at the same thing over and over.
(define in-sights #f)

;;; grab a single prize (and move onto the space that had it)
(define (grab)
  (if (robot-grab)
      (robot-move 1))
  (set! in-sights #f))

;;; Look around!
(define (look-left-right thing)
  (if (not (equal? in-sights thing))
      (robot-turn 1))
  (if (and (not (equal? in-sights thing)) (robot-look thing))
      (set! in-sights thing))
  (if (not (equal? in-sights thing))
      (robot-turn 2))
  (if (and (not (equal? in-sights thing)) (robot-look thing))
      (set! in-sights thing)))

(define (seek-thing thing)
  (if (and (not (equal? in-sights thing)) (robot-look thing))
      (set! in-sights thing))
  (look-left-right thing)
  (if (not (equal? in-sights thing))
      (robot-turn 1))
  (equal? in-sights thing))

;;; Book it for a priority-one item.  We don't look around for anything
;;; else, but we do sniff the air for items of the same type we might pass
;;; on the way.
(define (get-thing thing)
  (if (robot-feel thing)
      (grab)
      (begin (smell-test thing)
	     (if (robot-move 1)
		 (get-thing thing)))))

;;; This doesn't do any smelling.  It's called by smell-test when it thinks
;;; there might be something to the left (it's already checked to the right).
(define (smell-behind thing)
  (robot-turn 2)
  (if (robot-feel thing)
      (robot-grab))
  (robot-turn 1))

;;; This feels to the right and left if it smells the thing its asked about.
;;; Do the smell test after (robot-feel thing) fails.
;;; Otherwise you spin around from smelling what's in front of you.
;;; smell-test does a lot of turning but leaves you facing the same way.
(define (smell-test thing)
  (if (robot-smell thing)
      (begin
	(robot-turn 1)
	(if (robot-feel thing)
	    (begin
	      (robot-grab)
	      (if (robot-smell thing)
		  (smell-behind thing)
		  (robot-turn -1)))
	    (smell-behind thing)))))

;;; The drunkard's walk isn't too drunk.
;;; Without obstructions, it only turns once in a while (1/10 steps).
(define (drunkard-walk)
  (if (and (not (= (random 10) 0)) (robot-feel "space"))
      (robot-move 1)
      (begin (robot-turn (+ 1 (random 2))) (drunkard-walk))))

;;; The main loop!  We go for our thing-one if we can see it.  If we can see
;;; thing-two, we go for that while checking for thing-one.  If we can see
;;; neither, we drunkards-walk one step.
(define (main-loop)
  (cond ((seek-thing thing-one)
	 (get-thing thing-one)
	 (set! in-sights #f))

	((seek-thing thing-two) 
	 (if (robot-feel thing-two)
	     (grab)
	     (begin
	       (smell-test thing-two)
	       (robot-move 1))))

	(#t (drunkard-walk)))
  (main-loop))

;;; Be random
;(randomize)

;;; Go to it.
(main-loop)
