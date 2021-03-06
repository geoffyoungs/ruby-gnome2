/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************
 
  rbgtkradioaction.c -
 
  $Author: mutoh $
  $Date: 2006/12/26 16:11:13 $
 
  Copyright (C) 2004-2006 Masao Mutoh
************************************************/
 
#include "global.h"

#if GTK_CHECK_VERSION(2,4,0)

#define _SELF(self) (GTK_RADIO_ACTION(RVAL2GOBJ(self)))

static VALUE
raction_initialize(VALUE self, VALUE name, VALUE label, VALUE tooltip, VALUE stock_id, VALUE value)
{
    const gchar *gstock = NULL;

    if (TYPE(stock_id) == T_STRING){
        gstock = RVAL2CSTR(stock_id);
    } else if (TYPE(stock_id) == T_SYMBOL) {
        gstock = rb_id2name(SYM2ID(stock_id));
    }
    G_INITIALIZE(self, gtk_radio_action_new(RVAL2CSTR(name),
                                            RVAL2CSTR(label),
                                            NIL_P(tooltip) ? NULL : RVAL2CSTR(tooltip),
                                            gstock,
                                            NUM2INT(value)));
    return Qnil;
}

static VALUE
raction_get_group(VALUE self)
{
    /* Owned by GTK+ */
    return GSLIST2ARY(gtk_radio_action_get_group(_SELF(self)));
}

struct rbgtk_rval2gtkradioactiongslist_args {
    VALUE ary;
    long n;
    GSList *result;
};

static VALUE
rbgtk_rval2gtkradioactiongslist_body(VALUE value)
{
    long i;
    struct rbgtk_rval2gtkradioactiongslist_args *args = (struct rbgtk_rval2gtkradioactiongslist_args *)value;

    for (i = 0; i < args->n; i++)
        args->result = g_slist_append(args->result, GTK_RADIO_ACTION(RVAL2GOBJ(RARRAY_PTR(args->ary)[i])));

    return Qnil;
}

static VALUE
rbgtk_rval2gtkradioactiongslist_rescue(VALUE value)
{
    g_slist_free(((struct rbgtk_rval2gtkradioactiongslist_args *)value)->result);

    rb_exc_raise(rb_errinfo());
}

static GSList *
rbgtk_rval2gtkradioactiongslist(VALUE value)
{
    struct rbgtk_rval2gtkradioactiongslist_args args;

    args.ary = rb_ary_to_ary(value);
    args.n = RARRAY_LEN(args.ary);
    args.result = NULL;

    rb_rescue(rbgtk_rval2gtkradioactiongslist_body, (VALUE)&args,
              rbgtk_rval2gtkradioactiongslist_rescue, (VALUE)&args);

    return args.result;
}

#define RVAL2GTKRADIOACTIONGSLIST(value) rbgtk_rval2gtkradioactiongslist(value)

static VALUE
raction_set_group(VALUE self, VALUE rbgroup)
{
    GtkRadioAction *action = _SELF(self);

    /* TODO: This might leak.  Use gtk_radio_action_join_group() in 3.0. */
    if (TYPE(rbgroup) == T_ARRAY)
        gtk_radio_action_set_group(action, RVAL2GTKRADIOACTIONGSLIST(rbgroup));
    else if (NIL_P(rbgroup))
        gtk_radio_action_set_group(action, NULL);
    else
        gtk_radio_action_set_group(action,
                                   gtk_radio_action_get_group(GTK_RADIO_ACTION(RVAL2GOBJ(rbgroup))));

    return self;
}

#if ! GTK_CHECK_VERSION(2,10,0)
static VALUE
raction_get_current_value(VALUE self)
{
    return INT2NUM(gtk_radio_action_get_current_value(_SELF(self)));
}
#endif
    
#endif

/* Defined as Properties
void        gtk_radio_action_set_current_value
                                            (GtkRadioAction *action,
                                             gint current_value);
*/

void 
Init_gtk_radio_action()
{
#if GTK_CHECK_VERSION(2,4,0)
    VALUE gRadioAction = G_DEF_CLASS(GTK_TYPE_RADIO_ACTION, "RadioAction", mGtk);

    rb_define_method(gRadioAction, "initialize", raction_initialize, 5);
#if ! GTK_CHECK_VERSION(2,10,0)
    /* Define as Property since 2.10 */
    rb_define_method(gRadioAction, "current_value", raction_get_current_value, 0);
#endif
    G_REPLACE_GET_PROPERTY(gRadioAction, "group", raction_get_group, 0);
    G_REPLACE_SET_PROPERTY(gRadioAction, "group", raction_set_group, 1);
#endif
}

