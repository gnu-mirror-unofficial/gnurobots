/* Copyright (C) 2000 Tim Northover <tim@pnorthover.freeserve.co.uk>
 * using xpms from Tim Whittock
 *
 * Map editor for GNU robots game
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

/* compile with:
   gcc mapedit.c -omapedit -I../lib `gtk-config --cflags --libs` */

/* Usage: mapedit [filename] */

#include <gtk/gtk.h>		/* xwindows functions */
#include <stdio.h>		/* for file functions */
#include <stdlib.h>		/* for atoi */

/* indexes into the blocks array */

#define SPACE 0
#define FOOD 1
#define WALL 2
#define BADDIE 3
#define PRIZE 4

/* this is 1 more than the real size to allow for a border around
   images: */

#define IMGSIZE 17

/* Stores information on blocks for display and storage */

typedef struct
{
  int type;			/* probably unnecessary at the moment, 
				   but potentially useful (pointers) */
  char *name;			/* for display */
  char save_char;		/* character in map files */
  char **xpm_data;		/* again at the moment not really necessary, 
				   but potentially */
  GdkPixmap *pixmap;		/* The image in gtk useable form */
}
BlockType;

typedef struct
{
  int width, height;
  char *data;			/* will contain index into blocks array */
}
GridType;

GridType grid;			/* The internal grid representation */

/* Now define known blocks -- leave xpm_data & pixmap for later init */

BlockType blocks[] = { {SPACE, "Empty Space", '.', NULL, NULL},
{FOOD, "Food", '+', NULL, NULL},
{WALL, "Wall", '#', NULL, NULL},
{BADDIE, "Baddie", '@', NULL, NULL},
{PRIZE, "Prize", '$', NULL, NULL}
};

int nblocks = 5;		/* number of blocks */

GtkEntry *width_text, *height_text;	/* for use by the size setting
					   button -- not ideal as
					   globals but simpler */

int dragging;			/* whether to change "grid" --
				   modified by button_press &
				   button_release */

int current_tool;		/* index into blocks */

GtkWidget *grid_window;		/* main window for displaying map --
                                   used by various functions */

/* initialisation/end funcs */

/* blocks global */
void init_blocks ();

/* initialises global "grid" -- doesn't touch anything X related */
void init_grid (char *filename);

/* used at beginning and in load button func to actually set "grid" */
void load_file (char *filename);

/* used to report failure to load/save file */
void message_box (char *title, char *message);

/* ATM only frees "grid.data" */
void freestuff ();

/* creates a box with pixmap & label -- for toolbox display */
GtkWidget *create_pixmap_with_label (GtkWidget * parent, GdkPixmap * pixmap,
				     char *text);
/* handler function when new tool is clicked */
void change_tool (GtkWidget * widget, gpointer data);


/* Grid update hanlders */

/* (mouse) button pressed in map window -- start draw */
void button_press (GtkFixed * fixed, GdkEventButton * event, gpointer data);

/* mouse moved in map window -- checks "dragging" */
void mouse_move (GtkFixed * fixed, GdkEventMotion * event, gpointer data);

/* stop drawing */
void button_release (GtkFixed * fixed, GdkEventButton * event, gpointer data);

/* sets "grid" and individual pixmap on map window */
void update_grid (GtkFixed * fixed, int x, int y);

/* creates radio button vertical box (for tools) */
GtkWidget *create_toolbox_box (GtkWidget * parent, GtkSignalFunc func);

/* creates box with load, save... */
GtkWidget *create_command_box (char *filename);

/* creates window for both of above widgets */
GtkWidget *create_toolbox_window (char *filename);

/* creates map display window */
GtkWidget *create_grid_window ();

/* clicked routine for Save */
void save_button (GtkWidget * widget, GtkEntry * textbox);

/* clicked routine for Load */
void load_button (GtkWidget * widget, GtkEntry * textbox);

