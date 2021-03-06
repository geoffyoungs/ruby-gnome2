/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkdialog.c -

  $Author: ggc $
  $Date: 2007/07/13 16:07:31 $

  Copyright (C) 2002-2005 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#include "global.h"

#define _SELF(self) (GTK_DIALOG(RVAL2GOBJ(self)))

static ID id_to_a;

static VALUE
dialog_add_button(VALUE self, VALUE button_text, VALUE response_id)
{
    const gchar *name;
    if (SYMBOL_P(button_text)) {
        name = rb_id2name(SYM2ID(button_text));
    } else {
        name = RVAL2CSTR(button_text);
    }
    return GOBJ2RVAL(gtk_dialog_add_button(_SELF(self), name, 
                                           RVAL2GENUM(response_id, GTK_TYPE_RESPONSE_TYPE)));
}

struct rbgtk_dialog_add_buttons_internal_args {
    VALUE self;
    VALUE buttons;
};

static VALUE
rbgtk_dialog_add_buttons_internal_body(VALUE value)
{
    struct rbgtk_dialog_add_buttons_internal_args *args = (struct rbgtk_dialog_add_buttons_internal_args *)value;
    long i;
    long n = RARRAY_LEN(args->buttons);

    for (i = 0; i < n; i++) {
        VALUE button = rb_ary_to_ary(RARRAY_PTR(args->buttons)[i]);

        dialog_add_button(args->self, RARRAY_PTR(button)[0], RARRAY_PTR(button)[1]);
    }

    return args->self;
}

static VALUE
rbgtk_dialog_add_buttons_internal_ensure(VALUE value)
{
    g_object_thaw_notify(RVAL2GOBJ(((struct rbgtk_dialog_add_buttons_internal_args *)value)->self));

    return Qnil;
}

VALUE
rbgtk_dialog_add_buttons_internal(VALUE self, VALUE buttons)
{
    struct rbgtk_dialog_add_buttons_internal_args args = { self, buttons };

    if (NIL_P(RARRAY_PTR(buttons)[0]))
        return self;

    g_object_freeze_notify(RVAL2GOBJ(self));

    return rb_ensure(rbgtk_dialog_add_buttons_internal_body, (VALUE)&args,
                     rbgtk_dialog_add_buttons_internal_ensure, (VALUE)&args);
}    

static VALUE
dialog_add_buttons(int argc, VALUE *argv, VALUE self)
{
    VALUE button_ary;
    rb_scan_args(argc, argv, "*", &button_ary);
    rbgtk_dialog_add_buttons_internal(self, button_ary);
    return self;
}    

static VALUE
dialog_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE title, parent, flags, button_ary;
    rb_scan_args(argc, argv, "03*", &title, &parent, &flags, &button_ary);

    if (argc == 0){
        RBGTK_INITIALIZE(self, gtk_dialog_new());
    } else if (argc > 0){
        GtkDialog* dialog = GTK_DIALOG(g_object_new(GTK_TYPE_DIALOG, NULL));
        GtkDialogFlags gflags = NIL_P(flags) ? 0 : RVAL2GFLAGS(flags, GTK_TYPE_DIALOG_FLAGS);
        if (! NIL_P(title))
            gtk_window_set_title(GTK_WINDOW(dialog), RVAL2CSTR(title));
        if (! NIL_P(parent))
            gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(RVAL2GOBJ(parent)));
        
        if (gflags & GTK_DIALOG_MODAL)
            gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        if (gflags & GTK_DIALOG_DESTROY_WITH_PARENT)
            gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);
        if (gflags & GTK_DIALOG_NO_SEPARATOR)
            gtk_dialog_set_has_separator(dialog, FALSE);

        RBGTK_INITIALIZE(self, dialog);
        if (! NIL_P(button_ary))
            rbgtk_dialog_add_buttons_internal(self, button_ary);
    } else {
        rb_raise(rb_eArgError, "invalid argument number");
    }
    return Qnil;
}

static VALUE
dialog_run(VALUE self)
{
    if (rb_block_given_p()){
        VALUE ret = INT2NUM(gtk_dialog_run(_SELF(self)));
        rb_yield(ret);
        return ret;
    } else {
        return INT2NUM(gtk_dialog_run(_SELF(self)));
    }
}

static VALUE
dialog_response(VALUE self, VALUE response_id)
{
    gtk_dialog_response(_SELF(self), NUM2INT(response_id));
    return self;
}

