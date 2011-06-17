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

#ifndef UNICO_TYPES_H
#define UNICO_TYPES_H

#define DRAW_ARGS GtkThemingEngine *engine, \
                  cairo_t *cr, \
                  gint x, \
                  gint y, \
                  gint width, \
                  gint height

G_BEGIN_DECLS

enum {
  SIDE_LEFT   = 1,
  SIDE_BOTTOM = 1 << 1,
  SIDE_RIGHT  = 1 << 2,
  SIDE_TOP    = 1 << 3,
  SIDE_ALL    = 0xF
};

typedef struct
{
  gboolean lower;
  gboolean horizontal;
  gboolean fill_level;
} UnicoSliderParameters;

typedef enum
{
  UNICO_STROKE_STYLE_CUSTOM = 0,
  UNICO_STROKE_STYLE_INSET = 1,
  UNICO_STROKE_STYLE_NONE = 2
} UnicoStrokeStyle;

typedef enum
{
  UNICO_STYLE_DEFAULT = 0,
  UNICO_NUM_STYLES = 1
} UnicoStyles;

typedef struct _UnicoStyleFunctions UnicoStyleFunctions;

struct _UnicoStyleFunctions
{
  void (*draw_cell) (DRAW_ARGS,
                     GtkRegionFlags flags);

  void (*draw_check) (DRAW_ARGS);

  void (*draw_column_header_background) (DRAW_ARGS,
                                         GtkRegionFlags flags);

  void (*draw_column_header_frame) (DRAW_ARGS,
                                    GtkRegionFlags flags);

  void (*draw_combo_button_background) (DRAW_ARGS);

  void (*draw_combo_button_frame) (DRAW_ARGS);

  void (*draw_common_background) (DRAW_ARGS);

  void (*draw_common_frame) (DRAW_ARGS);

  void (*draw_frame_gap) (DRAW_ARGS,
                          GtkPositionType gap_side,
                          gdouble         xy0_gap,
                          gdouble         xy1_gap);

  void (*draw_icon_view) (DRAW_ARGS);

  void (*draw_menubaritem_background) (DRAW_ARGS);

  void (*draw_menubaritem_frame) (DRAW_ARGS);

  void (*draw_notebook) (DRAW_ARGS,
                         GtkPositionType gap_side,
                         gdouble         xy0_gap,
                         gdouble         xy1_gap);

  void (*draw_progressbar_fill_background) (DRAW_ARGS);

  void (*draw_progressbar_fill_frame) (DRAW_ARGS);

  void (*draw_radio) (DRAW_ARGS);

  void (*draw_scrollbar_slider) (DRAW_ARGS);

  void (*draw_scrollbar_stepper_background) (DRAW_ARGS);

  void (*draw_scrollbar_stepper_frame) (DRAW_ARGS);

  void (*draw_scrolled_window_frame) (DRAW_ARGS);

  void (*draw_separator) (DRAW_ARGS);

  void (*draw_switch) (DRAW_ARGS,
                       GtkOrientation orientation);

  void (*draw_slider_button) (DRAW_ARGS,
                              UnicoSliderParameters *slider);

  void (*draw_tab) (DRAW_ARGS,
                    GtkPositionType gap_side);
};

G_END_DECLS

#endif /* UNICO_TYPES_H */
