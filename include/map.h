/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * This file is part of GNU Robots.
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

#ifndef __MAP_H__
#define __MAP_H__

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define G_TYPE_MAP			  map_get_type()
#define G_IS_MAP(obj)		  G_TYPE_CHECK_INSTANCE_TYPE((obj), \
                                  G_TYPE_MAP)
#define G_IS_MAP_CLASS(klass) G_TYPE_CHECK_CLASS_TYPE((klass), G_TYPE_MAP)
#define MAP_GET_CLASS(obj)	  G_TYPE_INSTANCE_GET_CLASS((obj), \
                                  G_TYPE_MAP, MapClass)
#define MAP(obj)			  G_TYPE_CHECK_INSTANCE_CAST((obj), \
                                  G_TYPE_MAP, Map)
#define MAP_CLASS(klass)	  G_TYPE_CHECK_CLASS_CAST((klass), \
                                  G_TYPE_MAP, MapClass)

typedef struct
{
  gint num_rows;
  gint num_cols;
} MapSize;

typedef struct _Map Map;
typedef struct _MapClass MapClass;

struct _Map {
  GObject 	object;

  /* The actual Map */
  gint 		**_map;
  MapSize	size;

  gint 		errors;
};

struct _MapClass {
  GObjectClass	parent_class;
};

/* some convenient macros */
#define MAP_GET_OBJECT(map, x, y)	    	((map)->_map[(y)][(x)])
#define MAP_SET_OBJECT(map, x, y, thing)	((map)->_map[(y)][(x)] = thing)

/* normal GObject stuff */
GType map_get_type(void);

/* Our object functions */
Map* map_new_from_file(const gchar *map, gint num_rows, gint num_cols);

G_END_DECLS

#endif /* __MAP_H__ */
