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
#include "unico-types.h"

void
unico_lookup_functions (UnicoEngine          *engine,
                        UnicoStyleFunctions **functions)
{
  if (functions)
    *functions = &engine->style_functions[UNICO_STYLE_DEFAULT];
}

void
unico_get_line_width (GtkThemingEngine *engine,
                      gdouble          *line_width)
{
  GtkBorder *border;
  GtkStateFlags flags;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "border-width", &border,
                          NULL);

  *line_width = MIN (MIN (border->top, border->bottom),
                     MIN (border->left, border->right));

  gtk_border_free (border);
}

void
unico_get_border_radius (GtkThemingEngine *engine,
                         gint             *radius)
{
  GtkStateFlags flags;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "border-radius", radius,
                          NULL);
}

gboolean
unico_has_inner_stroke (GtkThemingEngine *engine)
{
  GtkStateFlags flags;
  UnicoStrokeStyle inner_stroke_style;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "-unico-inner-stroke-style", &inner_stroke_style,
                          NULL);

  return inner_stroke_style != UNICO_STROKE_STYLE_NONE;
}

gboolean
unico_has_outer_stroke (GtkThemingEngine *engine)
{
  GtkStateFlags flags;
  UnicoStrokeStyle outer_stroke_style;

  flags = gtk_theming_engine_get_state (engine);
  gtk_theming_engine_get (engine, flags,
                          "-unico-outer-stroke-style", &outer_stroke_style,
                          NULL);

  return outer_stroke_style != UNICO_STROKE_STYLE_NONE;
}
