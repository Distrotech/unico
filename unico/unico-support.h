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

#ifndef UNICO_SUPPORT_H
#define UNICO_SUPPORT_H

#include <gtk/gtk.h>

#include "unico-engine.h"
#include "unico-types.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL gboolean unico_gdk_rgba_is_default (GdkRGBA *color);

G_GNUC_INTERNAL void unico_get_line_width (GtkThemingEngine *engine,
                                           gdouble          *line_width);

G_GNUC_INTERNAL gboolean unico_gtk_border_is_zero (GtkBorder *border);

G_GNUC_INTERNAL void unico_lookup_functions (UnicoEngine          *engine,
                                             UnicoStyleFunctions **functions);

G_GNUC_INTERNAL void unico_trim_scale_allocation (GtkThemingEngine *engine,
                                                  gdouble          *x,
                                                  gdouble          *y,
                                                  gdouble          *width,
                                                  gdouble          *height);

G_END_DECLS

#endif /* UNICO_SUPPORT_H */
