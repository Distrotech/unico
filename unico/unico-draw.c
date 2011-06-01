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
#include <cairo.h>

#include "unico-cairo-support.h"
#include "unico-draw.h"
#include "unico-types.h"

#define UNICO_RECT_SET(rect, _x, _y, _w, _h) (rect).x      = (_x); \
                                             (rect).y      = (_y); \
                                             (rect).width  = (_w); \
                                             (rect).height = (_h);

static void
unico_draw_cell (DRAW_ARGS,
                 GtkRegionFlags flags)
{
  GtkJunctionSides junction;

  if ((flags & GTK_REGION_FIRST) != 0)
    junction = GTK_JUNCTION_RIGHT;
  else if ((flags & GTK_REGION_LAST) != 0)
    junction = GTK_JUNCTION_LEFT;
  else
    junction = GTK_JUNCTION_TOP | GTK_JUNCTION_BOTTOM;

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, junction);

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, junction);
}

static void
unico_draw_check (DRAW_ARGS)
{
  GtkStateFlags flags;
  gboolean in_cell, in_menu;
  gboolean draw_bullet, inconsistent;

  flags = gtk_theming_engine_get_state (engine);

  in_cell = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_CELL);
  in_menu = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENU);

  inconsistent = (flags & GTK_STATE_FLAG_INCONSISTENT) != 0;
  draw_bullet = (flags & GTK_STATE_FLAG_ACTIVE) != 0;
  draw_bullet |= inconsistent;

  if (!in_menu)
    {
      unico_cairo_draw_background (engine, cr,
                                   x, y, width, height,
                                   0, gtk_theming_engine_get_junction_sides (engine));
      unico_cairo_draw_frame (engine, cr,
                              x, y, width, height,
                              0, gtk_theming_engine_get_junction_sides (engine));
    }

  if (draw_bullet)
    {
      GdkRGBA *bullet_color;

      gtk_theming_engine_get (engine, flags,
                              "-unico-bullet-color", &bullet_color,
                              NULL);

      if (inconsistent)
        {
          cairo_save (cr);

          cairo_set_line_width (cr, 2.0);
          cairo_move_to (cr, 3, (double) height / 2.0);
          cairo_line_to (cr, width - 3, (double) height / 2.0);

          cairo_restore (cr);
        }
      else
        {
          cairo_translate (cr, x, y);

          if (in_menu)
            {
              cairo_scale (cr, (double) width / 18.0, (double) height / 18.0);
              cairo_translate (cr, 2.0, 3.0);
            }
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, flags,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              cairo_scale (cr, (double) width / 18.0, (double) height / 18.0);

              cairo_move_to (cr, 5.0, 5.65);
              cairo_line_to (cr, 8.95, 9.57);
              cairo_line_to (cr, 16.0, 2.54);
              cairo_line_to (cr, 16.0, 8.36);
              cairo_line_to (cr, 10.6, 15.1);
              cairo_line_to (cr, 7.6, 15.1);
              cairo_line_to (cr, 2.95, 10.48);
              cairo_line_to (cr, 2.95, 7.65);
              cairo_close_path (cr);

              gdk_cairo_set_source_rgba (cr, bullet_outline_color);
              cairo_fill (cr);

              cairo_translate (cr, 4.0, 2.0);

              gdk_rgba_free (bullet_outline_color);
            }

          cairo_move_to (cr, 0.0, 6.0);
          cairo_line_to (cr, 0.0, 8.0);
          cairo_line_to (cr, 4.0, 12.0);
          cairo_line_to (cr, 6.0, 12.0);
          cairo_line_to (cr, 15.0, 1.0);
          cairo_line_to (cr, 15.0, 0.0);
          cairo_line_to (cr, 14.0, 0.0);
          cairo_line_to (cr, 5.0, 9.0);
          cairo_line_to (cr, 1.0, 5.0);
          cairo_close_path (cr);
        }

      gdk_cairo_set_source_rgba (cr, bullet_color);
      cairo_fill (cr);

      gdk_rgba_free (bullet_color);
    }
}