static VALUE
dialog_add_action_widget(VALUE self, VALUE child, VALUE response_id)
{
    gtk_dialog_add_action_widget(_SELF(self), GTK_WIDGET(RVAL2GOBJ(child)), 
                                 NUM2INT(response_id));
    return self;
}

static VALUE
dialog_set_default_response(VALUE self, VALUE response_id)
{
    gtk_dialog_set_default_response(_SELF(self), NUM2INT(response_id));
    return self;
}

static VALUE
dialog_set_response_sensitive(VALUE self, VALUE response_id, VALUE setting)
{
    gtk_dialog_set_response_sensitive(_SELF(self), NUM2INT(response_id), RVAL2CBOOL(setting));
    return self;
}

#if GTK_CHECK_VERSION(2,6,0)
static VALUE
dialog_s_alternative_dialog_button_order(VALUE self, VALUE screen)
{
    gboolean ret = gtk_alternative_dialog_button_order(NIL_P(screen) ? NULL : 
                                                       GDK_SCREEN(RVAL2GOBJ(screen)));
    return CBOOL2RVAL(ret);
}

/* Use gtk_dialog_set_alternative_button_order_from_array() instead.
void        gtk_dialog_set_alternative_button_order
                                            (GtkDialog *dialog,
                                             gint first_response_id,
                                             ...);
*/

static VALUE
dialog_set_alternative_button_order(VALUE self, VALUE rbnew_order)
{
    GtkDialog *dialog = _SELF(self);
    long n;
    gint *new_order = RVAL2GINTS(rbnew_order, &n);

    gtk_dialog_set_alternative_button_order_from_array(dialog, n, new_order);

    g_free(new_order);

    return self;
}

#endif

static VALUE
dialog_vbox(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->vbox);
}

static VALUE
dialog_action_area(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->action_area);
}

#if GTK_CHECK_VERSION(2,8,0)
static VALUE
dialog_get_response_for_widget(VALUE self, VALUE widget)
{
    return INT2NUM(gtk_dialog_get_response_for_widget(_SELF(self), RVAL2GOBJ(widget)));
}
#endif

void 
Init_gtk_dialog()
{
    VALUE gDialog = G_DEF_CLASS(GTK_TYPE_DIALOG, "Dialog", mGtk);

    id_to_a = rb_intern("to_a");

    rb_define_method(gDialog, "initialize", dialog_initialize, -1);
    rb_define_method(gDialog, "run", dialog_run, 0);
    rb_define_method(gDialog, "response", dialog_response, 1);
    rb_define_method(gDialog, "add_button", dialog_add_button, 2);
    rb_define_method(gDialog, "add_buttons", dialog_add_buttons, -1);
    rb_define_method(gDialog, "add_action_widget", dialog_add_action_widget, 2);
    rb_define_method(gDialog, "set_default_response", dialog_set_default_response, 1);
    G_DEF_SETTER(gDialog, "default_response");

#if GTK_CHECK_VERSION(2,6,0)
    rb_define_singleton_method(gDialog, "alternative_dialog_button_order?", 
                               dialog_s_alternative_dialog_button_order, 1);
    rb_define_method(gDialog, "set_alternative_button_order", dialog_set_alternative_button_order, 1);
#endif

    rb_define_method(gDialog, "set_response_sensitive", dialog_set_response_sensitive, 2);
    rb_define_method(gDialog, "vbox", dialog_vbox, 0);
    rb_define_method(gDialog, "action_area", dialog_action_area, 0);

#if GTK_CHECK_VERSION(2,8,0)
    rb_define_method(gDialog, "get_response", dialog_get_response_for_widget, 1);
    rb_define_method(gDialog, "get_response_for_widget", dialog_get_response_for_widget, 1);
#endif

    /* GtkDialogFlags */
    G_DEF_CLASS(GTK_TYPE_DIALOG_FLAGS, "Flags", gDialog);
    G_DEF_CONSTANTS(gDialog, GTK_TYPE_DIALOG_FLAGS, "GTK_DIALOG_");

    /* GtkResponseType */
    G_DEF_CLASS(GTK_TYPE_RESPONSE_TYPE, "ResponseType", gDialog);
    G_DEF_CONSTANTS(gDialog, GTK_TYPE_RESPONSE_TYPE, "GTK_");
}
