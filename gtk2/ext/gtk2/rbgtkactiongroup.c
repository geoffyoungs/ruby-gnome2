/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkactiongroup.c -

  $Author: ggc $
  $Date: 2007/07/13 16:07:31 $

  Copyright (C) 2004-2006 Masao Mutoh
************************************************/

#include "global.h"

#if GTK_CHECK_VERSION(2,4,0)

static ID id_action_procs;
static ID id_toggle_action_procs;

#define _SELF(self) (GTK_ACTION_GROUP(RVAL2GOBJ(self)))
#define RVAL2WIDGET(w) (GTK_WIDGET(RVAL2GOBJ(w)))

static VALUE
actiongroup_initialize(VALUE self, VALUE name)
{
    G_INITIALIZE(self, gtk_action_group_new(RVAL2CSTR(name)));
    return Qnil;
}

/* Defined as properties
const gchar* gtk_action_group_get_name      (GtkActionGroup *action_group);
gboolean    gtk_action_group_get_sensitive  (GtkActionGroup *action_group);
void        gtk_action_group_set_sensitive  (GtkActionGroup *action_group,
                                             gboolean sensitive);
gboolean    gtk_action_group_get_visible    (GtkActionGroup *action_group);
void        gtk_action_group_set_visible    (GtkActionGroup *action_group,
                                             gboolean visible);
*/

static VALUE
actiongroup_get_action(VALUE self, VALUE action_name)
{
    return GOBJ2RVAL(gtk_action_group_get_action(_SELF(self), RVAL2CSTR(action_name)));
}

static VALUE
actiongroup_list_actions(VALUE self)
{
    return GLIST2ARYF(gtk_action_group_list_actions(_SELF(self)));
}

static VALUE
actiongroup_add_action(int argc, VALUE *argv, VALUE self)
{
    VALUE action, accelerator;

    rb_scan_args(argc, argv, "11", &action, &accelerator);

    gtk_action_group_add_action_with_accel(_SELF(self),
                                           GTK_ACTION(RVAL2GOBJ(action)),
                                           NIL_P(accelerator) ? NULL : RVAL2CSTR(accelerator));
    G_CHILD_ADD(self, action);

    return self;
}

static VALUE
actiongroup_remove_action(VALUE self, VALUE action)
{
    gtk_action_group_remove_action(_SELF(self), GTK_ACTION(RVAL2GOBJ(action)));
    G_CHILD_REMOVE(self, action);
    return self;
}

static void
activate_action(GtkAction *action, VALUE self)
{
    VALUE action_procs = rb_ivar_get(self, id_action_procs);
    VALUE proc = rb_hash_aref(action_procs, CSTR2RVAL(gtk_action_get_name(action)));
    if (! NIL_P(proc))
        rb_funcall(proc, id_call, 2, self, GOBJ2RVAL(action));
}

static VALUE
actiongroup_add_actions(VALUE self, VALUE entries)
{
    guint i;
    VALUE action_procs;
    guint n_entries = (guint)RARRAY_LEN(entries);
    GtkActionEntry* gentries = g_new0(GtkActionEntry, n_entries);

    if (rb_ivar_defined(self, id_action_procs) == Qtrue){
        action_procs = rb_ivar_get(self, id_action_procs);
    } else {
        action_procs = rb_hash_new();
    }

    for (i = 0; i < n_entries; i++){
        VALUE entry;
        int size;

	entry = RARRAY_PTR(entries)[i];
	size = RARRAY_LEN(entry);
        if (size < 1)
            rb_raise(rb_eArgError, "wrong array parameter");

        gentries[i].name = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[0]);
        gentries[i].callback = G_CALLBACK(activate_action);

        if (size < 2) continue;

        if (NIL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = NULL;
        } else if (SYMBOL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = rb_id2name(SYM2ID(RARRAY_PTR(entry)[1]));
        } else if (TYPE(RARRAY_PTR(entry)[1]) == T_STRING){
            gentries[i].stock_id = RVAL2CSTR(RARRAY_PTR(entry)[1]);
        } else{
            rb_raise(rb_eArgError,
		     "invalid argument %s (expect Symbol or String)",
                     rb_class2name(CLASS_OF(RARRAY_PTR(entry)[1])));
        }

        if (size < 3) continue;
        gentries[i].label = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[2]);
        if (size < 4) continue;
        gentries[i].accelerator = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[3]);
        if (size < 4) continue;
        gentries[i].tooltip = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[4]);
        if (size < 5) continue;
        rb_hash_aset(action_procs, RARRAY_PTR(entry)[0], RARRAY_PTR(entry)[5]);
    }
    rb_ivar_set(self, id_action_procs, action_procs);
    gtk_action_group_add_actions(_SELF(self), gentries, n_entries,
				 (gpointer)self);
    g_free(gentries);

    return self;
}

