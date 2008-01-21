#ifndef _G_ROBOT_H
#define _G_ROBOT_H
/* MACROS */

#include <glib-object.h>
#include <glib.h>
#include "userinterface.h"
#include "map.h"

G_BEGIN_DECLS

extern GType _g_robot_type;

typedef struct _GRobot GRobot;

struct _GRobot {
  GObject 	object;
  
  gint 		x;
  gint 		y;
  gint 		dir;
  glong 	score;
  glong 	energy;
  glong 	shields;
  glong 	shots;
  glong 	units;

  UserInterface *ui;
  Map *map;
}; 

typedef struct _GRobotClass GRobotClass;

struct _GRobotClass {
  GObjectClass	parent_class;

  void		(*death)	(GRobot *robot);
};

#define G_TYPE_ROBOT			(_g_robot_type)
#define G_IS_ROBOT(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_ROBOT))
#define G_IS_ROBOT_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_ROBOT))
#define G_ROBOT_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_ROBOT, GRobotClass))
#define G_ROBOT(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_ROBOT, GRobot))
#define G_ROBOT_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_ROBOT, GRobotClass))

/* some convenient macros */
#define G_ROBOT_POSITION_X(robot)	((robot)->x)
#define G_ROBOT_POSITION_Y(robot)	((robot)->y)

/* normal GObject stuff */
GType		g_robot_get_type			(void);

/* Our object functions */
GRobot*		g_robot_new 			(int x, 
						 int y, 
						 int dir, 
						 long score, 
						 long energy,
						 long shield, 
						 long units, 
						 long shots, 
						 UserInterface *ui, 
						 Map *map);

void		g_robot_turn 			(GRobot *robot, gint num_turns);
gboolean	g_robot_move 			(GRobot *robot, gint steps);
gboolean	g_robot_smell 			(GRobot *robot, gchar *str);
gboolean	g_robot_feel 			(GRobot *robot, gchar *str);
gboolean	g_robot_look 			(GRobot *robot, gchar *str);
gboolean 	g_robot_grab 			(GRobot *robot);
gboolean	g_robot_zap 			(GRobot *robot);
gboolean 	g_robot_stop 			(GRobot *robot);
glong		g_robot_get_shields 		(GRobot *robot);
glong		g_robot_get_energy 		(GRobot *robot);
glong		g_robot_get_score 		(GRobot *robot);

G_END_DECLS

#endif
