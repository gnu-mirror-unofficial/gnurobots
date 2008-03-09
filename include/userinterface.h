/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots game engine. This is the header file for user_interface module
 *
 * GNU Robots is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNU Robots is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Robots; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include <glib-object.h>
#include "map.h"

G_BEGIN_DECLS

extern GType _user_interface_type;

typedef struct _UserInterface UserInterface;
typedef struct _UserInterfaceClass UserInterfaceClass;

struct _UserInterfaceClass {
  GTypeInterface parent;

  void 	(* user_interface_add_thing)		(UserInterface *ui, 
					 	 gint x, 
					 	 gint y, 
					 	 gint thing);

  void 	(* user_interface_draw)	 		(UserInterface *ui);

  void	(* user_interface_update_status)	(UserInterface *ui,
						 const gchar *s, 
		 				 glong energy, 
						 glong score, 
						 glong shields);

  void 	(* user_interface_move_robot) 		(UserInterface *ui, 
						 gint from_x, 
						 gint from_y, 
						 gint to_x, 
						 gint to_y, 
						 gint cdir,
		 				 glong energy, 
						 glong score, 
						 glong shields);

   void (* user_interface_robot_smell) 		(UserInterface *ui, 
						 gint x, 
						 gint y, 
						 gint cdir,
		 				 glong energy, 
						 glong score, 
						 glong shields);

   void	(* user_interface_robot_zap)		(UserInterface *ui, 
						 gint x, 
						 gint y, 
						 gint cdir, 
			       			 gint x_to, 
						 gint y_to,
		 				 glong energy, 
						 glong score, 
						 glong shields);

   void (* user_interface_robot_feel) 		(UserInterface *ui, 
						 gint x, 
						 gint y, 
						 gint cdir, 
						 gint x_to, 
						 gint y_to,
		 				 glong energy, 
						 glong score, 
						 glong shields);

   void (* user_interface_robot_grab) 		(UserInterface *ui,
				         	 gint x, 
				         	 gint y, 
				         	 gint cdir, 
				         	 gint x_to, 
				         	 gint y_to,
				         	 glong energy,
				         	 glong score, 
				         	 glong shields);

   void (* user_interface_robot_look) 		(UserInterface *ui,
						 gint x, 
						 gint y, 
						 gint cdir, 
						 gint x_to, 
						 gint y_to,
		 				 glong energy, 
						 glong score, 
						 glong shields);

/* routines to get/display data from/to user */
   void (* user_interface_get_string) 		(UserInterface *ui,
						 gchar *prompt, 
						 gchar *buff, 
						 gint len);
};

#define G_TYPE_USER_INTERFACE			(_user_interface_type)
#define G_IS_USER_INTERFACE(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_USER_INTERFACE))
#define G_IS_USER_INTERFACE_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_USER_INTERFACE))
#define USER_INTERFACE_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_INTERFACE ((obj), G_TYPE_USER_INTERFACE, UserInterfaceClass))
#define USER_INTERFACE(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_USER_INTERFACE, UserInterface))
#define USER_INTERFACE_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_USER_INTERFACE, UserInterfaceClass))

/* normal GObject stuff */
GType	user_interface_get_type		(void);

/* functions we want implemented by the implementers of our interface */
void 	user_interface_add_thing 	(UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint thing);

void 	user_interface_draw	 	(UserInterface *ui);

void	user_interface_update_status	(UserInterface *ui,
					 const gchar *s, 
		 			 glong energy, 
					 glong score, 
					 glong shields);

void 	user_interface_move_robot 	(UserInterface *ui, 
					 gint from_x, 
					 gint from_y, 
					 gint to_x, 
					 gint to_y, 
					 gint cdir,
		 			 glong energy, 
					 glong score, 
					 glong shields);

void 	user_interface_robot_smell 	(UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir,
		 			 glong energy, 
					 glong score, 
					 glong shields);

void 	user_interface_robot_zap 	(UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir, 
			       		 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields);

void 	user_interface_robot_feel 	(UserInterface *ui, 
					 gint x, 
					 gint y, 
					 gint cdir, 
					 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields);

void 	user_interface_robot_grab 	(UserInterface *ui,
				         gint x, 
				         gint y, 
				         gint cdir, 
				         gint x_to, 
				         gint y_to,
				         glong energy,
				         glong score, 
				         glong shields);

void 	user_interface_robot_look 	(UserInterface *ui,
					 gint x, 
					 gint y, 
					 gint cdir, 
					 gint x_to, 
					 gint y_to,
		 			 glong energy, 
					 glong score, 
					 glong shields);

/* routines to get/display data from/to user */
void 	user_interface_get_string 	(UserInterface *ui,
					 gchar *prompt, 
					 gchar *buff, 
					 gint len);

typedef UserInterface * (* UserInterfaceInitFunc) (Map *map, 
						   GType parent_type);
#define USER_INTERFACE_INIT_FUNCTION "user_interface_new"

G_END_DECLS

#endif /* __USER_INTERFACE_H__*/
