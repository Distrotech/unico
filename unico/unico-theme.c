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

#include <gmodule.h>
#include <gtk/gtk.h>

#include "unico-engine.h"

G_MODULE_EXPORT void theme_init (GTypeModule *module);

G_MODULE_EXPORT void theme_exit (void);

G_MODULE_EXPORT GtkThemingEngine* create_engine (void);

G_MODULE_EXPORT void 
theme_init (GTypeModule *module)
{
  unico_engine_register_types (module);
}

G_MODULE_EXPORT void 
theme_exit (void)
{
}

G_MODULE_EXPORT GtkThemingEngine*
create_engine (void)
{
  return GTK_THEMING_ENGINE (g_object_new (UNICO_TYPE_ENGINE,
                                           "name", "unico",
                                           NULL));
}