/* clicked routine for Set Size */
void set_size (GtkWidget * widget, gpointer data);

/* destroys current map window and creates a new one from "grid" */
void recreate_grid_window ();

int
main (int argc, char **argv)
{
  char *filename;
  FILE *fp;			/* for checking filename given */

  GtkWidget *toolbox;		/* main window -- currently unused but
				   saved for future -- and it's nice
				   to know I know something about the
				   window I've created */

  /* Global initialisation */

  if (argc > 1)			/* check if filename specified */
    {
      filename = argv[1];
      fp = fopen (filename, "r");
      if (!fp)
	{
	  g_print ("Could not open file '%s'\n", filename);
	  g_print ("Usage: %s [filename]\n", argv[0]);
	  return 1;
	}
      fclose (fp);
    }
  else
    filename = NULL;

  gtk_init (&argc, &argv);	/* must be before any other gtk_functions... */
  current_tool = SPACE;
  init_blocks ();		/* ...like the ones in here */

  init_grid (filename);		/* loads file or creates 16x16 grid
                                   with SPACE */

  toolbox = create_toolbox_window (filename);	/* main window creation */

  gtk_main ();			/* Main message loop -- Go! */

  return 0;			/* Everything went fine (from our
                                   perspective) */
}

/* initialises the rest of the "blocks" array */
void
init_blocks ()
{
  int i;			/* counter */
  GdkColormap *colour;		/* necessary for creating pixmaps 
				   -- otherwise warnings ensue */
#include "xpm/food.xpm"
#include "xpm/space.xpm"
#include "xpm/prize.xpm"
#include "xpm/baddie.xpm"
#include "xpm/wall.xpm"

  /* These could be set globally, but this avoids making the xpm data
     global */

  blocks[0].xpm_data = space_xpm;
  blocks[1].xpm_data = food_xpm;
  blocks[2].xpm_data = wall_xpm;
  blocks[3].xpm_data = baddie_xpm;
  blocks[4].xpm_data = prize_xpm;

  colour = gdk_colormap_get_system ();	/* needed to prevent warnings */

  for (i = 0; i < nblocks; i++)
    {
      blocks[i].pixmap =
	gdk_pixmap_colormap_create_from_xpm_d (NULL, colour, NULL, NULL,
					       blocks[i].xpm_data);
    }
}

/* either loads file or creates blank map -- called at beginning */

void
init_grid (char *filename)
{
  int i;			/* counter */

  if (filename)			/* file specified -- only false first
                                   time if no command line options */
    {
      load_file (filename);
    }

  else
    {
      grid.width = grid.height = 16;

      grid.data = (char *) g_malloc (grid.width * grid.height * sizeof (char));	/* grab memory */

      for (i = 0; i < grid.width * grid.height; i++)	/* initialise grid */
	{
	  grid.data[i] = SPACE;
	}
    }
}

/* callback for radio buttons -- changes tool */

void
change_tool (GtkWidget * widget, gpointer data)
{
  /* can't just pass integer -- need GINT_TO_POINTER and converse */

  current_tool = GPOINTER_TO_INT (data);
}

/* This creates a pixmap and a label from data -- returns hbox with contents */

GtkWidget *
create_pixmap_with_label (GtkWidget * parent, GdkPixmap * pixmap, char *text)
{
  GtkWidget *pixmapwid, *box, *label;	/* All variables -- easier
                                           than doubling up */

  box = gtk_hbox_new (FALSE, 2);

  /* Create them */

  pixmapwid = gtk_pixmap_new (pixmap, NULL);
  gtk_widget_show (pixmapwid);
  label = gtk_label_new (text);
  gtk_widget_show (label);

  /* Put them in the box */

  gtk_box_pack_start (GTK_BOX (box), pixmapwid, FALSE, FALSE, 4);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 4);
  gtk_widget_show (box);

  return box;
}

