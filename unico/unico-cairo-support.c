/* The Unico Theming Engine for Gtk+.
 * Copyright (C) 2011 Canonical Ltd
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed  in the hope that it will be useful,
 * but  WITHOUT ANY WARRANTY; without even  the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License  along  with  this library;  if not,  write to  the Free
 * Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Authored by Andrea Cimitan <andrea.cimitan@canonical.com>
 *
 */

#include <cairo.h>
#include <gtk/gtk.h>

#include "unico-cairo-support.h"
#include "unico-support.h"
#include "unico-types.h"

void
unico_cairo_round_rect (cairo_t         *cr,
                        gdouble          x,
                        gdouble          y,
                        gdouble          width,
                        gdouble          height,
                        gint             radius,
                        guint            sides,
                        GtkJunctionSides junction)
{
  radius = CLAMP (radius, 0, MIN (width / 2, height / 2));

  if (sides & SIDE_RIGHT)
    {
      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_move_to (cr, x + width, y);
      else
        {
          cairo_new_sub_path (cr);

          cairo_arc (cr, x + width - radius, y + radius, radius, - G_PI / 4, 0);
        }

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        cairo_line_to (cr, x + width, y + height);
      else
        cairo_arc (cr, x + width - radius, y + height - radius, radius, 0, G_PI / 4);
    }

  if (sides & SIDE_BOTTOM)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMRIGHT))
        {
          if ((sides & SIDE_RIGHT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + width - radius, y + height - radius, radius, G_PI / 4, G_PI / 2);
        }
      else if ((sides & SIDE_RIGHT) == 0)
        cairo_move_to (cr, x + width, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        cairo_line_to (cr, x, y + height);
      else
        cairo_arc (cr, x + radius, y + height - radius, radius, G_PI / 2, 3 * (G_PI / 4));
    }
  else
    cairo_move_to (cr, x, y + height);

  if (sides & SIDE_LEFT)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_BOTTOMLEFT))
        {
          if ((sides & SIDE_BOTTOM) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + height - radius, radius, 3 * (G_PI / 4), G_PI);
        }
      else if ((sides & SIDE_BOTTOM) == 0)
        cairo_move_to (cr, x, y + height);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        cairo_line_to (cr, x, y);
      else
        cairo_arc (cr, x + radius, y + radius, radius, G_PI, G_PI + G_PI / 4);
    }

  if (sides & SIDE_TOP)
    {
      if (radius != 0 &&
          ! (junction & GTK_JUNCTION_CORNER_TOPLEFT))
        {
          if ((sides & SIDE_LEFT) == 0)
            cairo_new_sub_path (cr);

          cairo_arc (cr, x + radius, y + radius, radius, 5 * (G_PI / 4), 3 * (G_PI / 2));
        }
      else if ((sides & SIDE_LEFT) == 0)
        cairo_move_to (cr, x, y);

      if (radius == 0 ||
          (junction & GTK_JUNCTION_CORNER_TOPRIGHT))
        cairo_line_to (cr, x + width, y);
      else
        cairo_arc (cr, x + width - radius, y + radius, radius, 3 * (G_PI / 2), - G_PI / 4);
    }
}

void
unico_cairo_round_rect_inner (cairo_t         *cr,
                              gdouble          x,
                              gdouble          y,
                              gdouble          width,
                              gdouble          height,
                              gint             radius,
                              guint            sides,
                              GtkJunctionSides junction)
{
  gdouble line_width;

  line_width = cairo_get_line_width (cr);

  /* center the rounded rectangle using line_width */
  unico_cairo_round_rect (cr, x + line_width / 2.0,
                          y + line_width / 2.0,
                          width - line_width,
                          height - line_width,
                          radius, sides, junction);
}
