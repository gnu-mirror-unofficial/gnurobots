/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * Robot object for GNU Robots Game.
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

#ifndef __G_ROBOT_H__
#define __G_ROBOT_H__

#include <glib-object.h>
#include <glib.h>
#include "ui-arena.h"
#include "map.h"

G_BEGIN_DECLS

#define G_TYPE_ROBOT            g_robot_get_type()
#define G_IS_ROBOT(obj)         G_TYPE_CHECK_INSTANCE_TYPE ((obj),\
                                  G_TYPE_ROBOT)
#define G_IS_ROBOT_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE ((klass),\
                                  G_TYPE_ROBOT)
#define G_ROBOT_GET_CLASS(obj)  G_TYPE_INSTANCE_GET_CLASS ((obj),\
                                  G_TYPE_ROBOT, GRobotClass)
#define G_ROBOT(obj)            G_TYPE_CHECK_INSTANCE_CAST ((obj),\
                                  G_TYPE_ROBOT, GRobot)
#define G_ROBOT_CLASS(klass)    G_TYPE_CHECK_CLASS_CAST ((klass),\
                                  G_TYPE_ROBOT, GRobotClass)

typedef struct _GRobot GRobot;
typedef struct _GRobotClass GRobotClass;

struct _GRobot {
  GObject   object;

  gint      x;
  gint      y;
  gint      dir;
  glong     score;
  glong     energy;
  glong     shields;
  glong     shots;
  glong     units;

  UIArena *ui;
  Map *map;
};

struct _GRobotClass {
  GObjectClass parent_class;

  void (*death) (GRobot *robot);
};

/* some convenient macros */
#define G_ROBOT_POSITION_X(robot)   ((robot)->x)
#define G_ROBOT_POSITION_Y(robot)   ((robot)->y)
#define sign(x)                     (x/abs(x))

/* normal GObject stuff */
GType g_robot_get_type(void) G_GNUC_CONST;

/* Our object functions */
GRobot* g_robot_new(int x, int y, int dir, long score, long energy,
        long shield, long units, long shots, UIArena *ui, Map *map);

void        g_robot_turn            (GRobot *robot, gint num_turns);
gboolean    g_robot_move            (GRobot *robot, gint steps);
gboolean    g_robot_smell           (GRobot *robot, gchar *str);
gboolean    g_robot_feel            (GRobot *robot, gchar *str);
gboolean    g_robot_look            (GRobot *robot, gchar *str);
gboolean    g_robot_grab            (GRobot *robot);
gboolean    g_robot_zap             (GRobot *robot);
gboolean    g_robot_stop            (GRobot *robot);
glong       g_robot_get_shields     (GRobot *robot);
glong       g_robot_get_energy      (GRobot *robot);
glong       g_robot_get_score       (GRobot *robot);

G_END_DECLS

#endif /* __G_ROBOT_H__ */
