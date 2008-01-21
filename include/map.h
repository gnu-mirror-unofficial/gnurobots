#ifndef _MAP_H
#define _MAP_H
/* MACROS */

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

extern GType _map_type;

typedef struct 
{
  gint num_rows;
  gint num_cols;
} MapSize;

typedef struct _Map Map;

struct _Map {
  GObject 	object;
  
  /* The actual Map */
  gint 		**_map;
  MapSize	size;

  gint 		errors;
}; 

typedef struct _MapClass MapClass;

struct _MapClass {
  GObjectClass	parent_class;
};

#define G_TYPE_MAP			(_map_type)
#define G_IS_MAP(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), G_TYPE_MAP))
#define G_IS_MAP_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), G_TYPE_MAP))
#define MAP_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), G_TYPE_MAP, MapClass))
#define MAP(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), G_TYPE_MAP, Map))
#define MAP_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), G_TYPE_MAP, MapClass))

/* some convenient macros */
#define MAP_GET_OBJECT(map, x, y)		((map)->_map[(y)][(x)])
#define MAP_SET_OBJECT(map, x, y, thing)	((map)->_map[(y)][(x)] = thing)

/* normal GObject stuff */
GType		map_get_type			(void);

/* Our object functions */
Map*		map_new_from_file 		(const gchar *map, 
						 gint num_rows,
						 gint num_cols);

G_END_DECLS

#endif