/* This creates a vbox with each tool in linked to specified function */

GtkWidget *
create_toolbox_box (GtkWidget * parent, GtkSignalFunc func)
{
  int i;			/* counter */
  GtkWidget *box;		/* Box to return */
  GtkWidget *button;		/* radio button for tools */
  GtkWidget *pixmap;		/* Picture to display -- with label */

  box = gtk_vbox_new (FALSE, 2);

  for (i = 0, button = NULL; i < nblocks; i++)
    {
      pixmap = create_pixmap_with_label (parent, blocks[i].pixmap, blocks[i].name);	/* actually a box -- but contains a pixmap */

      /* create button radio group -- use ?: to prevent special case */

      button =
	gtk_radio_button_new (button ?
			      gtk_radio_button_group (GTK_RADIO_BUTTON
						      (button)) : NULL);

      gtk_container_add (GTK_CONTAINER (button), pixmap);

      gtk_signal_connect (GTK_OBJECT (button), "clicked",
			  GTK_SIGNAL_FUNC (func), GINT_TO_POINTER (i));

      gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 3);

      gtk_widget_show (button);
    }

  return box;			/* return a nice package */
}

/* callback for clicking save button -- currently only way so it does
   th work here */

void
save_button (GtkWidget * widget, GtkEntry * textbox)
{
  /* textbox is passed as data in ...signal_connect */

  int i, j;			/* counters -- width & height */
  FILE *fp;
  char *filename;		/* obtained from "textbox" */

  if (!grid_window)		/* Don't save if they've closed the window */
    {
      return;
    }

  /* Open and check file */

  filename = gtk_entry_get_text (GTK_ENTRY (textbox));
  fp = fopen (filename, "w");

  if (!fp)			/* can't proceed */
    {
      message_box ("Error:", "Could not save file");
      return;			/* Can't proceed */
    }

  /* Write file */

  for (i = 0; i < grid.height; i++)
    {
      for (j = 0; j < grid.width; j++)
	fputc (blocks[(int) grid.data[i * grid.width + j]].save_char, fp);	/* put the save_char for correct "blocks" entry */

      fputc ('\n', fp);		/* newline */
    }

  fclose (fp);
}

/* actual function to load file -- don't put more spare \n's at end
   than width -- it won't like it */

void
load_file (char *filename)
{
  FILE *fp;
  int length;			/* length of file */

  int ctr, i, first;		/* counter into data,
				   counter for finding correct block,
				   whether first line */

  char *data, c;		/* buffer and current character */

  /* standard open and check... */

  fp = fopen (filename, "r");

  if (!fp)
    {
      message_box ("Error", "Could not open file");
      return;
    }

  /* find file size  -- and allocation */

  fseek (fp, 0, SEEK_END);
  length = ftell (fp);
  rewind (fp);			/* for reading */

  data = (char *) g_malloc (length * sizeof (char));

  /* will be larger than necessary -- but only by (height) bytes
     usually */

  ctr = 0;			/* index into "data" */

  first = 1;			/* first line */

  while ((c = fgetc (fp)) != EOF)
    {
      /* This is executed more frequently than strictly necessary
         (ideally only on \n as well as normal) -- but this ensures
         things work */

      data[ctr] = WALL;		/* Sensible default (used in xrobots
                                   as default) */

      /* find correct block */

      for (i = 0; i < nblocks; i++)
	if (c == blocks[i].save_char)
	  {
	    data[ctr++] = i;
	    break;
	  }

      if (c == '\n' && first)	/* end of first line -- now know width */
	{
	  first = 0;		/* not first any more */
	  grid.width = ctr;
	}
    }
  fclose (fp);			/* Done here */

  grid.height = length / (grid.width + 1);	/* height * (width + newlines) == file size (roughly) */

  g_free (grid.data);		/* free the old... */

  grid.data = data;		/* ...and replace with new */
}

/* Actual load button clicked -- calls load_file for hard work*/

