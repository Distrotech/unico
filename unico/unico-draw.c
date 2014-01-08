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
#include <math.h>

#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-support.h"
#include "unico-types.h"

static gboolean
draw_arrow_texture (GtkThemingEngine *engine,
                    cairo_t          *cr,
                    gdouble           angle,
                    gdouble           x,
                    gdouble           y,
                    gdouble           size)
{
  GtkStateFlags state;
  GValue value = { 0, };
  cairo_pattern_t *texture = NULL;
  cairo_surface_t *surface = NULL;
  gboolean retval = FALSE;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_property (engine, "-unico-arrow-texture", state, &value);

  if (!G_VALUE_HOLDS_BOXED (&value))
    return FALSE;

  texture = g_value_dup_boxed (&value);
  g_value_unset (&value);

  if (texture != NULL)
    cairo_pattern_get_surface (texture, &surface);

  if (surface != NULL)
    {
      cairo_save (cr);

      cairo_translate (cr, (gint) (x + size / 2), (gint) (y + size / 2));
      cairo_rotate (cr, angle);

      cairo_set_source_surface (cr, surface, - cairo_image_surface_get_width (surface) / 2,
                                             - cairo_image_surface_get_height (surface) / 2);
      cairo_paint (cr);

      cairo_restore (cr);

      retval = TRUE;
    }

  if (texture != NULL)
    cairo_pattern_destroy (texture);

  return retval;
}

static void
unico_draw_arrow (GtkThemingEngine *engine,
                  cairo_t          *cr,
                  gdouble           angle,
                  gdouble           x,
                  gdouble           y,
                  gdouble           size)
{
  GtkStateFlags state;
  GdkRGBA color;
  gdouble size_reduction = 4;

  if (draw_arrow_texture (engine, cr, angle, x, y, size))
    return;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_color (engine, state, &color);

  cairo_save (cr);

  /* use floor function to adjust doubles */
  y = floor (y);
  x = floor (x);
  size = floor (size);

  size -= size_reduction;

  cairo_translate (cr, size_reduction / 2, size_reduction / 2);

  cairo_translate (cr, x + (gint)(size / 2) - 0.5, y + (gint)(size / 2) + 0.5);
  cairo_rotate (cr, angle);

  cairo_move_to (cr, 0, - (gint)(size / 2));
  cairo_line_to (cr, - (gint)(size / 2), (gint)(size / 4));
  cairo_line_to (cr, (gint)(size / 2), (gint)(size / 4));
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_expander (DRAW_ARGS)
{
  GtkStateFlags state;
  GdkRGBA color;
  gint size;
  gdouble angle = G_PI;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_color (engine, state, &color);

  cairo_save (cr);

  /* use floor function to adjust doubles */
  size = floor (MIN (width, height));

  x += (gint) (width / 2) - size / 2;
  y += (gint) (height / 2) - size / 2;

  if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
    angle = G_PI_2;

  cairo_translate (cr, x + (gint)(size / 2) - 0.5, y + (gint)(size / 2) + 0.5);
  cairo_rotate (cr, angle);

  cairo_move_to (cr, 0, - (gint)(size / 2));
  cairo_line_to (cr, - (gint)(size / 2), (gint)(size / 4));
  cairo_line_to (cr, (gint)(size / 2), (gint)(size / 4));
  cairo_close_path (cr);

  cairo_set_source_rgba (cr, color.red, color.green, color.blue, color.alpha * 0.75);
  cairo_fill_preserve (cr);

  gdk_cairo_set_source_rgba (cr, &color);
  cairo_stroke (cr);

  cairo_restore (cr);
}

static void
unico_draw_focus (DRAW_ARGS)
{
  GdkRGBA *fill_color, *border_color;
  GtkStateFlags state;
  gint focus_pad, line_width;
  gint radius;

  state = gtk_theming_engine_get_state (engine);

  gtk_theming_engine_get_style (engine,
                                "focus-padding", &focus_pad,
                                "focus-line-width", &line_width,
                                NULL);

  /* Skip rendering if focus-line-width is 0 or less. */
  if (line_width < 1)
    return;

  /* Use the outline attributes for the focus properties to avoid using
   * custom attributes. Outlines don't have a fill, so hardcode the fill
   * color to be the same color as the border, but with 20% of its alpha.
   */
  gtk_theming_engine_get (engine, state,
                          "outline-color", &border_color,
                          "outline-offset", &radius,
                          NULL);

  fill_color = gdk_rgba_copy (border_color);
  fill_color->alpha = border_color->alpha * 0.2;

  x += focus_pad;
  y += focus_pad;
  width -= focus_pad * 2;
  height -= focus_pad * 2;

  cairo_save (cr);

  cairo_set_line_width (cr, line_width);

  /* first layer, background */
  unico_cairo_round_rect (cr, x, y,
                              width, height,
                              radius, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, fill_color);
  cairo_fill (cr);

  /* second layer, border */
  unico_cairo_round_rect_inner (cr, x, y,
                                    width, height,
                                    radius, SIDE_ALL, GTK_JUNCTION_NONE);
  gdk_cairo_set_source_rgba (cr, border_color);
  cairo_stroke (cr);

  cairo_restore (cr);

  gdk_rgba_free (border_color);
  gdk_rgba_free (fill_color);
}

void
unico_register_style_default (UnicoStyleFunctions *functions)
{
  g_assert (functions);

  functions->draw_arrow    = unico_draw_arrow;
  functions->draw_expander = unico_draw_expander;
  functions->draw_focus    = unico_draw_focus;
}
