/* Robot API for the GNU Robots game */

/* Copyright (C) 1998 Jim Hall, jhall1@isd.net */

/*
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _API_H
#define _API_H

#include <glib.h>
#include <guile/gh.h>           /* GNU Guile high */

void		api_init			(void);

/* Functions */
SCM		api_robot_turn 			(SCM s_n);
SCM 		api_robot_move 			(SCM s_n);
SCM 		api_robot_smell 		(SCM s_th);
SCM 		api_robot_feel 			(SCM s_th);
SCM 		api_robot_look 			(SCM s_th);
SCM 		api_robot_grab 			(void);
SCM 		api_robot_zap 			(void);
SCM 		api_robot_stop 			(void);

SCM 		api_robot_get_shields 		(void);
SCM 		api_robot_get_energy 		(void);
SCM 		api_robot_get_score 		(void);

#endif /* _API_H */