static void
unico_draw_column_header_background (DRAW_ARGS,
                                     GtkRegionFlags flags)
{
  GtkJunctionSides junction;

  if ((flags & GTK_REGION_FIRST) != 0)
    junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_CORNER_BOTTOMLEFT;
  else if ((flags & GTK_REGION_LAST) != 0)
    junction = GTK_JUNCTION_LEFT | GTK_JUNCTION_CORNER_BOTTOMRIGHT;
  else
    junction = GTK_JUNCTION_TOP | GTK_JUNCTION_BOTTOM;

  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, junction);
}

static void
unico_draw_column_header_frame (DRAW_ARGS, 
                                GtkRegionFlags flags)
{
  GtkJunctionSides junction;

  if ((flags & GTK_REGION_FIRST) != 0)
    junction = GTK_JUNCTION_RIGHT | GTK_JUNCTION_CORNER_BOTTOMLEFT;
  else if ((flags & GTK_REGION_LAST) != 0)
    junction = GTK_JUNCTION_LEFT | GTK_JUNCTION_CORNER_BOTTOMRIGHT;
  else
    junction = GTK_JUNCTION_TOP | GTK_JUNCTION_BOTTOM;

  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, junction);
}

static void
unico_draw_combo_button_background (DRAW_ARGS)
{
  /* Playground for junctions. */
  unico_cairo_draw_background (engine, cr, x, y, width, height, 0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_combo_button_frame (DRAW_ARGS)
{
  /* Playground for junctions. */
  unico_cairo_draw_frame (engine, cr, x, y, width, height, 0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_common_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_common_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_frame_gap (DRAW_ARGS,
                      GtkPositionType gap_side,
                      gdouble         xy0_gap,
                      gdouble         xy1_gap)
{
  GtkJunctionSides junction;
  GtkStateFlags flags;
  gint border_width, radius;
  gdouble x0, y0, x1, y1, xc, yc, wc, hc;
  gdouble line_width;

  xc = yc = wc = hc = 0;
  flags = gtk_theming_engine_get_state (engine);

  junction = gtk_theming_engine_get_junction_sides (engine);

  unico_get_line_width (engine, &line_width);
  unico_get_border_radius (engine, &radius);
  border_width = 3 * cairo_get_line_width (cr);

  cairo_save (cr);

  switch (gap_side)
  {
    case GTK_POS_TOP:
      xc = x + xy0_gap + border_width;
      yc = y;
      wc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);
      hc = border_width;

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > width - radius)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;

      break;
    case GTK_POS_BOTTOM:
      xc = x + xy0_gap + border_width;
      yc = y + height - border_width;
      wc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);
      hc = border_width;

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      if (xy1_gap > width - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
    case GTK_POS_LEFT:
      xc = x;
      yc = y + xy0_gap + border_width;
      wc = border_width;
      hc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPLEFT;

      if (xy1_gap > height - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMLEFT;

      break;
    case GTK_POS_RIGHT:
      xc = x + width - border_width;
      yc = y + xy0_gap + border_width;
      wc = border_width;
      hc = MAX (xy1_gap - xy0_gap - 2 * border_width, 0);

      if (xy0_gap < radius)
        junction |= GTK_JUNCTION_CORNER_TOPRIGHT;

      if (xy1_gap > height - radius)
        junction |= GTK_JUNCTION_CORNER_BOTTOMRIGHT;

      break;
  }

  cairo_clip_extents (cr, &x0, &y0, &x1, &y1);
  cairo_rectangle (cr, x0, y0, x1 - x0, yc - y0);
  cairo_rectangle (cr, x0, yc, xc - x0, hc);
  cairo_rectangle (cr, xc + wc, yc, x1 - (xc + wc), hc);
  cairo_rectangle (cr, x0, yc + hc, x1 - x0, y1 - (yc + hc));
  cairo_clip (cr);

  cairo_translate (cr, x, y);

  /* FIXME Maybe we need to add a check for the GtkBorderStyle,
   * old GTK_SHADOW_IN corresponds to GTK_BORDER_STYLE_INSET. */

/*  unico_cairo_draw_inner_stroke_rect (engine, cr,*/
/*                                      line_width, line_width,*/
/*                                      width - line_width * 2, height - line_width * 2,*/
/*                                      radius - line_width, 0, junction);*/

  unico_cairo_draw_frame (engine, cr, 0, 0, width, height, 0, junction);

  cairo_restore (cr);
}

static void
unico_draw_icon_view (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_menubaritem_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, GTK_JUNCTION_BOTTOM);
}

static void
unico_draw_menubaritem_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, GTK_JUNCTION_BOTTOM);
}

