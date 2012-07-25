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

#include <cairo.h>
#include <cairo-gobject.h>
#include <gtk/gtk.h>

#include "unico.h"
#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-engine.h"
#include "unico-support.h"
#include "unico-types.h"

#define UNICO_NAMESPACE "unico"

#define UNICO_CAIRO_INIT \
        cairo_set_line_width (cr, 1.0); \
        cairo_set_line_cap (cr, CAIRO_LINE_CAP_SQUARE); \
        cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER);

G_DEFINE_DYNAMIC_TYPE (UnicoEngine, unico_engine, GTK_TYPE_THEMING_ENGINE)

static void
unico_engine_render_arrow (GtkThemingEngine *engine,
                           cairo_t          *cr,
                           gdouble           angle,
                           gdouble           x,
                           gdouble           y,
                           gdouble           size)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_arrow (engine, cr, angle, x, y, size);
}

static void
unico_engine_render_expander (GtkThemingEngine *engine,
                              cairo_t          *cr,
                              gdouble           x,
                              gdouble           y,
                              gdouble           width,
                              gdouble           height)
{
  UnicoStyleFunctions *style_functions;

  UNICO_CAIRO_INIT

  unico_lookup_functions (UNICO_ENGINE (engine), &style_functions);

  style_functions->draw_expander (engine, cr, x, y, width, height);
}

void
unico_engine_register_types (GTypeModule *module)
{
  unico_engine_register_type (module);
}

static void
unico_engine_init (UnicoEngine *engine)
{
  unico_register_style_default (&engine->style_functions[UNICO_STYLE_DEFAULT]);
}

static void
unico_engine_class_init (UnicoEngineClass *klass)
{
  GtkThemingEngineClass *engine_class = GTK_THEMING_ENGINE_CLASS (klass);

  engine_class->render_arrow       = unico_engine_render_arrow;
  engine_class->render_expander    = unico_engine_render_expander;

  gtk_theming_engine_register_property (UNICO_NAMESPACE, NULL,
                                        g_param_spec_boxed ("arrow-texture",
                                                            "Arrow texture",
                                                            "Arrow texture",
                                                            CAIRO_GOBJECT_TYPE_PATTERN, 0));
}

static void
unico_engine_class_finalize (UnicoEngineClass *klass)
{
}
