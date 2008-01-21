/* TODO: Either this needs to go, or it needs a copyright header */

#include <stdio.h>
#include <glib.h>

/* is there a standard C function to do this? */

gint
sign (gint n)
{
  if (n < 0) {
    return (-1);
  } else if (n > 0) {
    return (+1);
  }

  return (0);
}