void
load_button (GtkWidget * widget, GtkEntry * textbox)
{
  char *filename;

  filename = gtk_entry_get_text (textbox);
  load_file (filename);

  recreate_grid_window ();	/* destroys current window and creates new */
}

/* Callback for Setting Size button */

void
set_size (GtkWidget * widget, gpointer data)
{
  int x, y, i;

  x = atoi (gtk_entry_get_text (width_text));
  y = atoi (gtk_entry_get_text (height_text));
  if (x > 0 && y > 0 && (x != grid.width || y != grid.height))
    /* check data in boxes (don't change to same size as now) */
    {
      grid.width = x;
      grid.height = y;

      if (grid.data)
	g_free (grid.data);

      grid.data =
	(char *) g_malloc (grid.width * grid.height * sizeof (char));

      for (i = 0; i < grid.width * grid.height; i++)	/* note that current grid is not saved or scaled */
	grid.data[i] = SPACE;

      recreate_grid_window ();	/* inform gtk of change */
    }
}

/* destroys current map window and creates new */

void
recreate_grid_window ()
{
  if (grid_window)		/* just in case -- person could have
                                   closed the window */
    {
      gtk_widget_destroy (grid_window);
    }
  grid_window = create_grid_window ();
}

/* create box of functions -- very tedious */

GtkWidget *
create_command_box (char *filename)
{
  char str[255];		/* to set text boxes based on
                                   width/height -- I know it's
                                   unlikely that it will be 255 chars
                                   long */

  GtkWidget *vbox, *sizebox;	/* vbox == main box for widgets,
                                   sizebox == box for Entrys & labels
                                   for size */

  GtkWidget *button, *textbox;	/* generic button followed by textbox
                                   for filename */

  GtkWidget *label, *xsize, *ysize;	/* stuff for setting size */

  vbox = gtk_vbox_new (FALSE, 3);	/* create main box */

  /* create filename textentry */

  textbox = gtk_entry_new ();

  gtk_entry_set_text (GTK_ENTRY (textbox),
		      filename ? filename : "newmap.map");

  gtk_widget_show (textbox);

  gtk_box_pack_start (GTK_BOX (vbox), textbox, FALSE, FALSE, 2);

  /* create save button & link with callback */

  button = gtk_button_new_with_label ("Save");

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (save_button), textbox);	/* note it gets the filename as data */

  gtk_widget_show (button);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 2);

  /* create the load button & link */

  button = gtk_button_new_with_label ("Load");

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (load_button), textbox);	/* Also gets callback */

  gtk_widget_show (button);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 2);

  /* create row for size controls -- except actual button */

  sizebox = gtk_hbox_new (FALSE, 1);

  /* X label & text */

  label = gtk_label_new ("X:");
  gtk_widget_show (label);

  gtk_box_pack_start (GTK_BOX (sizebox), label, FALSE, FALSE, 1);

  xsize = gtk_entry_new_with_max_length (3);
  width_text = GTK_ENTRY (xsize);
  sprintf (str, "%d", grid.width);
  gtk_entry_set_text (GTK_ENTRY (xsize), str);
  gtk_widget_set_usize (xsize, 30, 20);
  gtk_widget_show (xsize);

  gtk_box_pack_start (GTK_BOX (sizebox), xsize, FALSE, FALSE, 1);

  /* Y label && text */

  label = gtk_label_new ("Y:");
  gtk_widget_show (label);

  gtk_box_pack_start (GTK_BOX (sizebox), label, FALSE, FALSE, 1);

  ysize = gtk_entry_new_with_max_length (3);
  height_text = GTK_ENTRY (ysize);
  sprintf (str, "%d", grid.height);
  gtk_entry_set_text (GTK_ENTRY (ysize), str);
  gtk_widget_set_usize (ysize, 30, 20);
  gtk_widget_show (ysize);

  gtk_box_pack_start (GTK_BOX (sizebox), ysize, FALSE, FALSE, 1);

  gtk_widget_show (sizebox);
  gtk_box_pack_start (GTK_BOX (vbox), sizebox, FALSE, FALSE, 2);

  /* Done size controls */

  /* Actual size button */

  button = gtk_button_new_with_label ("Set Size");

  gtk_signal_connect (GTK_OBJECT (button), "clicked",
		      GTK_SIGNAL_FUNC (set_size), NULL);

  gtk_widget_show (button);

  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, FALSE, 2);

  return vbox;			/* return nice simple package */
}

