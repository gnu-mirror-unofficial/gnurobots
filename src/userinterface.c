/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots game engine.  This is the User Interface module
 *
 * GNU Robots is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
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

#include <glib.h>

#include "configs.h"
#include "userinterface.h"

/*enum
{
  ARG_0,
  ARG_MAP
};*/

GType _user_interface_type = 0;

static void user_interface_base_init (UserInterfaceClass * klass);

GType
user_interface_get_type (void)
{
  if (!_user_interface_type)
  {
    static const GTypeInfo interface_info = {
      sizeof (UserInterfaceClass),
      (GBaseInitFunc) user_interface_base_init,
      NULL,
      NULL,
      NULL,
      NULL,
      0,
      0,
      NULL,
      NULL
    };

    _user_interface_type = g_type_register_static (G_TYPE_INTERFACE,
                           "UserInterface",
                           &interface_info, 0);
  }

  return _user_interface_type;
}

static void
user_interface_base_init (UserInterfaceClass * klass)
{
  static gboolean initialized = FALSE;

  if (!initialized)
  {
    /*GObjectClass *gobject_class =
       g_type_class_peek (((GTypeInterface *) klass)->g_instance_type);

       g_object_class_install_property (gobject_class, ARG_MAP, */
    g_object_interface_install_property (klass,
                     g_param_spec_object ("map",
                                  "Map",
                                  "Reference to the Game Map object",
                                  G_TYPE_MAP,
                                  G_PARAM_READWRITE
                                  |
                                  G_PARAM_CONSTRUCT));
    initialized = TRUE;
  }
}

void
user_interface_add_thing (UserInterface * ui, gint x, gint y, gint thing)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_add_thing (ui, x, y, thing);
}

void
user_interface_draw (UserInterface * ui)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_draw (ui);
}

void
user_interface_move_robot (UserInterface * ui,
               gint from_x,
               gint from_y,
               gint to_x,
               gint to_y,
               gint cdir,
               glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_move_robot (ui, from_x,
                                from_y, to_x,
                                to_y, cdir,
                                energy, score,
                                shields);
}

/* user_interfaces to animate the robot */
void
user_interface_robot_smell (UserInterface * ui,
                gint x,
                gint y,
                gint cdir,
                glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_robot_smell (ui, x, y, cdir,
                                 energy, score,
                                 shields);
}

void
user_interface_robot_zap (UserInterface * ui,
              gint x,
              gint y,
              gint cdir,
              gint x_to,
              gint y_to, glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_robot_zap (ui, x, y, cdir,
                               x_to, y_to, energy,
                               score, shields);
}

void
user_interface_robot_feel (UserInterface * ui,
               gint x,
               gint y,
               gint cdir,
               gint x_to,
               gint y_to,
               glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_robot_feel (ui, x, y, cdir,
                                x_to, y_to,
                                energy, score,
                                shields);
}

void
user_interface_robot_grab (UserInterface * ui,
               gint x,
               gint y,
               gint cdir,
               gint x_to,
               gint y_to,
               glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_robot_grab (ui, x, y, cdir,
                                x_to, y_to,
                                energy, score,
                                shields);
}

void
user_interface_robot_look (UserInterface * ui,
               gint x,
               gint y,
               gint cdir,
               gint x_to,
               gint y_to,
               glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_robot_look (ui, x, y, cdir,
                                x_to, y_to,
                                energy, score,
                                shields);
}

/* user_interfaces to get/display data from/to user */
void
user_interface_get_string (UserInterface * ui,
               gchar * prompt, gchar * buff, gint len)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_get_string (ui, prompt, buff,
                                len);
}

void
user_interface_update_status (UserInterface * ui,
                  const gchar * s,
                  glong energy, glong score, glong shields)
{
  USER_INTERFACE_GET_CLASS (ui)->user_interface_update_status (ui, s, energy,
                                   score,
                                   shields);
}
