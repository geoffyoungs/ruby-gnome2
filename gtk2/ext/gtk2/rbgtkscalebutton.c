/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkscalebutton.c -

  $Author: ggc $
  $Date: 2007/07/12 14:43:11 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

#if GTK_CHECK_VERSION(2,12,0)

#define _SELF(s) (GTK_SCALE_BUTTON(RVAL2GOBJ(s)))

static VALUE
scalebutton_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE arg1, arg2, arg3, arg4, arg5;
    GtkWidget *widget;
    gdouble min = 0;
    gdouble max = 100;
    gdouble step = 2;

    rb_scan_args(argc, argv, "14", &arg1, &arg2, &arg3, &arg4, &arg5);

    if (!NIL_P(arg2))
        min = NUM2DBL(arg2);

    if (!NIL_P(arg3))
        max = NUM2DBL(arg3);

    if (!NIL_P(arg4))
        step = NUM2DBL(arg4);

    widget = gtk_scale_button_new(RVAL2GENUM(arg1, GTK_TYPE_ICON_SIZE), min, max, step, NULL);
    RBGTK_INITIALIZE(self, widget);

    if (!NIL_P(arg5)) {
        const gchar **icons = RVAL2STRV(arg5);

        gtk_scale_button_set_icons(_SELF(self), icons);

        g_free(icons);
    }

    return Qnil;
}

/* Defined as Properties
void                gtk_scale_button_set_adjustment     (GtkScaleButton *button,
                                                         GtkAdjustment *adjustment);
void                gtk_scale_button_set_icons          (GtkScaleButton *button,
                                                         const gchar **icons);
void                gtk_scale_button_set_value          (GtkScaleButton *button,
                                                         gdouble value);
GtkAdjustment *     gtk_scale_button_get_adjustment     (GtkScaleButton *button);
gdouble             gtk_scale_button_get_value          (GtkScaleButton *button);
*/

#endif

void 
Init_gtk_scalebutton()
{
#if GTK_CHECK_VERSION(2,12,0)
    VALUE gScaleButton = G_DEF_CLASS(GTK_TYPE_SCALE_BUTTON, "ScaleButton", mGtk);
    rb_define_method(gScaleButton, "initialize", scalebutton_initialize, -1);
#endif
}