/* This creates and initialises the toolbox window (actually the MAIN
   WINDOW)*/

GtkWidget *
create_toolbox_window (char *filename)
{
  GtkWidget *window;		/* main window */

  GtkWidget *toolbox, *command_box, *hbox;	/* three important components (hbox to store others) */

  grid_window = create_grid_window ();	/* map window controlled by
                                           this -- needs to be able to
                                           load */

  /* Create our window -- its closure causes the app to quit... */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Toolbox");

  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
		      GTK_SIGNAL_FUNC (gtk_exit), NULL);	/* ... as setup on this line */

  /* create and display two panes of display */

  toolbox = create_toolbox_box (window, change_tool);
  gtk_widget_show (toolbox);
  command_box = create_command_box (filename);
  gtk_widget_show (command_box);

  /* join two panes together */

  hbox = gtk_hbox_new (FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), toolbox, FALSE, FALSE, 5);
  gtk_box_pack_start (GTK_BOX (hbox), command_box, FALSE, FALSE, 5);
  gtk_widget_show (hbox);
  gtk_container_add (GTK_CONTAINER (window), hbox);

  /* show window */

  gtk_widget_show (window);
  return window;		/* give it back to main */
}

/* receives position to change -- puts correct pixmap in place */

/* See Fixed(Wibble1) in create_grid_window for description of fixed */

void
update_grid (GtkFixed * fixed, int x, int y)
{
  int length, i, dx, dy;	/* length == how many children are in fixed,
				   i == counter for iteration,
				   dx, dy == How far apart the current position is from the desired one */

  GList *children;		/* children of fixed -- i.e. the
                                   pixmap widgets */

  GtkFixedChild *child;		/* individual child of fixed */

  grid.data[x + y * grid.width] = current_tool;

  /* initialise for search */

  children = fixed->children;
  length = g_list_length (children);

  /* start finding -- this is a horrible kludge, but the simplest to
     code */

  for (i = 0; i < length; i++)
    {
      child = (GtkFixedChild *) g_list_nth_data (children, i);	/* get current thing */

      dx = x - (child->x / IMGSIZE);	/* position in pixels -- must be changed to reality */

      dy = y - (child->y / IMGSIZE);

      if (dx == 0 && dy == 0)	/* if correct position */
	{
	  gtk_pixmap_set (GTK_PIXMAP (child->widget),
			  blocks[current_tool].pixmap, NULL);	/* change the pixmap */

	  break;		/* and quit -- don't compound folly */
	}
    }
}

/* handler for drawing -- set dragging true */

void
button_press (GtkFixed * fixed, GdkEventButton * event, gpointer data)
{
  int i, j;			/* positions */

  dragging = 1;

  i = (int) (event->x / IMGSIZE);
  j = (int) (event->y / IMGSIZE);

  update_grid (fixed, i, j);	/* set current square so user doesn't
                                   have to move mouse */
}

/* callback for mouse moved on map window -- checks dragging */

/* This is not ideal as it puts extra load on the cpu when the mouse
   is moving in the map window */

void
mouse_move (GtkFixed * fixed, GdkEventMotion * event, gpointer data)
{
  int i, j;

  i = (int) (event->x / IMGSIZE);
  j = (int) (event->y / IMGSIZE);
  if (dragging)
    update_grid (fixed, i, j);
}

