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

#ifndef UNICO_ENGINE_H
#define UNICO_ENGINE_H

#include <gtk/gtk.h>

#include "unico-types.h"

G_BEGIN_DECLS

#define UNICO_TYPE_ENGINE            (unico_engine_get_type ())
#define UNICO_ENGINE(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), UNICO_TYPE_ENGINE, UnicoEngine))
#define UNICO_ENGINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), UNICO_TYPE_ENGINE, UnicoEngineClass))
#define UNICO_IS_ENGINE(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), UNICO_TYPE_ENGINE))
#define UNICO_IS_ENGINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), UNICO_TYPE_ENGINE))
#define UNICO_ENGINE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), UNICO_TYPE_ENGINE, UnicoEngineClass))

typedef struct _UnicoEngine UnicoEngine;
typedef struct _UnicoEngineClass UnicoEngineClass;

struct _UnicoEngine
{
  GtkThemingEngine parent_instance;
  UnicoStyleFunctions style_functions[UNICO_NUM_STYLES];
  UnicoStyles style;
};

struct _UnicoEngineClass
{
  GtkThemingEngineClass parent_class;
  UnicoStyleFunctions style_functions[UNICO_NUM_STYLES];
};

G_GNUC_INTERNAL void  unico_engine_register_types (GTypeModule *module);

G_GNUC_INTERNAL GType unico_engine_get_type       (void);

G_END_DECLS

#endif /* UNICO_ENGINE_H */
