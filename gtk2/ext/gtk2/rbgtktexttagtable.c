/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtktexttagtable.c -
 
  $Author: ggc $
  $Date: 2007/07/13 16:07:32 $

  Copyright (C) 2002,2003 OGASAWARA, Takeshi
************************************************/
#include "global.h"

#define _SELF(self) (GTK_TEXT_TAG_TABLE(RVAL2GOBJ(self)))
#define RVAL2TAG(t) (GTK_TEXT_TAG(RVAL2GOBJ(t)))

static VALUE
txt_tt_initialize(VALUE self)
{
    G_INITIALIZE(self, gtk_text_tag_table_new());
    return Qnil;
}

static VALUE
txt_tt_add(VALUE self, VALUE tag)
{
    G_CHILD_ADD(self, tag);
    gtk_text_tag_table_add(_SELF(self), RVAL2TAG(tag));
    return self;
}

static VALUE
txt_tt_remove(VALUE self, VALUE tag)
{
    G_CHILD_REMOVE(self, tag);
    gtk_text_tag_table_remove(_SELF(self), RVAL2TAG(tag));
    return self;
}

static VALUE
txt_tt_lookup(VALUE self, VALUE name)
{
    VALUE ret = Qnil;
    GtkTextTag* tag = gtk_text_tag_table_lookup(_SELF(self), RVAL2CSTR(name));
    if (tag){
        ret = GOBJ2RVAL(tag);
        G_CHILD_ADD(self, ret);
    }
    return ret;
}

static gboolean
txt_tt_foreach_func(GtkTextTag *tag, gpointer func)
{
    return RVAL2CBOOL(rb_funcall((VALUE)func, id_call, 1, GOBJ2RVAL(tag)));
}

static VALUE
txt_tt_foreach(VALUE self)
{
    volatile VALUE func = rb_block_proc();
    gtk_text_tag_table_foreach(_SELF(self), 
                               (GtkTextTagTableForeach)txt_tt_foreach_func, 
                               (gpointer)func);
    return self;
}

static VALUE
txt_tt_get_size(VALUE self)
{
    return INT2NUM(gtk_text_tag_table_get_size(_SELF(self)));
}

void 
Init_txt_tt()
{
    VALUE gTextTagTable = G_DEF_CLASS(GTK_TYPE_TEXT_TAG_TABLE, "TextTagTable", mGtk);

    rb_define_method(gTextTagTable, "initialize", txt_tt_initialize, 0);
    rb_define_method(gTextTagTable, "add", txt_tt_add, 1);
    rb_define_method(gTextTagTable, "remove", txt_tt_remove, 1);
    rb_define_method(gTextTagTable, "lookup", txt_tt_lookup, 1);
    rb_define_alias(gTextTagTable, "[]", "lookup");
    rb_define_method(gTextTagTable, "each", txt_tt_foreach, 0);
    rb_define_method(gTextTagTable, "size", txt_tt_get_size, 0);
}