/* stop drawing */

void
button_release (GtkFixed * fixed, GdkEventButton * event, gpointer data)
{
  dragging = 0;
}

/* called when user closes window -- sets grid_window to NULL to
   prevent recreate_grid_window segfaulting */

void
destroy_func (GtkWidget * widget, gpointer data)
{
  *((GtkWidget **) data) = NULL;
  grid_window = NULL;
}

/* Does the donkeywork in creating the map view */

GtkWidget *
create_grid_window ()
{
  int i, j, type;		/* i, j for iteration through "grid.data",
				   type == index into blocks -- stored
				   for convenience */

  GtkWidget *window;		/* actual window */
  GtkWidget *fixed;		/* layout manager for pictures */

  GtkWidget *pixmap;		/* temporary sotrage for each picture
                                   while in use */

  /* first create and set up main features */

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
		      GTK_SIGNAL_FUNC (destroy_func), &window);
  gtk_window_set_title (GTK_WINDOW (window), "Map View");

  /* Fixed(Wibble1):
     fixed is a method of storing widgets by position,
     1. I create a sequence of pixmap widgets and place them in correct positions,
     2. I initialise them to the correct picture,
     3. I link the fixed's press, motion & release events to something to find which
     pixmap is under pointer and change picture & "grid" */

  fixed = gtk_fixed_new ();

  for (i = 0; i < grid.width; i++)
    for (j = 0; j < grid.height; j++)
      {
	type = grid.data[i + j * grid.width];
	pixmap = gtk_pixmap_new (blocks[type].pixmap, NULL);

	gtk_fixed_put (GTK_FIXED (fixed), pixmap, i * IMGSIZE, j * IMGSIZE);	/* position */

	gtk_widget_show (pixmap);
      }

  /* enable events needed... */

  gtk_widget_set_events (fixed,
			 GDK_BUTTON_PRESS_MASK | GDK_POINTER_MOTION_MASK |
			 GDK_BUTTON_RELEASE_MASK);

  /* ... and take control */

  gtk_signal_connect (GTK_OBJECT (fixed), "button_press_event",
		      GTK_SIGNAL_FUNC (button_press), NULL);

  gtk_signal_connect (GTK_OBJECT (fixed), "motion_notify_event",
		      GTK_SIGNAL_FUNC (mouse_move), NULL);

  gtk_signal_connect (GTK_OBJECT (fixed), "button_release_event",
		      GTK_SIGNAL_FUNC (button_release), NULL);

  gtk_widget_show (fixed);

  gtk_container_add (GTK_CONTAINER (window), fixed);	/* window only contains pictures */

  gtk_widget_show (window);

  return window;		/* give the window back to
                                   create_toolbox_window */
}

/* Free all the rubbish */

void
freestuff ()
{
  g_free (grid.data);
}

/* Doesn't look very nice but is functional (message box and code) */

void
message_box (char *title, char *text)
{
  GtkWidget *label, *window, *button, *box;

  window = gtk_window_new (GTK_WINDOW_DIALOG);	/* make window */

  gtk_window_set_title (GTK_WINDOW (window), title);

  label = gtk_label_new (text);	/* Label as required */

  gtk_widget_show (label);

  button = gtk_button_new_with_label ("OK");	/* Just an OK button
                                                   that kills the
                                                   window */

  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
			     GTK_SIGNAL_FUNC (gtk_widget_destroy),
			     (gpointer) window);

  gtk_widget_show (button);

  box = gtk_vbox_new (FALSE, 3);	/* Put everything in... */

  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);
  gtk_box_pack_start (GTK_BOX (box), button, FALSE, FALSE, 3);

  gtk_widget_show (box);

  gtk_container_add (GTK_CONTAINER (window), box);

  gtk_widget_show (window);	/* ... and show */

  /* Don't return anything --  no interaction necessary */
}
