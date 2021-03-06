/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkaccellabel.c -

  $Author: mutoh $
  $Date: 2003/02/01 16:46:23 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 2001 Neil Conway
************************************************/

#include "global.h"

static VALUE
alabel_initialize(VALUE self, VALUE string)
{
    RBGTK_INITIALIZE(self, gtk_accel_label_new(RVAL2CSTR(string)));
    return Qnil;
}

static VALUE
alabel_get_accel_width(VALUE self)
{
    return INT2NUM(gtk_accel_label_get_accel_width(GTK_ACCEL_LABEL(RVAL2GOBJ(self))));
}

static VALUE
alabel_refetch(VALUE self)
{
    gtk_accel_label_refetch(GTK_ACCEL_LABEL(RVAL2GOBJ(self)));
    return Qfalse; // always returns false
}

void
Init_gtk_accel_label()
{
  VALUE gAccelLabel = G_DEF_CLASS(GTK_TYPE_ACCEL_LABEL, "AccelLabel", mGtk);

  rb_define_method(gAccelLabel, "initialize", alabel_initialize, 1);
  rb_define_method(gAccelLabel, "accel_width", alabel_get_accel_width, 0);
  rb_define_method(gAccelLabel, "refetch", alabel_refetch, 0);
}
