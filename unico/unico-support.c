/* The Unico Theming Engine for Gtk+.
 * Copyright (C) 2011 Canonical Ltd
 *
 * This  library is free  software; you can  redistribute it and/or
 * modify it  under  the terms  of the  GNU Lesser  General  Public
 * License  as published  by the Free  Software  Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include <gtk/gtk.h>

#include "unico-engine.h"
#include "unico-support.h"
#include "unico-types.h"

gboolean
unico_gdk_rgba_is_default (GdkRGBA *color)
{
  GdkRGBA default_color;

  /* pink is default GdkRGBA color set in gtk/gtkstyleproperty.c */
  gdk_rgba_parse (&default_color, "pink");

  return gdk_rgba_equal (&default_color, color);
}

void
unico_get_line_width (GtkThemingEngine *engine,
                      gdouble          *line_width)
{
  GtkBorder border;
  GtkStateFlags state;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_border (engine, state, &border);

  *line_width = MIN (MIN (border.top, border.bottom),
                     MIN (border.left, border.right));
}

void
unico_get_border_radius (GtkThemingEngine *engine,
                         gint             *radius)
{
  GtkStateFlags state;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get (engine, state,
                          "border-radius", radius,
                          NULL);
}

gboolean
unico_gtk_border_is_zero (GtkBorder *border)
{
  return (border->top == 0) && (border->bottom == 0) && (border->left == 0) && (border->right == 0);
}

void
unico_lookup_functions (UnicoEngine          *engine,
                        UnicoStyleFunctions **functions)
{
  /* only one style is defined now,
   * add here a check for a theming engine css property,
   * for example -unico-style, and assign new styles */
  if (functions)
    *functions = &engine->style_functions[UNICO_STYLE_DEFAULT];
}

void
unico_trim_scale_allocation (GtkThemingEngine *engine,
                             gdouble          *x,
                             gdouble          *y,
                             gdouble          *width,
                             gdouble          *height)
{
  if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
    {
      *y += *height / 2.0 - 2.0;
      *height = 5;
    }
  else
    {
      *x += *width / 2.0 - 2.0;
      *width = 5;
    }
}