static void
activate_toggle_action(GtkAction *action, VALUE self)
{
    VALUE action_procs = rb_ivar_get(self, id_toggle_action_procs);
    VALUE proc = rb_hash_aref(action_procs, CSTR2RVAL(gtk_action_get_name(action)));
    if (! NIL_P(proc))
        rb_funcall(proc, id_call, 2, self, GOBJ2RVAL(action));
}

static VALUE
actiongroup_add_toggle_actions(VALUE self, VALUE entries)
{
    guint i;
    VALUE toggle_action_procs;
    guint n_entries = (guint)RARRAY_LEN(entries);
    GtkToggleActionEntry* gentries = g_new0(GtkToggleActionEntry, n_entries);

    if (rb_ivar_defined(self, id_toggle_action_procs) == Qtrue){
        toggle_action_procs = rb_ivar_get(self, id_toggle_action_procs);
    } else {
        toggle_action_procs = rb_hash_new();
    }

    for (i = 0; i < n_entries; i++){
        VALUE entry;
	int size;

	entry = RARRAY_PTR(entries)[i];
	size = RARRAY_LEN(entry);
        if (size < 1)
            rb_raise(rb_eArgError, "wrong array parameter");

        gentries[i].name = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[0]);
        gentries[i].callback = G_CALLBACK(activate_toggle_action);

        if (size < 2) continue;

        if (NIL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = NULL;
        } else if (SYMBOL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = rb_id2name(SYM2ID(RARRAY_PTR(entry)[1]));
        } else if (TYPE(RARRAY_PTR(entry)[1]) == T_STRING){
            gentries[i].stock_id = RVAL2CSTR(RARRAY_PTR(entry)[1]);
        } else{
            rb_raise(rb_eArgError, "invalid argument %s (expect Symbol or String)",
                     rb_class2name(CLASS_OF(RARRAY_PTR(entry)[1])));
        }

        if (size < 3) continue;
        gentries[i].label = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[2]);
        if (size < 4) continue;
        gentries[i].accelerator = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[3]);
        if (size < 4) continue;
        gentries[i].tooltip = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[4]);
        if (size < 5) continue;
        rb_hash_aset(toggle_action_procs,
		     RARRAY_PTR(entry)[0], RARRAY_PTR(entry)[5]);
        if (size < 6) continue;
        gentries[i].is_active = RVAL2CBOOL(RARRAY_PTR(entry)[6]);
    }
    rb_ivar_set(self, id_toggle_action_procs, toggle_action_procs);
    gtk_action_group_add_toggle_actions(_SELF(self), gentries, n_entries,
					(gpointer)self);
    g_free(gentries);

    return self;
}

static void
activate_radio_action(GtkAction *action, GtkRadioAction *current, VALUE func)
{
    if (! NIL_P(func))
        rb_funcall(func, id_call, 2, GOBJ2RVAL(action), GOBJ2RVAL(current));
}

