/* Copyright (C) 1998 Jim Hall <jhall1@isd.net>
 * Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots Map object.
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

#include <unistd.h>     /* for getopt */
#include <stdio.h>
#include <string.h>     /* for strdup */

#include <glib.h>

#include "configs.h"
#include "map.h"

G_DEFINE_TYPE(Map, map, G_TYPE_OBJECT)

enum
{
  ARG_0,
  ARG_SIZE
};

static void map_set_property (GObject *object, guint prop_id,
                  const GValue *value, GParamSpec *pspec);

static void map_get_property (GObject *object, guint prop_id,
                  GValue *value, GParamSpec *pspec);

static void
map_class_init (MapClass *klass)
{
  GObjectClass *gobject_class;

  gobject_class = G_OBJECT_CLASS(klass);

  gobject_class->set_property = map_set_property;
  gobject_class->get_property = map_get_property;

  g_object_class_install_property (G_OBJECT_CLASS (klass), ARG_SIZE,
                   g_param_spec_pointer ("size",
                             "Size",
                             "The size of the map",
                             G_PARAM_READWRITE |
                             G_PARAM_CONSTRUCT));
}

static void
map_init (Map *map)
{
  map->_map = NULL;
  map->size.num_rows = -1;
  map->size.num_cols = -1;
}

/* fload_map - loads a map file into memory. */
static void
fload_map (Map *map, FILE *stream)
{
  gint ch;
  gint i, j;

  /* Read the map file */
  i = 0;
  j = 0;

  while ((ch = fgetc (stream)) != EOF)
  {
    /* Add the ch to the map */
    switch (ch)
    {
    case SPACE:
    case WALL:
    case BADDIE:
    case PRIZE:
    case FOOD:
      map->_map[j][i] = ch;
      break;

    case '\n':
      /* ignore the newline */
      break;

    default:
      /* not a valid map char, but we'll add a space anyway */
      map->_map[j][i] = SPACE;
      break;
    }               /* switch */

    /* Check for newline */
    if (ch == '\n')
    {
      /* the line may have ended early.  pad with WALL */
      while (i < map->size.num_cols)
      {
        map->_map[j][i++] = WALL;
      }
    }

    /* if newline */
    /* Check for limits on map */
    if (++i >= map->size.num_cols)
    {
      /* We have filled this line */
      ++j;
      i = 0;

      /* Flush the buffer for this line */
      while (ch != '\n')
      {
        ch = fgetc (stream);
      }
    }
    /* if i */
    if (j >= map->size.num_rows)
    {
      /* the map is filled */
      return;
    }
  }             /* while fgetc */

  /* After the loop, we are done reading the map file.  Make sure this
     is bounded by WALL. */
  if (i > 0)
  {
    ++j;
  }

  for (i = 0; i < map->size.num_cols; i++)
  {
    map->_map[j][i] = WALL;
  }
}

static void
cleanup_map (Map *map)
{
  gint i, j;

  /* make sure there is a wall at top/bottom */
  for (i = 0; i < map->size.num_cols; i++)
  {
    map->_map[0][i] = WALL;
    map->_map[map->size.num_rows - 1][i] = WALL;
  }

  /* make sure there is a wall at left/right */
  for (j = 0; j < map->size.num_rows; j++)
  {
    map->_map[j][0] = WALL;
    map->_map[j][map->size.num_cols - 1] = WALL;
  }
}

static void
map_set_property (GObject *object, guint prop_id,
          const GValue *value, GParamSpec *pspec)
{
  Map *map;
  guint i;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_MAP (object));

  map = MAP (object);

  switch (prop_id)
  {
  case ARG_SIZE:
    /* Free the map if any */
    if (map->_map != NULL)
    {
      for (i = 0; i < map->size.num_rows; i++)
      {
        g_free (map->_map[i]);
      }

      g_free (map->_map);
    }

    /* Get the new size of the map */
    g_memmove (&map->size, g_value_get_pointer (value), sizeof (MapSize));

    /* Allocate the according to the new size */
    map->_map = g_malloc (map->size.num_rows * sizeof (gint **));

    for (i = 0; i < map->size.num_rows; i++)
    {
      map->_map[i] = g_malloc (map->size.num_cols * sizeof (gint));
    }

    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

static void
map_get_property (GObject *object, guint prop_id,
          GValue *value, GParamSpec *pspec)
{
  Map *map;

  /* it's not null if we got it, but it might not be ours */
  g_return_if_fail (G_IS_MAP (object));

  map = MAP (object);

  switch (prop_id)
  {
  case ARG_SIZE:
    g_value_set_pointer (value, g_memdup (&map->size, sizeof (MapSize)));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    break;
  }
}

Map *
map_new_from_file (const gchar *map_file, gint num_rows, gint num_cols)
{
  Map *map;
  FILE *stream;
  MapSize size;

  size.num_rows = num_rows;
  size.num_cols = num_cols;

  stream = fopen (map_file, "r");

  if (stream != NULL)
  {
    /*map = MAP (g_object_new (map_get_type (), "size", &size, NULL));*/
    map = MAP(g_object_new (map_get_type(), "size", &size, NULL));
    fload_map (map, stream);
    fclose (stream);
    cleanup_map (map);

    return map;
  }

  /* else, no file to open */
  else
  {
    g_warning ("Could not open map file: %s\n", map_file);
    return NULL;
  }
}
