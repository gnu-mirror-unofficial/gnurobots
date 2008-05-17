/* Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
 *
 * GNU Robots Game.
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

#ifndef CONFIGS_H
#define CONFIGS_H

#define TILE_SIZE 16
#undef USE_MITSHM

#ifdef PKGLIBDIR
#define MODULE_PATH 	PKGLIBDIR
#else
#ifdef ABS_TOP_BUILDDIR
#define MODULE_PATH 	ABS_TOP_BUILDDIR "/lib/.libs"
#else
#define MODULE_PATH 	"lib/.libs"
#endif /* ABS_TOP_BUILDDIR */
#endif /* PKGLIBDIR */

#ifndef MODULE_PATH_ENV
#  define MODULE_PATH_ENV   "GNU_ROBOTS_PLUGIN_PATH"
#endif

#ifndef MAPS_PATH_ENV
#  define MAPS_PATH_ENV   "GNU_ROBOTS_MAPS_PATH"
#endif

#ifndef SCRIPTS_PATH_ENV
#  define SCRIPTS_PATH_ENV   "GNU_ROBOTS_SCRIPTS_PATH"
#endif

#define MAX_PATH 256

/* Defaults */
#define DEFAULT_MAP 	"maze.map"
#define DEFAULT_SCRIPT  "mapper.scm"

/* Symbolic constants */
#define SPACE ' '
#define FOOD  '+'
#define PRIZE '$'
#define WALL  '#'
#define BADDIE '@'
#define ROBOT 'R'

/* Directions */
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

/* For all displays */
#define SLEEP_TIME 200 		/* in milliseconds */
#define USLEEP_TIME 200000      /* in microseconds */
#define USLEEP_MULT 16          /* not used yet --jh */

#define DEFAULT_ENERGY 1000
#define DEFAULT_SHIELDS 100

#define DEFAULT_MAP_COLUMNS 40
#define DEFAULT_MAP_ROWS 20

#define PKGINFO PACKAGE_NAME " " VERSION
#define COPYRIGHT "Copyright (C) 1998,1999,2000 Jim Hall <jhall1@isd.net>\nCopyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>"

#endif /* CONFIGS_H */
