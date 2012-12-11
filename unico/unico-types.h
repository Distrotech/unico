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

#ifndef UNICO_TYPES_H
#define UNICO_TYPES_H

#define DRAW_ARGS GtkThemingEngine *engine, \
                  cairo_t          *cr, \
                  gdouble           x, \
                  gdouble           y, \
                  gdouble           width, \
                  gdouble           height

G_BEGIN_DECLS

enum {
  SIDE_LEFT   = 1,
  SIDE_BOTTOM = 1 << 1,
  SIDE_RIGHT  = 1 << 2,
  SIDE_TOP    = 1 << 3,
  SIDE_ALL    = 0xF
};

typedef enum
{
  UNICO_STYLE_DEFAULT = 0,
  UNICO_NUM_STYLES = 1
} UnicoStyles;

typedef struct _UnicoStyleFunctions UnicoStyleFunctions;

struct _UnicoStyleFunctions
{
  void (*draw_arrow) (GtkThemingEngine *engine,
                      cairo_t          *cr,
                      gdouble           angle,
                      gdouble           x,
                      gdouble           y,
                      gdouble           size);

  void (*draw_expander) (DRAW_ARGS);

  void (*draw_focus) (DRAW_ARGS);
};

G_END_DECLS

#endif /* UNICO_TYPES_H */
