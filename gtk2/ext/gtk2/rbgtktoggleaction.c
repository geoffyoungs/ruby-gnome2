/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************
 
  rbgtktoggleaction.c -
 
  $Author: ggc $
  $Date: 2007/07/13 16:07:32 $
 
  Copyright (C) 2004,2005 Masao Mutoh
************************************************/
 
#include "global.h"

#if GTK_CHECK_VERSION(2,4,0)

#define _SELF(self) (GTK_TOGGLE_ACTION(RVAL2GOBJ(self)))

static VALUE
taction_initialize(VALUE self, VALUE name, VALUE label, VALUE tooltip, VALUE stock_id)
{
    const gchar *gstockid = NULL;

    if (TYPE(stock_id) == T_SYMBOL){
        gstockid = rb_id2name(SYM2ID(stock_id));
    } else if (TYPE(stock_id) == T_STRING){
        gstockid = RVAL2CSTR(stock_id);
    }

    G_INITIALIZE(self, gtk_toggle_action_new(RVAL2CSTR(name),
                                             RVAL2CSTR(label),
                                             NIL_P(tooltip) ? NULL : RVAL2CSTR(tooltip),
                                             gstockid));
    return Qnil;
}

static VALUE
taction_toggled(VALUE self)
{
    gtk_toggle_action_toggled(_SELF(self));
    return self;
}

/* Defined as Properties:
void                gtk_toggle_action_set_active        (GtkToggleAction *action,
                                                         gboolean is_active);
gboolean            gtk_toggle_action_get_active        (GtkToggleAction *action);
*/
#endif

void 
Init_gtk_toggle_action()
{
#if GTK_CHECK_VERSION(2,4,0)

    VALUE gToggleAction = G_DEF_CLASS(GTK_TYPE_TOGGLE_ACTION, "ToggleAction", mGtk);

    rb_define_method(gToggleAction, "initialize", taction_initialize, 4);
    rb_define_method(gToggleAction, "toggled", taction_toggled, 0);
#endif
}

