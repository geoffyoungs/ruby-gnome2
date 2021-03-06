/* -*- c-file-style: "ruby" -*- */
/*
 * Ruby/GIO: a Ruby binding of gio-2.0.x.
 * Copyright (C) 2008-2009  Ruby-GNOME2 Project Team
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "gio2.h"

#define _SELF(value) RVAL2GASYNCRESULT(value)

/* NOTE: g_async_result_get_user_data isn't of any use from Ruby. */

static VALUE
asyncresult_get_source_object(VALUE self)
{
        return GOBJ2RVAL_UNREF(g_async_result_get_source_object(_SELF(self)));
}

void
Init_gasyncresult(VALUE glib)
{
        VALUE asyncresult = G_DEF_INTERFACE(G_TYPE_ASYNC_RESULT, "AsyncResult", glib);

        rb_define_method(asyncresult, "source_object", asyncresult_get_source_object, 0);
}