static VALUE
actiongroup_add_radio_actions(int argc, VALUE *argv, VALUE self)
{
    VALUE entries, value, proc;
    guint i;
    guint n_entries;
    GtkRadioActionEntry* gentries;

    rb_scan_args(argc, argv, "12", &entries, &value, &proc);

    if (NIL_P(value)) value = -1;
    if (NIL_P(proc) && rb_block_given_p()){
        proc = rb_block_proc();
        G_RELATIVE(self, proc);
    }
    n_entries = (guint)RARRAY_LEN(entries);
    gentries = g_new0(GtkRadioActionEntry, n_entries);

    for (i = 0; i < n_entries; i++){
        VALUE entry;
	int size;

	entry = RARRAY_PTR(entries)[i];
	size = RARRAY_LEN(entry);
        if (size < 1)
            rb_raise(rb_eArgError, "wrong array parameter");

        gentries[i].name = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[0]);

        if (size < 2) continue;

        if (NIL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = NULL;
        } else if (SYMBOL_P(RARRAY_PTR(entry)[1])){
            gentries[i].stock_id = rb_id2name(SYM2ID(RARRAY_PTR(entry)[1]));
        } else if (TYPE(RARRAY_PTR(entry)[1]) == T_STRING){
            gentries[i].stock_id = RVAL2CSTR(RARRAY_PTR(entry)[1]);
        } else{
            rb_raise(rb_eArgError,
		     "invalid argument %s (expect Symbol or String)",
                     rb_class2name(CLASS_OF(RARRAY_PTR(entry)[1])));
        }

        if (size < 3) continue;
        gentries[i].label = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[2]);
        if (size < 4) continue;
        gentries[i].accelerator = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[3]);
        if (size < 4) continue;
        gentries[i].tooltip = RVAL2CSTR_ACCEPT_NIL(RARRAY_PTR(entry)[4]);
        if (size < 5) continue;
        gentries[i].value = NUM2INT(RARRAY_PTR(entry)[5]);
    }
    gtk_action_group_add_radio_actions(_SELF(self), gentries,
				       n_entries, NUM2INT(value),
                                       G_CALLBACK(activate_radio_action),
				       (gpointer)proc);
    g_free(gentries);

    return self;
}

static gchar *
translate_func(const gchar *path, gpointer func)
{
    VALUE ret = rb_funcall((VALUE)func, id_call, 1, CSTR2RVAL(path));
    return (gchar *)RVAL2CSTR(ret);
}

static VALUE
actiongroup_set_translate_func(VALUE self)
{
    VALUE func = rb_block_proc();
    G_RELATIVE(self, func);
    gtk_action_group_set_translate_func(_SELF(self), 
                                        (GtkTranslateFunc)translate_func, 
                                        (gpointer)func, NULL);
    return self;
}

static VALUE
actiongroup_set_translation_domain(VALUE self, VALUE domain)
{
    gtk_action_group_set_translation_domain(_SELF(self), 
                                            NIL_P(domain) ? (const gchar*)NULL : RVAL2CSTR(domain));
    return self;
}

#if GTK_CHECK_VERSION(2,6,0)
static VALUE
actiongroup_translate_string(VALUE self, VALUE str)
{
    return CSTR2RVAL(gtk_action_group_translate_string(_SELF(self), RVAL2CSTR(str)));
}
#endif

static void
action_group_mark(void *p)
{
    GtkActionGroup *group;
    GList *actions, *node;

    group = GTK_ACTION_GROUP(p);
    actions = gtk_action_group_list_actions(group);
    for (node = actions; node; node = g_list_next(node)) {
	GtkWidget *action = node->data;
	rbgobj_gc_mark_instance(action);
    }
    g_list_free(actions);
}
#endif

void 
Init_gtk_actiongroup()
{
#if GTK_CHECK_VERSION(2,4,0)
    VALUE gActionGroup;

    gActionGroup = G_DEF_CLASS_WITH_GC_FUNC(GTK_TYPE_ACTION_GROUP, "ActionGroup",
					    mGtk, action_group_mark, NULL);

    id_action_procs = rb_intern("@action_procs");
    id_toggle_action_procs = rb_intern("@toggle_action_procs");

    rb_define_method(gActionGroup, "initialize", actiongroup_initialize, 1);
    rb_define_method(gActionGroup, "get_action", actiongroup_get_action, 1);
    rb_define_method(gActionGroup, "actions", actiongroup_list_actions, 0);
    rb_define_method(gActionGroup, "add_action", actiongroup_add_action, -1);
    rb_define_method(gActionGroup, "remove_action", actiongroup_remove_action, 1);
    rb_define_method(gActionGroup, "add_actions", actiongroup_add_actions, 1);
    rb_define_method(gActionGroup, "add_toggle_actions", actiongroup_add_toggle_actions, 1);
    rb_define_method(gActionGroup, "add_radio_actions", actiongroup_add_radio_actions, -1);
    rb_define_method(gActionGroup, "set_translate_func", actiongroup_set_translate_func, 0);
    rb_define_method(gActionGroup, "set_translation_domain", actiongroup_set_translation_domain, 1);
    G_DEF_SETTER(gActionGroup, "translation_domain");
#if GTK_CHECK_VERSION(2,6,0)
    rb_define_method(gActionGroup, "translate_string", actiongroup_translate_string, 1);
#endif
#endif
}
