/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkfontselectiondialog.c -

  $Author: ggc $
  $Date: 2007/07/13 14:27:09 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 2001 Neil Conway <neilconway@rogers.com>
************************************************/

#include "global.h"

#define _SELF(self) (GTK_FONT_SELECTION_DIALOG(RVAL2GOBJ(self)))

static VALUE
fsd_initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE title;
    rb_scan_args(argc, argv, "01", &title);
    RBGTK_INITIALIZE(self, gtk_font_selection_dialog_new(NIL_P(title) ? NULL : RVAL2CSTR(title)));
    return Qnil;
}

static VALUE
fsd_get_font_name(VALUE self)
{
    gchar* name = gtk_font_selection_dialog_get_font_name(_SELF(self));
    return name ? CSTR2RVAL(name) : Qnil;
}

static VALUE
fsd_set_font_name(VALUE self, VALUE fontname)
{
    return CBOOL2RVAL(gtk_font_selection_dialog_set_font_name(_SELF(self),
                                                              RVAL2CSTR(fontname)));
}

static VALUE
fsd_get_preview_text(VALUE self)
{
    const gchar* text = gtk_font_selection_dialog_get_preview_text(_SELF(self));
    return text ? CSTR2RVAL(text) : Qnil;
}

static VALUE
fsd_set_preview_text(VALUE self, VALUE text)
{
    gtk_font_selection_dialog_set_preview_text(_SELF(self), RVAL2CSTR(text));
    return self;
}

static VALUE
fsd_get_font_selection(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->fontsel);
}

static VALUE
fsd_get_ok_button(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->ok_button);
}

static VALUE
fsd_get_cancel_button(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->cancel_button);
}

static VALUE
fsd_get_apply_button(VALUE self)
{
    return GOBJ2RVAL(_SELF(self)->apply_button);
}

void 
Init_gtk_font_selection_dialog()
{
    VALUE gFontSelectionDialog = G_DEF_CLASS(GTK_TYPE_FONT_SELECTION_DIALOG, 
                                             "FontSelectionDialog", mGtk);

    rb_define_method(gFontSelectionDialog, "initialize", fsd_initialize, -1);
    rb_define_method(gFontSelectionDialog, "font_name", fsd_get_font_name, 0);
    rb_define_method(gFontSelectionDialog, "set_font_name", fsd_set_font_name, 1);
    rb_define_method(gFontSelectionDialog, "preview_text", fsd_get_preview_text, 0);
    rb_define_method(gFontSelectionDialog, "set_preview_text", fsd_set_preview_text, 1);
    rb_define_method(gFontSelectionDialog, "font_selection", fsd_get_font_selection, 0);
    rb_define_method(gFontSelectionDialog, "ok_button", fsd_get_ok_button, 0);
    rb_define_method(gFontSelectionDialog, "cancel_button", fsd_get_cancel_button, 0);
    rb_define_method(gFontSelectionDialog, "apply_button", fsd_get_apply_button, 0);

    G_DEF_SETTERS(gFontSelectionDialog);
}
