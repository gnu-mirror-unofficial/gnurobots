;
; Mapping robot for GNU Robots 0.77
; 1998-08-22 by Kyle Hasselbacher <kyle@toehold.com>
;

; The central idea here is that the robot keeps a map of where it's been.
;
; BEHAVIOR
; The robot will move in a straight line until it encounters a wall or some
; place that it's already been.  When it gets there, it will head for some
; place it hasn't been.
; Any time the robot is in a place it hasn't been before, it will feel the
; spaces around it as necessary to find out what's there.  If it feels
; something it can grab, it does.
;
; PROBLEMS
; (1) Speed.  If the robot is far away from a "frontier", it takes a long
; time for it to find a path there.
; (2) It sometimes does some unnecessary turning.  The (feel-around)
; function always leaves it facing as it was, but after that it'll want to
; turn anway because there's something in front of it.
; (3) Its exploration isn't particularly systematic.  It never really makes
; zero progress, but it sometimes goes goes over ground multiple times when
; it doesn't need to.
;
; NOTES
; It keeps a list of frequencies of everything it's found so that it always
; feels for the most prevalent map items first.
; The map data structure stretches as the robot expands its area of
; knowledge.
; The robot also keeps track of its location and orientation.

;;;
;;; Variables
;;;
(define freq '(("space" 1) ("wall" 0) ("baddie" 0) ("food" 0) ("prize" 0)))

; Oops.  I redefined a primitive.  Well, I didn't like that function anyway...
(define map '())

; Assumed to start facing east at the origin.
(define facing (list 'east 'south 'west 'north))
(define loc (cons 1 1))

(define map-wide 0)
(define map-tall 0)

; We change this over the life of the robot, to keep it from getting hung up
;(define favorite-direction 'east)
;(define on-frontier #t)

; If predict-death is true, the robot will dump its map to the screen and
; exit when it think it's low on energy.
(define predict-death #t)
(define energy 1000) ; Starting energy

; If this is true, the robot will cut short path searches as soon as it
; finds a place it hasn't been before.  This sometimes makes the robot
; unnecessarily aggressive because it will see a path THROUGH a baddie
; to a frontier before it notices the (cheaper) path around it.
(define loose-goals #f)

;;;
;;; Mapping functions
;;;
(define (init-map x y)
  (set! map-wide x)
  (set! map-tall y)
  (map-rows (+ 2 x) (+ 2 y)))

(define (widen-map n)
  (set! map-wide (+ n map-wide))
  (set! map (widen-map-n map n)))

(define (widen-map-n map n)
  (if (null? map)
      '()
      (cons (append (car map) (make-list n #f)) (widen-map-n (cdr map) n))))

(define (heighten-map n)
  (set! map-tall (+ n map-tall))
  (set! map (append map (map-rows (list-count (car map)) n))))

; Yuck!  This doesn't work:  (make-list y (make-list x #f))
; because every row is a pointer to the same list!
(define (map-rows x n)
  (if (< n 1)
      '()
      (cons (make-list x #f) (map-rows x (- n 1)))))

(define (mark-map! loc thing)
  (list-set! (list-ref map (cdr loc)) (car loc) thing))

;;;
;;; Path finding
;;;

;
; We have a map, so we should be able to find a path from one part of the
; map to another without having to grope around, right?  Here's how:
;
; (1) Make a list of possible paths.  Each path is a list containing the
; "cost" of taking that path (including a heuristic estimate) and the
; points along the path.
; (2) Extend the current least-cost path in every possible direction
; (creating new paths) and generate new cost estimates.
; (3) Eliminate paths to duplicate locations (keeping the least-cost path).
; (4) Eliminate paths with loops.
; (5) Sort the list of paths.
;
; The heuristic will probably be the horizontal difference plus the
; vertical difference plus one.  Note that it costs to turn, so we need to
; keep track of how the robot is facing too.  We'll make it cost 5 to move
; through a baddie since you can do it if you zap 'im.
;

;
; A lot of the code below was stolen wholesale (with comments) directly
; from my second assignment in CS 348 (Intro to AI) at the U of I four
; years ago.  It was written in LISP, so few changes were necessary, but
; variable names aren't always consistent.
;

; PATH DATA STRUCTURE:
; It's a list that looks like this:  (123 'north (1 . 2) (3 . 4))
; The number is the estimated cost of the path from beginning to
; destination (NOT from beginning to the current end of the path)
; The direction is the initial orientation of the robot.  It never
; changes.
; The first pair is the location the path started.  The second is the first
; step of the path, etc.  The last pair in the list is the end of the path
; right now (which may not be at the goal).

; This will return a path including its cost.
(define (find-path dest-loc)
  (find-path-a dest-loc (list (list (guess-cost loc dest-loc)
				    (car facing) loc))))

(define (find-path-a dest-loc path-list)
  (d-list (list "find-path " dest-loc " " path-list "\n"))
  (cond ((null? path-list) path-list)
	((is-goal? dest-loc (car (last-pair (car path-list)))) (car path-list))
	(#t (find-path-a dest-loc
			 (sort-paths
			  (elim-common-dest
			   (reject-loops
			    (append (cdr path-list)
				    (new-paths (car path-list)
					       dest-loc)))))))))

; The last location in the path.
(define (end-path path)
  (list-ref path (- (list-count path) 1)))

; How much it costs to take a particular set of steps.
; It needs to know the final destination so it can guess the cost of the
; rest of the steps to get there.
; It needs to know the initial orientation so that it can detect turns.
(define (steps-cost steps dest face)
;  (d-list (list "steps-cost " steps " " dest " " face "\n"))
  (if (null? (cdr steps))
      (guess-cost (car steps) dest)
	(+ (if (equal? face (tell-direction (car steps)	(cadr steps)))
	       0 1)
	   (cond ((equal? (at-loc (car steps)) "space") 1)
		 ((equal? (at-loc (car steps)) "baddie") 6)
		 (#t 10000))
	 (steps-cost (cdr steps) dest (tell-direction (car steps)
						      (cadr steps))))))

(define (guess-cost start-loc dest-loc)
  (+ (abs (- (car start-loc) (car dest-loc)))
     (abs (- (cdr start-loc) (cdr dest-loc)))
     1))

; Find more paths based on this path.
(define (new-paths path dest-loc)
  (recompute-costs dest-loc
   (path-sanity (list
		 (append path (list (vector-loc 'north (end-path path))))
		 (append path (list (vector-loc 'south (end-path path))))
		 (append path (list (vector-loc 'east (end-path path))))
		 (append path (list (vector-loc 'west (end-path path))))))))

; Throw out paths that try to go through anything other than spaces or
; baddies.
; Throw out paths that go through points outside the map.
(define (path-sanity path-list)
  (if (null? path-list)
      '()
      (if (or (and (not (equal? (at-loc (end-path (car path-list))) "space"))
		   (not (equal? (at-loc (end-path (car path-list))) "baddie")))
	      (out-of-bounds? (end-path (car path-list))))
	  (path-sanity (cdr path-list))
	  (cons (car path-list) (path-sanity (cdr path-list))))))

(define (recompute-costs dest-loc path-list)
;  (d-list (list "recompute-costs " dest-loc " " path-list "\n"))
  (if (null? path-list)
      '()
      (begin
	(set-car! (car path-list) (steps-cost (cddar path-list) dest-loc
					      (cadar path-list)))
	(cons (car path-list) (recompute-costs dest-loc (cdr path-list))))))

;
; Takes a path list and removes those paths which contain loops (double
; occurrances of any one node).  Each path is checked with the looping
; procedure.
;
(define (reject-loops path-list)
;  (d-list (list "reject-loops " path-list "\n"))
  (if (null? path-list)
      path-list
      (if (looping (car path-list))
	  (reject-loops (cdr path-list))
	  (cons (car path-list) (reject-loops (cdr path-list))))))
;
; A path is checked for loops by checking each node for membership in the
; remainder of the path.
;
(define (looping path)
;  (d-list (list "looping " path "\n"))
  (if (null? path)
      #f
      (if (pair? (car path))
	  (or (member (car path) (cdr path))
	      (looping (cdr path)))
	  (looping (cdr path)))))

; This was a LISP primitive.  It might be a Scheme primitive too, but it
; didn't do what I wanted.  (The original code used symbols for nodes, but
; this is using pairs.)
(define (member test-loc loc-list)
  (if (null? loc-list)
      #f
      (or (loc-eq? test-loc (car loc-list))
	  (member test-loc (cdr loc-list)))))

;
; This takes a list of paths and eliminates those which end at the same
; node.  It takes a list of all paths which have the same ending as the
; current path (provided by same-end)--this list can contain only one
; thing--and takes the shortest of those paths to remain in the list.
;
(define (elim-common-dest path-list)
  (if (null? path-list)
      '()
      (cons (shortest-path (same-end (end-path (car path-list))
				     path-list))
	    (elim-common-dest (elim-dest (end-path (car path-list))
					 (cdr path-list))))))

;
; This takes a path list and an ending node, and returns all paths in the
; list which do not end at that node.  It's used to eliminate paths which
; end at the same node.
;
(define (elim-dest dest path-list)
  (if (null? path-list)
      '()
      (if (loc-eq? dest (end-path (car path-list)))
	  (elim-dest dest (cdr path-list))
	  (cons (car path-list)
		(elim-dest dest (cdr path-list))))))

;
; This takes an ending node and a path list and returns all paths in the
; list which DO end at that node.  It's used to FIND paths which end at the
; same node.
;
(define (same-end end path-list)
  (if (null? path-list) path-list
      (if (loc-eq? end (end-path (car path-list)))
	  (cons (car path-list)
	      (same-end end (cdr path-list)))
	  (same-end end (cdr path-list)))))
;
; This is just a proper call to real-sp, which does the real work of
; finding the shortest path in a list of paths.  It returns a path.
;
(define (shortest-path path-list)
  (real-sp '() path-list))
;
; This recursive function finds the shortest path in a list.  The first
; argument is the shortest path found so far, and the second argument is
; the list of paths for comparison.
;
(define (real-sp shortest path-list)
  (if (null? path-list) shortest
      (if (and (number? (caar path-list)) ; Just in case path-list is messed.
	       (or (null? shortest)
		   (< (caar path-list) (car shortest))))
	  (real-sp (car path-list) (cdr path-list))
	  (real-sp shortest (cdr path-list)))))

; Tell whether our current endpoint is the goal.  If loose-goals is true,
; this will include any location we haven't already visited.  This might
; later be expanded to include "good enough" goals for when the real goal
; is completely inaccessible.
(define (is-goal? dest-loc cur-loc)
  (or (loc-eq? dest-loc cur-loc)
      (and loose-goals (not (been-to cur-loc)))))

; Test whether two locations are equal.
(define (loc-eq? a b)
  (and (= (car a) (car b)) (= (cdr a) (cdr b))))

(define (sort-paths path-list)
  (quicksort path-list (lambda (a b)
			 (cond ((< (car a) (car b)) 'less-than)
			       ((= (car a) (car b)) 'equal-to)
			       ((> (car a) (car b)) 'greater-than)))))

(define (execute-path path)
  (d-list (list "execute-path " path "\n"))
  (execute-steps (cdddr path)))

; Go through the steps dictated by a path.  This will make all the turns,
; moves, and zaps necessary to get you where you're going according to the
; plan.
(define (execute-steps step-list)
  (d-list (list "execute-steps " step-list "\n"))
  (if (null? step-list)
      '()
      (begin
	(turn-face (tell-direction loc (car step-list)))
	(if (equal? (at-loc (car step-list)) "baddie")
	    (zap))
	(move 1)
	(execute-steps (cdr step-list)))))

;;;
;;; Action wrappers
;;;

(define (zap)
  (decr-energy 5)
  (if (robot-zap)
      (mark-map! (front-loc) "space")))

; Maybe this should also note the spaciness of intervening map squares,
; but hopefully we won't move into them if they're not spaces.
(define (move n)
  (decr-energy n)
  (if (robot-move n)
      (begin
	(change-loc n)
	(if (< map-wide (car loc))
	    (widen-map (- (car loc) map-wide)))
	(if (< map-tall (cdr loc))
	    (heighten-map (- (cdr loc) map-tall)))
	(feel-around))
      #f))

(define (change-loc n)
  (if (= n 0)
      '()
      (begin
	(set! loc (front-loc))
	(change-loc (- n 1)))))

(define (turn n)
  (decr-energy (abs n))
  (change-face n)
  (robot-turn n))

(define (change-face n)
  (if (= n 0)
      '()
      (begin
	(if (> n 0)
	    (begin
	      (set! facing (append (cdr facing) (list (car facing))))
	      (change-face (- n 1))))
	(if (< n 0)
	    (begin
	      (set! facing (list (list-ref facing 3) (list-ref facing 0)
				 (list-ref facing 1) (list-ref facing 2)))
	      (change-face (+ n 1)))))))

;;;
;;; Sensory functions.
;;;

; This will feel in front of the robot for everything it knows and grab
; things that are worth grabbing.
(define (grope)
  (let ((thing (grope-things freq)))
    (note-freq! freq thing)
    (if (or (equal? thing "food")
	    (equal? thing "prize"))
	(begin
	  (robot-grab)
	  (if (equal? thing "food")
	      (set! energy (+ 10 energy)))
	  (decr-energy 1)
	  "space")
	thing)))

; This does the actual feeling for the individual things in the frequency
; list.
(define (grope-things freq)
  (if (null? freq)
      #f
      (begin (decr-energy 1)
	     (if (robot-feel (caar freq))
		 (caar freq)
		 (grope-things (cdr freq))))))

; This makes sure the robot knows its immediate surroundings.  It's called
; after every movement.  It won't feel spaces it's already felt, and it
; always leaves the robot facing the same direction it started.
(define (feel-around)
  (let ((start-face (car facing)))
    (feel-directions facing)
    (turn-face start-face)))

(define (feel-directions face-list)
  (if (null? face-list)
      '()
      (begin
	(if (not (at-loc (vector-loc (car face-list) loc)))
	    (begin
;	      (set! on-frontier #t)
	      (turn-face (car face-list))
	      (mark-map! (front-loc) (grope))))
	(feel-directions (cdr face-list)))))

; An old version of feel-around.

;(define (feel-around)
;  (if (not (at-loc (front-loc)))
;      (mark-map! (front-loc) (grope)))
;  (if (not (at-loc (right-loc)))
;      (begin
;	(turn 1) ; right from "front"
;	(mark-map! (front-loc) (grope))
;	(if (not (at-loc (back-loc)))
;	    (begin
;	      (turn 2) ; left from "front"
;	      (mark-map! (front-loc) (grope))
;	      (turn 1)) ; front
;	    (turn -1))) ; front
;      (if (not (at-loc (left-loc)))
;	  (begin
;	    (turn -1) ; left from "front"
;	    (mark-map! (front-loc) (grope))
;	    (turn 1))))) ; front

; This changes the frequency list to reflect the last thing we felt.
(define (note-freq! freq thing)
  (if (null? freq)
      '()
      (if (equal? (caar freq) thing)
	  (set-car! freq (list thing (+ 1 (cadar freq))))
	  (note-freq! (cdr freq) thing))))

(define (sort-freq!)
  (set! freq (quicksort freq (lambda (a b)
			       (cond ((> (cadr a) (cadr b)) 'less-than)
				     ((= (cadr a) (cadr b)) 'equal-to)
				     ((< (cadr a) (cadr b)) 'greater-than))))))

; THE SCHEMATICS OF COMPUTATION by Vincent S. Manis and James J. Little
; page 487
(define quicksort
  (lambda (x compare)
    (if (null? x)
	x
	(let*
	    ((pivot (car x))
	     (smaller '()) (equal '()) (larger '())
	     (classify
	      (lambda (item)
		(case (compare item pivot)
		  ((less-than)
		   (set! smaller (cons item smaller)))
		  ((equal-to)
		   (set! equal (cons item equal)))
		  ((greater-than)
		   (set! larger (cons item larger)))))))
	  (for-each classify x)
;	  (format #t "smaller: ~a equal: ~a larger: ~%"
;		  smaller equal larger)
	  (append (quicksort smaller compare)
		  equal (quicksort larger compare))))))

;;;
;;; Orientation-related functions
;;;

; These give the coordinates of spots around the robot
(define (front-loc)
  (relative-loc 'front loc))

(define (back-loc)
  (relative-loc 'back loc))

(define (right-loc)
  (relative-loc 'right loc))

(define (left-loc)
  (relative-loc 'left loc))

; Tell me a direction (right, left, front, back) and a location, and I'll
; tell you the coordinate in the direction from the location.  This uses
; the current orientation of the robot to do its computation.
(define (relative-loc dir loc)
  (case dir
    ((left)  (case (car facing)
	       ((west)  (cons (car loc)       (+ 1 (cdr loc))))
	       ((east)  (cons (car loc)       (- (cdr loc) 1)))
	       ((north) (cons (- (car loc) 1) (cdr loc)))
	       ((south) (cons (+ (car loc) 1) (cdr loc)))))
    ((right) (case (car facing)
	       ((east)  (cons (car loc)       (+ 1 (cdr loc))))
	       ((west)  (cons (car loc)       (- (cdr loc) 1)))
	       ((south) (cons (- (car loc) 1) (cdr loc)))
	       ((north) (cons (+ (car loc) 1) (cdr loc)))))
    ((back)  (case (car facing)
	       ((south) (cons (car loc)       (- (cdr loc) 1)))
	       ((north) (cons (car loc)       (+ 1 (cdr loc))))
	       ((west)  (cons (+ (car loc) 1) (cdr loc)))
	       ((east)  (cons (- (car loc) 1) (cdr loc)))))
    ((front) (case (car facing)
	       ((south) (cons (car loc)       (+ 1 (cdr loc))))
	       ((north) (cons (car loc)       (- (cdr loc) 1)))
	       ((west)  (cons (- (car loc) 1) (cdr loc)))
	       ((east)  (cons (+ (car loc) 1) (cdr loc)))))))

; Tell me a vector (north, south, east, west) and a location, and I'll tell
; you the location in that direction from your location.
(define (vector-loc face loc)
  (case face
    ((east)  (cons (+ (car loc) 1) (cdr loc)))
    ((west)  (cons (- (car loc) 1) (cdr loc)))
    ((north) (cons (car loc)       (- (cdr loc) 1)))
    ((south) (cons (car loc)       (+ (cdr loc) 1)))))

; Turn in a particular direction (north, south, east, west).
(define (turn-face face)
  (case face
    ((east) (case (car facing)
	      ((east) #t)
	      ((west) (turn 2))
	      ((north) (turn 1))
	      ((south) (turn -1))))
    ((west) (case (car facing)
	      ((west) #t)
	      ((east) (turn 2))
	      ((north) (turn -1))
	      ((south) (turn 1))))
    ((north) (case (car facing)
	       ((north) #t)
	       ((south) (turn 2))
	       ((east) (turn -1))
	       ((west) (turn 1))))
    ((south) (case (car facing)
	       ((south) #t)
	       ((north) (turn 2))
	       ((east) (turn 1))
	       ((west) (turn -1))))))

;;;
;;; Unorganized functions.
;;;

(define (decr-energy n)
  (set! energy (- energy n))
  (d-list (list "*** energy " energy " ***\n"))
  (if (and predict-death (< energy 11))
      (dump)))

; I bet there's a primitive to do this, but I can write this faster
; than I can look it up.
(define (list-count tsil)
  (if (null? tsil)
      0
      (+ 1 (list-count (cdr tsil)))))

; Tell whether a coordinate is outside the map.
(define (out-of-bounds? loc)
  (or (> 1 (car loc)) (> 1 (cdr loc))
      (> (car loc) (- (list-count (car map)) 1))
      (> (cdr loc) (- (list-count map) 1))))

; Tell what's at a particular location.  This will say "wall" for
; out-of-bounds locations to the north or west and #f for out-of-bounds
; locations to the south or east (since the map may be stretched in that
; direction, theoretically).
(define (at-loc loc)
  (if (or (> 0 (car loc)) (> 0 (cdr loc)))
      "wall"
      (if (or (> (car loc) (- (list-count (car map)) 1))
	      (> (cdr loc) (- (list-count map) 1)))
	  #f
	  (list-ref (list-ref map (cdr loc)) (car loc)))))

; Tell whether we've visited a particular location.  It checks whether we
; know what's at that location and the locations around it, so you can get
; a true return even if you haven't actually stepped on the spot, but it
; still means you don't need to go there.
(define (been-to loc)
  (and (at-loc loc)
       (at-loc (cons (+ 1 (car loc)) (cdr loc)))
       (at-loc (cons (- (car loc) 1) (cdr loc)))
       (at-loc (cons (car loc) (- (cdr loc) 1)))
       (at-loc (cons (car loc) (+ 1 (cdr loc))))))

; This gives a list of coordinates that surround a particular location at a
; particular "radius."  The coordinates actually form a square.  If the
; radius is 1, you'll get 8 coordinates.  If the radius is 2, you get 16.
; This is used to search for locations of a particular type in a radiating
; fashion from the robot itself.
(define (coord-around-list loc radius)
  (append (h-coord-list (cons (- (car loc) radius) (- (cdr loc) radius))
			(+ 1 (* 2 radius)))
	  (v-coord-list (cons (+ (car loc) radius) (+ 1 (- (cdr loc) radius)))
			(- (* 2 radius) 1))
	  (h-coord-list (cons (- (car loc) radius) (+ (cdr loc) radius))
			(+ 1 (* 2 radius)))
	  (v-coord-list (cons (- (car loc) radius) (+ 1 (- (cdr loc) radius)))
			(- (* 2 radius) 1))))

(define (h-coord-list loc n)
  (if (= n 0)
      '()
      (cons loc (h-coord-list (cons (+ 1 (car loc)) (cdr loc)) (- n 1)))))

(define (v-coord-list loc n)
  (if (= n 0)
      '()
      (cons loc (v-coord-list (cons (car loc) (+ 1 (cdr loc))) (- n 1)))))

; Gimme a function and a list.  I'll give you a list of pairs.
; (function-results . pair)   Of course, the function should take one argument.

;(define (coord-list-apply func loc-list)
;  (if (null? loc-list)
;      '()
;      (cons (cons (func (car loc-list))
;		  (car loc-list))
;	    (coord-list-apply func (cdr loc-list)))))

;(define (find-result x tsil)
;  (if (null? tsil)
;      '()
;      (if (eqv? x (caar tsil))
;	  (cons (cdar tsil)
;		(find-result x (cdr tsil)))
;	  (find-result x (cdr tsil)))))


; What direction is loc2 from loc1 ?
; (If the direction is an exact diagonal, I don't know what you'll get, but
; I don't think that's particularly wrong either.)
(define (tell-direction loc1 loc2)
  (if (> (abs (- (car loc1) (car loc2)))
	 (abs (- (cdr loc1) (cdr loc2))))
      (if (< 0 (- (car loc1) (car loc2)))
	  'west
	  'east)
      (if (< 0 (- (cdr loc1) (cdr loc2)))
	  'north
	  'south)))

;(define (new-favorite-direction)
;;  (d-list (list "--- favorite direction is:  " favorite-direction "\n"))
;  (set! on-frontier #f)
;  (let* ((cur-dir favorite-direction)
;	 (face-dir (car facing))
;	 (new-dir (tell-direction loc (find-frontier 1))))
;    (if (equal? new-dir cur-dir)
;	(if (equal? new-dir face-dir)
;	    (set! favorite-direction (cadr facing))
;	    (set! favorite-direction face-dir))
;	(set! favorite-direction new-dir))))

; Call this with an initial argument of 1.  It will search outward from the
; robot for a location it hasn't yet visited.  It will return a list of
; such locations which are all roughly the same distance from the robot.
; This is used to select a new destination for the robot when it's gotten
; stuck somewhere.
(define (find-frontier n)
  (d-list (list "--- find-frontier " n " ---\n"))
  (if (> n 20) (dump)) ; PROBABLY a problem
  (let ((result (frontier-do n)))
    (d-list (list result "\n"))
    (if (and (not (null? result))
	     (equal? "baddie" (at-loc (car result)))
	     (= n 1))
	(set! result (append (frontier-do 2) result)))
    (if (null? result)
	(find-frontier (+ n 1))
	(car result))))

; Produces a list of possible frontier values (sorted and sanity checked).
(define (frontier-do n)
  (quicksort  ; Spaces are better than baddies.
   (frontier-sanity (coord-around-list loc n))
   (lambda (a b)
     (cond ((and (equal? (at-loc a) "space")
		 (equal? (at-loc b) "baddie")) 'less-than)
	   ((and (equal? (at-loc a) "baddie")
		 (equal? (at-loc b) "space")) 'greater-than)
	   (#t 'equal-to)))))

; This keeps the results of find-frontier in check.  We throw out:
;
; (1) Locations that are off the map.
; (2) Walls.
; (3) Locations we haven't mapped (find-path doesn't know how to get there).
; (4) Locations we've already visited.
;
; That way find-frontier should give us a spot which is exatly next to a
; spot we haven't mapped.
(define (frontier-sanity loc-list)
  (if (null? loc-list)
      '()
      (if (or (out-of-bounds? (car loc-list))
	      (equal? (at-loc (car loc-list)) "wall")
	      (not (at-loc (car loc-list)))
	      (been-to (car loc-list)))
	  (frontier-sanity (cdr loc-list))
	  (cons (car loc-list) (frontier-sanity (cdr loc-list))))))

; This decides how we move.
(define (go)
  (if (and (equal? (at-loc (front-loc)) "space")
	   (not (been-to (front-loc))))
      (move 1)
      (let ((path (find-path (find-frontier 1))))
	(if (null? path)
	    (dump)
	    (execute-path path)))))

;(define (go)
;  (if (and (equal? (at-loc (vector-loc favorite-direction loc)) "space")
;	   (or (not on-frontier)
;	       (not (been-to (vector-loc favorite-direction loc))))
;	   (not (equal? (car facing) favorite-direction)))
;      (turn-face favorite-direction)
;      (if (equal? (at-loc (front-loc)) "space")
;	  (move 1)
;	  (if (and (equal? (at-loc (right-loc)) "space")
;		   (or (not on-frontier) (not (been-to (right-loc)))))
;	      (turn 1)
;	      (if (and (equal? (at-loc (left-loc)) "space")
;		       (or (not on-frontier) (not (been-to (left-loc)))))
;		  (turn -1)
;		  (let ((path (find-path (find-frontier 1))))
;		    (if (null? path)
;			(dump)
;			(execute-path path))))))))


;		    (new-favorite-direction)
;		    (turn-face favorite-direction)))))))

;;;
;;; Debugging functions.
;;;

; Dummies (so the guile interpreter doesn't blow up)
;(define (robot-feel n) (display "robot-feel\n") "space")
;(define (robot-move n) (display "robot-move\n") #t)
;(define (robot-turn n) (display "robot-turn\n") #t)
;(define (robot-grab)   (display "robot-grab\n") #t)

;(define (d-list tsil) (if (null? tsil) '() (cons (display (car tsil)) (d-list (cdr tsil)))))

(define (d-list tsil) tsil)

; Print the map to the screen and exit.
(define (dump)
  (display-map map)
  (quit))

(define (display-map map)
  (if (null? map)
      '()
      (begin
	(display-map-line (car map))
	(display-map (cdr map)))))

(define (space-out n)
  (if (= n 0)
      '()
      (begin
	(display " ")
	(space-out (- n 1)))))

(define (display-map-line tsil)
  (if (null? tsil)
      (space-out (- 80 (list-count (car map))))
;      (display "\n")
      (begin
	(if (equal? "space" (car tsil))
	    (display "."))
	(if (equal? "baddie" (car tsil))
	    (display "@"))
	(if (equal? "wall" (car tsil))
	    (display "#"))
	(if (not (car tsil))
	    (display "x"))
	(if (equal? "food" (car tsil))
	    (display "+"))
	(if (equal? "prize" (car tsil))
	    (display "$"))
	(display-map-line (cdr tsil)))))

;;;
;;; Main program.
;;;

(define (main-loop)
  (go)
  (main-loop))

; INITIALIZATION.
(set! map (init-map 1 1)) ; The map is tiny, but it will grow.
(sort-freq!)              ; Sort the frequency list if it isn't already.
(mark-map! loc "space")   ; I start out on a space, Shirly.
(feel-around)             ; Get your bearings.

(main-loop)               ; GO.
