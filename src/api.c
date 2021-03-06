/* Copyright (C) 1998 Jim Hall <jhall@freedos.org>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * Robot API for the GNU Robots game
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
 * along with GNU Robots.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>             /* for abs, free */

#include <glib.h>
#include <libguile.h>           /* GNU Guile high */

#include "api.h"                /* GNU Robots API */
#include "main.h"               /* for exit_nicely */
#include "grobot.h"

extern GRobot *robot;

/* Functions */

SCM
api_robot_turn (SCM s_n)
{
  g_robot_turn (robot, scm_to_int (s_n));

  return SCM_BOOL (TRUE);
}

SCM
api_robot_move (SCM s_n)
{
  return SCM_BOOL (g_robot_move (robot, scm_to_int (s_n)));
}

SCM
api_robot_smell (SCM s_th)
{
  gboolean ret;
  gchar *str;

  if (scm_symbol_p (s_th) == SCM_BOOL_T) {
    s_th = scm_symbol_to_string (s_th);
  }
  str = scm_to_locale_string (s_th);
  ret = g_robot_smell (robot, str);

  return SCM_BOOL (ret);
}

SCM
api_robot_feel (SCM s_th)
{
  gboolean ret;
  gchar *str;

  if (scm_symbol_p (s_th) == SCM_BOOL_T) {
    s_th = scm_symbol_to_string (s_th);
  }
  str = scm_to_locale_string (s_th);
  ret = g_robot_feel (robot, str);

  return SCM_BOOL (ret);
}

SCM
api_robot_look (SCM s_th)
{
  gboolean ret;
  gchar *str;

  if (scm_symbol_p (s_th) == SCM_BOOL_T) {
    s_th = scm_symbol_to_string (s_th);
  }
  str = scm_to_locale_string (s_th);
  ret = g_robot_look (robot, str);

  return SCM_BOOL (ret);
}

SCM
api_robot_grab (void)
{
  return SCM_BOOL (g_robot_grab (robot));
}

SCM
api_robot_zap (void)
{
  return SCM_BOOL (g_robot_zap (robot));
}

SCM
api_robot_stop (void)
{
  return SCM_BOOL (g_robot_stop (robot));
}

SCM
api_robot_get_shields (void)
{
  glong shields;

  g_object_get (robot, "shields", &shields, NULL);

  /* Returns the robot shields */
  return (scm_from_long (shields));
}

SCM
api_robot_get_energy (void)
{
  glong energy;

  g_object_get (robot, "energy", &energy, NULL);

  /* Returns the robot energy */
  return (scm_from_long (energy));
}

SCM
api_robot_get_score (void)
{
  glong score;

  g_object_get (robot, "score", &score, NULL);

  /* Returns the robot score */
  return (scm_from_long (score));
}

void
api_init (void)
{
  /* define some new builtins (hooks) so that they are available in
     Scheme. */

  /* Guile has this function set to expect that function-pointers are
     passed as (void *), which generates warnings because it's actually
     illegal in C99 (supposedly because there are architectures where
     `function pointers' and `object pointers' actually are incompatible);
     So... jump through hoops to out-guile the system....
  */
  typedef SCM (*c99_fakeout_guile)(const char *, int, int, int, SCM(*)());
  c99_fakeout_guile define_gsubr = (c99_fakeout_guile) scm_c_define_gsubr;
  define_gsubr ("robot-turn", 1, 0, 0, api_robot_turn);
  define_gsubr ("robot-move", 1, 0, 0, api_robot_move);
  define_gsubr ("robot-smell", 1, 0, 0, api_robot_smell);
  define_gsubr ("robot-feel", 1, 0, 0, api_robot_feel);
  define_gsubr ("robot-look", 1, 0, 0, api_robot_look);
  define_gsubr ("robot-grab", 0, 0, 0, api_robot_grab);
  define_gsubr ("robot-zap", 0, 0, 0, api_robot_zap);

  define_gsubr ("robot-get-shields", 0, 0, 0, api_robot_get_shields);
  define_gsubr ("robot-get-energy", 0, 0, 0, api_robot_get_energy);
  define_gsubr ("robot-get-score", 0, 0, 0, api_robot_get_score);

  define_gsubr ("stop", 0, 0, 0, api_robot_stop);
  define_gsubr ("quit", 0, 0, 0, api_robot_stop);
}
