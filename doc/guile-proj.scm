;;;How To Submit A Project
;;;
;;;The first thing to do is look at the Guile Project Mail Submission
;;;doc. This will tell you all you need to know about describing your
;;;project. Then, send a mail to me (Greg.Harvey@thezone.net) with the
;;;subject "Guile Project Submission". Optionally, you can send it to
;;;(Greg.Harvey+guile-project@thezone.net).  Please do one or the
;;;other, or else there's a good chance it'll end up in the spam
;;;box. The body should contain your submission(s).
;;;
;;;How To Update A Project
;;;
;;;It's the exact same thing as making the initial submission, but you
;;;don't have to include every field, just whatever you want
;;;updated. So, say if you want to add a license field to your
;;;existing entry, you'd just have ((name "foo") (license "bar")) as a
;;;submission.

((name "robots")

 (category "Games")
 (keywords "Game " "robots " "diversion")
 
 (description "A game/diversion where you construct a robot (using Scheme) "
              "then set him loose and watch him explore a world on his own."
	      "The GNU Robot program is written in Scheme, and implemented "
	      "using GNU Guile.")

 (location (url "http://www.gnu.org/software/robots/" "GNU Robots homepage"))

 (authors "Jim Hall")
 (maintainer (email "Jim Hall" "jhall1@isd.net"))

 (status "GNU Robots has finally been released as version 1.0!!")

 (help-wanted "GNU Robots could really use a port to GTK+, for GNOME. "
	      "Also, it would be great if someone wrote a GNU Robots  "
	      "code generator, that generated a robot Scheme program  "
	      "based on the user contructing a robot program using    "
	      "little icons that are dropped into place and ordered   "
	      "using special connector wires.  This would be a good   "
	      "senior project for a computer science student!")

 (license "GPL"))
