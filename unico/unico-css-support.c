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

#include "unico-css-support.h"

GType
unico_stroke_style_get_type (void)
{
  static GType etype = 0;

  if (G_UNLIKELY (etype == 0))
    {
      static const GEnumValue values[] = {
        { UNICO_STROKE_STYLE_CUSTOM, "UNICO_STROKE_STYLE_CUSTOM", "custom" },
        { UNICO_STROKE_STYLE_INSET, "UNICO_STROKE_STYLE_INSET", "inset" },
        { UNICO_STROKE_STYLE_NONE, "UNICO_STROKE_STYLE_NONE", "none" },
        { 0, NULL, NULL }
      };

      etype = g_enum_register_static (g_intern_static_string ("UnicoStrokeStyle"), values);
    }

  return etype;
}

