/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkcolorsel.c -

  $Author: ggc $
  $Date: 2007/07/13 14:27:09 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

#define _SELF(s) (GTK_COLOR_SELECTION(RVAL2GOBJ(self)))

static VALUE gColorSel;

static VALUE
colorsel_initialize(VALUE self)
{
    RBGTK_INITIALIZE(self, gtk_color_selection_new());
    return Qnil;
}

static VALUE
colorsel_get_previous_alpha(VALUE self)
{
    return INT2NUM(gtk_color_selection_get_previous_alpha(_SELF(self)));
}

static VALUE
colorsel_set_previous_alpha(VALUE self, VALUE alpha)
{
    gtk_color_selection_set_previous_alpha(_SELF(self), NUM2INT(alpha));
    return self;
}

static VALUE
colorsel_set_previous_color(VALUE self, VALUE color)
{
    gtk_color_selection_set_previous_color(_SELF(self), RVAL2GDKCOLOR(color));
    return self;
}

static VALUE
colorsel_get_previous_color(VALUE self)
{
    GdkColor color;
    gtk_color_selection_get_previous_color(_SELF(self), &color);
    return GDKCOLOR2RVAL(&color);
}

static VALUE
colorsel_is_adjusting(VALUE self)
{
    return CBOOL2RVAL(gtk_color_selection_is_adjusting(_SELF(self)));
}

static VALUE
colorsel_s_palette_from_string(VALUE self, VALUE str)
{
    GdkColor* gcolors;
    gint i, n_colors;
    VALUE ary = Qnil;
    gboolean ret = gtk_color_selection_palette_from_string(RVAL2CSTR(str), 
                                                           &gcolors, &n_colors);
    
    if (ret) {
        ary = rb_ary_new();
        for (i = 0; i < n_colors; i++) {
            rb_ary_push(ary, GDKCOLOR2RVAL(&gcolors[i]));
        }
    }
                                                                                
    return ary;
}

struct rbgdk_rval2gdkcolors_args {
    VALUE ary;
    long n;
    GdkColor *result;
};

static VALUE
rbgdk_rval2gdkcolors_body(VALUE value)
{
    long i;
    struct rbgdk_rval2gdkcolors_args *args = (struct rbgdk_rval2gdkcolors_args *)value;

    for (i = 0; i < args->n; i++)
        args->result[i] = *RVAL2GDKCOLOR(RARRAY_PTR(args->ary)[i]);

    return Qnil;
}

static VALUE
rbgdk_rval2gdkcolors_rescue(VALUE value)
{
    g_free(((struct rbgdk_rval2gdkcolors_args *)value)->result);

    rb_exc_raise(rb_errinfo());
}

static GdkColor *
rbgdk_rval2gdkcolors(VALUE value, long *n)
{
    struct rbgdk_rval2gdkcolors_args args;

    args.ary = rb_ary_to_ary(value);
    args.n = RARRAY_LEN(args.ary);
    args.result = g_new(GdkColor, args.n + 1);

    rb_rescue(rbgdk_rval2gdkcolors_body, (VALUE)&args,
              rbgdk_rval2gdkcolors_rescue, (VALUE)&args);

    if (n != NULL)
        *n = args.n;

    return args.result;
}

#define RVAL2GDKCOLORS(value, n) rbgdk_rval2gdkcolors(value, n)

static VALUE
colorsel_s_palette_to_string(int argc, VALUE *argv, VALUE self)
{
    VALUE rbcolors;
    long n;
    GdkColor *colors;
    gchar *palette;

    if (argc > 1)
        rb_scan_args(argc, argv, "*", &rbcolors);
    else
        rb_scan_args(argc, argv, "10", &rbcolors);

    colors = RVAL2GDKCOLORS(rbcolors, &n);

    palette = gtk_color_selection_palette_to_string(colors, n);

    g_free(colors);

    return CSTR2RVAL_FREE(palette);
}

#if GTK_CHECK_VERSION(2,2,0)
static void
screen_func(GdkScreen *screen, const GdkColor *colors, gint n_colors)
{
    int i;
    VALUE func = rb_cvar_get(gColorSel, rb_intern("__palette_proc__"));
    VALUE ary = rb_ary_new();
    for (i = 0; i < n_colors; i++){
        ary = rb_ary_push(ary, GDKCOLOR2RVAL((GdkColor *)&colors[i]));
    }
    if (! NIL_P(func))
        rb_funcall(func, id_call, 2, GOBJ2RVAL(screen), ary);
}

static VALUE
colorsel_s_set_change_palette_hook(VALUE self)
{
    VALUE func = rb_block_proc();

    rb_cv_set(gColorSel, "__palette_proc__", func);
    gtk_color_selection_set_change_palette_with_screen_hook(
        (GtkColorSelectionChangePaletteWithScreenFunc)screen_func);
    return self;
}
#endif

/* Don't implement them.
GtkColorSelectionChangePaletteFunc gtk_color_selection_set_change_palette_hook
                                            (GtkColorSelectionChangePaletteFunc func);
void        (*GtkColorSelectionChangePaletteWithScreenFunc)
                                            (GdkScreen *screen,
                                             const GdkColor *colors,
                                             gint n_colors);
*/

void 
Init_gtk_color_selection()
{
    gColorSel = G_DEF_CLASS(GTK_TYPE_COLOR_SELECTION, "ColorSelection", mGtk);

    rb_define_method(gColorSel, "initialize", colorsel_initialize, 0);
    rb_define_method(gColorSel, "previous_alpha", colorsel_get_previous_alpha, 0);
    rb_define_method(gColorSel, "set_previous_alpha", colorsel_set_previous_alpha, 1);
    rb_define_method(gColorSel, "set_previous_color", colorsel_set_previous_color, 1);
    rb_define_method(gColorSel, "previous_color", colorsel_get_previous_color, 0);
    rb_define_method(gColorSel, "adjusting?", colorsel_is_adjusting, 0);

    rb_define_singleton_method(gColorSel, "palette_to_string", colorsel_s_palette_to_string, -1);
    rb_define_singleton_method(gColorSel, "palette_from_string", colorsel_s_palette_from_string, 1);

#if GTK_CHECK_VERSION(2,2,0)
    rb_define_singleton_method(gColorSel, "set_change_palette_hook", colorsel_s_set_change_palette_hook, 0);
#endif

    G_DEF_SETTERS(gColorSel);
}