static void
unico_draw_notebook (DRAW_ARGS,
                     GtkPositionType gap_side,
                     gdouble         xy0_gap,
                     gdouble         xy1_gap)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_draw_frame_gap (engine, cr, x, y, width, height, gap_side, xy0_gap, xy1_gap);
}

static void
unico_draw_progressbar_fill_background (DRAW_ARGS)
{
  /* Playground for effects. */
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_progressbar_fill_frame (DRAW_ARGS)
{
  /* Playground for effects. */
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_radio (DRAW_ARGS)
{
  GtkStateFlags flags;
  gboolean in_cell, in_menu;
  gboolean draw_bullet, inconsistent;

  flags = gtk_theming_engine_get_state (engine);

  in_cell = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_CELL);
  in_menu = gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_MENU);

  inconsistent = (flags & GTK_STATE_FLAG_INCONSISTENT) != 0;
  draw_bullet = (flags & GTK_STATE_FLAG_ACTIVE) != 0;
  draw_bullet |= inconsistent;

  if (!in_menu)
    {
      unico_cairo_draw_background (engine, cr,
                                   x, y, width, height,
                                   0, gtk_theming_engine_get_junction_sides (engine));
      unico_cairo_draw_frame (engine, cr,
                              x, y, width, height,
                              0, gtk_theming_engine_get_junction_sides (engine));
    }

  if (draw_bullet)
    {
      GdkRGBA *bullet_color;

      gtk_theming_engine_get (engine, flags,
                              "-unico-bullet-color", &bullet_color,
                              NULL);

      if (inconsistent)
        {
          cairo_save (cr);

          cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
          cairo_set_line_width (cr, 2.0);

          cairo_move_to(cr, 5, (double) height / 2.0);
          cairo_line_to(cr, width - 5, (double) height / 2.0);

          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_stroke (cr);

          cairo_restore (cr);
        }
      else
        {
          if (in_menu)
            cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                           (double) (width + height) / 4.0 - 4, 0, G_PI * 2);
          else
            {
              GdkRGBA *bullet_outline_color;

              gtk_theming_engine_get (engine, flags,
                                      "-unico-bullet-outline-color", &bullet_outline_color,
                                      NULL);

              cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                             (double) (width + height) / 4.0 - 4, 0, G_PI * 2);
              gdk_cairo_set_source_rgba (cr, bullet_outline_color);
              cairo_fill (cr);

              cairo_arc (cr, x + (double) width / 2.0, y + (double) height / 2.0,
                             (double) (width + height) / 4.0 - 5, 0, G_PI * 2);

              gdk_rgba_free (bullet_outline_color);
            }

          gdk_cairo_set_source_rgba (cr, bullet_color);
          cairo_fill (cr);
        }

      gdk_rgba_free (bullet_color);
    }
}

