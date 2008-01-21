/* Functions */

#include <glib.h>
#include <grobot.h>

void main_prog (void *closure, gint argc, gchar *argv[]);
void death (GRobot *robot);
void exit_nicely (void);
void usage (const gchar *argv0);
gint is_file (const gchar *filename);