static void
unico_draw_scrollbar_stepper_background (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_scrollbar_stepper_frame (DRAW_ARGS)
{
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_scrollbar_slider (DRAW_ARGS)
{
  unico_cairo_draw_background (engine, cr,
                               x, y, width, height,
                               0, gtk_theming_engine_get_junction_sides (engine));
  unico_cairo_draw_frame (engine, cr,
                          x, y, width, height,
                          0, gtk_theming_engine_get_junction_sides (engine));
}

static void
unico_draw_separator (DRAW_ARGS)
{
  GtkStateFlags flags;
  gdouble line_width;

  flags = gtk_theming_engine_get_state (engine);
  unico_get_line_width (engine, &line_width);

  /* FIXME right code should be
   * if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
   * but doesn't work for separator tool item. */
  if (width > height)
    {
      cairo_move_to (cr, x, y + height / 2 + line_width / 2);
      cairo_line_to (cr, x + width, y + height / 2 + line_width / 2);
      unico_cairo_draw_inner_stroke_from_path (engine, cr, x, y + height / 2 + line_width / 2, width, line_width);

      cairo_move_to (cr, x, y + height / 2 - line_width / 2);
      cairo_line_to (cr, x + width, y + height / 2 - line_width / 2);
      unico_cairo_draw_border_from_path (engine, cr, x, y + height / 2 - line_width / 2, width, line_width);
    }
  else
    {  
      cairo_move_to (cr, x + width / 2 + line_width / 2, y);
      cairo_line_to (cr, x + width / 2 + line_width / 2, y + height);
      unico_cairo_draw_inner_stroke_from_path (engine, cr, x + width / 2 + line_width / 2, y, line_width, height);

      cairo_move_to (cr, x + width / 2 - line_width / 2, y);
      cairo_line_to (cr, x + width / 2 - line_width / 2, y + height);
      unico_cairo_draw_border_from_path (engine, cr, x + width / 2 - line_width / 2, y, line_width, height);
    }
}

static void
draw_slider_button_path (cairo_t *cr,
                         gint     x,
                         gint     y,
                         gint     width,
                         gint     height,
                         gint     radius)
{
  cairo_move_to (cr, x + radius, y);
  cairo_arc (cr, x + width - radius, y + radius, radius, G_PI * 1.5, G_PI * 2);
  cairo_line_to (cr, x + width, y + height - width / 2.0);
  cairo_line_to (cr, x + width / 2.0, y + height);
  cairo_line_to (cr, x, y + height - width / 2.0);
  cairo_arc (cr, x + radius, y + radius, radius, G_PI, G_PI * 1.5);
}

static void
unico_draw_slider_button (DRAW_ARGS,
                          UnicoSliderParameters *slider)
{
  gdouble line_width, offset;
  gint radius;

  unico_get_line_width (engine, &line_width);
  unico_get_border_radius (engine, &radius);
  radius = MIN (radius, MIN (width / 2.0, height / 2.0));

  offset = 0;
  if (unico_has_outer_stroke (engine))
    offset = line_width;

  cairo_set_line_width (cr, line_width);

  if (!slider->horizontal) /* XXX: could be broken, need to test */
    unico_cairo_exchange_axis (cr, &x, &y, &width, &height);

  cairo_save (cr);

  cairo_translate (cr, x, y);

  /* Is there a way to avoid this translation? */
  cairo_translate (cr, 0.5, 0.5);

  draw_slider_button_path (cr, offset,
                               offset,
                               width - offset * 2,
                               height - offset * 2,
                               radius);
  unico_cairo_draw_background_from_path (engine, cr,
                                         offset,
                                         offset,
                                         width - offset * 2,
                                         height - offset * 2);

  draw_slider_button_path (cr, line_width / 2.0,
                               line_width / 2.0,
                               width - line_width,
                               height - line_width,
                               radius + 1);
  unico_cairo_draw_outer_stroke_from_path (engine, cr,
                                           line_width / 2.0,
                                           line_width / 2.0,
                                           width - line_width,
                                           height - line_width);

  draw_slider_button_path (cr, line_width / 2.0 + offset + line_width,
                               line_width / 2.0 + offset + line_width,
                               width - line_width - offset * 2 - line_width * 2,
                               height - line_width - offset * 2 - line_width * 2,
                               radius - 1);
  unico_cairo_draw_inner_stroke_from_path (engine, cr,
                                           line_width / 2.0 + offset + line_width,
                                           line_width / 2.0 + offset + line_width,
                                           width - line_width - offset * 2 - line_width * 2,
                                           height - line_width - offset * 2 - line_width * 2);

  draw_slider_button_path (cr, line_width / 2.0 + offset,
                               line_width / 2.0 + offset,
                               width - line_width - offset * 2,
                               height - line_width - offset * 2,
                               radius);
  unico_cairo_draw_border_from_path (engine, cr,
                                     line_width / 2.0 + offset,
                                     line_width / 2.0 + offset,
                                     width - line_width - offset * 2,
                                     height - line_width - offset * 2);

  cairo_restore (cr);
}

static void
unico_draw_tab (DRAW_ARGS,
                GtkPositionType gap_side)
{
  GtkStateFlags flags;
  GtkJunctionSides junction = 0;
  gdouble line_width;
  gdouble offset;
  guint hidden_side = 0;

  flags = gtk_theming_engine_get_state (engine);
  unico_get_line_width (engine, &line_width);

  offset = 0;
  if (unico_has_outer_stroke (engine) && (flags & GTK_STATE_FLAG_ACTIVE))
    offset = line_width;

  cairo_save (cr);

  switch (gap_side)
  {
    case GTK_POS_LEFT:
      junction = GTK_JUNCTION_LEFT;
      hidden_side = SIDE_LEFT;

      x -= line_width * 2 + offset;

      cairo_translate (cr, x + width, y);
      cairo_rotate (cr, G_PI / 2);
      break;
    case GTK_POS_RIGHT:
      junction = GTK_JUNCTION_RIGHT;
      hidden_side = SIDE_RIGHT;

      width += line_width * 2 + offset;

      cairo_translate (cr, x, y + height);
      cairo_rotate (cr, - G_PI / 2);
      break;
    case GTK_POS_TOP:
      junction = GTK_JUNCTION_TOP;
      hidden_side = SIDE_TOP;

      y -= line_width * 2 + offset;

      cairo_translate (cr, x + width, y + height);
      cairo_rotate (cr, G_PI);
      break;
    case GTK_POS_BOTTOM:
      junction = GTK_JUNCTION_BOTTOM;
      hidden_side = SIDE_BOTTOM;

      height += line_width * 2 + offset;

      cairo_translate (cr, x, y);
      break;
  }

  if (gap_side == GTK_POS_TOP ||
      gap_side == GTK_POS_BOTTOM)
    unico_cairo_draw_background (engine, cr, 0, 0, width, height, 0, GTK_JUNCTION_BOTTOM);
  else
    unico_cairo_draw_background (engine, cr, 0, 0, height, width, 0, GTK_JUNCTION_BOTTOM);
  cairo_restore (cr);

  cairo_save (cr);

  unico_cairo_draw_frame (engine, cr, x, y, width, height, hidden_side, junction);

  cairo_restore (cr);
}

void
unico_register_style_default (UnicoStyleFunctions *functions)
{
  g_assert (functions);

  functions->draw_cell                          = unico_draw_cell;
  functions->draw_check                         = unico_draw_check;
  functions->draw_column_header_background      = unico_draw_column_header_background;
  functions->draw_column_header_frame           = unico_draw_column_header_frame;
  functions->draw_combo_button_background       = unico_draw_combo_button_background;
  functions->draw_combo_button_frame            = unico_draw_combo_button_frame;
  functions->draw_common_background             = unico_draw_common_background;
  functions->draw_common_frame                  = unico_draw_common_frame;
  functions->draw_frame_gap                     = unico_draw_frame_gap;
  functions->draw_icon_view                     = unico_draw_icon_view;
  functions->draw_menubaritem_background        = unico_draw_menubaritem_background;
  functions->draw_menubaritem_frame             = unico_draw_menubaritem_frame;
  functions->draw_notebook                      = unico_draw_notebook;
  functions->draw_progressbar_fill_background   = unico_draw_progressbar_fill_background;
  functions->draw_progressbar_fill_frame        = unico_draw_progressbar_fill_frame;
  functions->draw_radio                         = unico_draw_radio;
  functions->draw_scrollbar_slider              = unico_draw_scrollbar_slider;
  functions->draw_scrollbar_stepper_background  = unico_draw_scrollbar_stepper_background;
  functions->draw_scrollbar_stepper_frame       = unico_draw_scrollbar_stepper_frame;
  functions->draw_separator                     = unico_draw_separator;
  functions->draw_slider_button                 = unico_draw_slider_button;
  functions->draw_tab                           = unico_draw_tab;
}
