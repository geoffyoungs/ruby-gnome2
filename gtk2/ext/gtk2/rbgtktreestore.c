/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtktreestore.c -

  $Author: ggc $
  $Date: 2007/07/10 08:07:15 $

  Copyright (C) 2002-2006 Masao Mutoh
************************************************/

#include "global.h"

#define _SELF(s) (GTK_TREE_STORE(RVAL2GOBJ(s)))

static VALUE
tstore_initialize(int argc, VALUE *argv, VALUE self)
{
    gint cnt;
    GtkTreeStore* store;
    GType* buf;

    if (argc == 0) rb_raise(rb_eArgError, "need more than 1 class type.");

    buf = ALLOCA_N(GType, argc);
    for (cnt = 0; cnt < argc; cnt++) {
        buf[cnt] = CLASS2GTYPE(argv[cnt]);
    }

    store = gtk_tree_store_newv(argc, buf);
 
    G_INITIALIZE(self, store);

    return Qnil;
}

static VALUE
tstore_set_column_types(int argc, VALUE *argv, VALUE self)
{
    gint cnt;
    GType* buf;
  
    if (argc == 0) rb_raise(rb_eArgError, "need more than 1 class type.");
    buf = ALLOCA_N(GType, argc);
    for (cnt = 0; cnt < argc; cnt++) {
        buf[cnt] = CLASS2GTYPE(argv[cnt]);
    }

    gtk_tree_store_set_column_types(_SELF(self), argc, buf);

    return self;
}

static VALUE
tstore_set_value(VALUE self, VALUE iter, VALUE column, VALUE value)
{
    GType gtype = gtk_tree_model_get_column_type(GTK_TREE_MODEL(RVAL2GOBJ(self)), NUM2INT(column));
    GValue gval = {0,};
    g_value_init(&gval, gtype);

    rbgobj_rvalue_to_gvalue(value, &gval);

    G_CHILD_ADD(self, iter);
    G_CHILD_ADD(iter, value);

    gtk_tree_store_set_value(_SELF(self), RVAL2GTKTREEITER(iter), NUM2INT(column), &gval);

    g_value_unset(&gval);
    return self;
}

/*
  void        gtk_tree_store_set (GtkTreeStore *tree_store,
                                               GtkTreeIter *iter,
					                                                    ...);

  void        gtk_tree_store_set_valist       (GtkTreeStore *tree_store,
  GtkTreeIter *iter,
  va_list var_args);
*/
    
static VALUE
tstore_remove(VALUE self, VALUE iter)
{
    G_CHILD_REMOVE(self, iter);
#if GTK_CHECK_VERSION(2,2,0)
    return CBOOL2RVAL(gtk_tree_store_remove(_SELF(self), RVAL2GTKTREEITER(iter)));
#else
    gtk_tree_store_remove(_SELF(self), RVAL2GTKTREEITER(iter));
    return Qtrue;
#endif
}

static VALUE
tstore_insert(int argc, VALUE *argv, VALUE self)
{
    VALUE parent, position, values, ret;
    GtkTreeIter iter;
    GtkTreeStore* model = _SELF(self);

    rb_scan_args(argc, argv, "21", &parent, &position, &values);

    if (NIL_P(values)){
        gtk_tree_store_insert(model, &iter, 
                              NIL_P(parent) ? NULL : RVAL2GTKTREEITER(parent), 
                              NUM2INT(position));
        iter.user_data3 = model;
        ret = GTKTREEITER2RVAL(&iter);
        G_CHILD_ADD(self, ret);
    } else {
#if GTK_CHECK_VERSION(2,10,0)
        gint *c_columns;
        GValue *c_values;
        long size, i;

        size = NUM2INT(rb_funcall(values, rb_intern("size"), 0));
        c_columns = ALLOCA_N(gint, size);
        c_values = ALLOCA_N(GValue, size);
        
        if(TYPE(values)==T_ARRAY) {
            for(i=0; i<size; i++) {
                GType gtype;
                GValue gval = {0,};

                c_columns[i] = i;
                gtype = gtk_tree_model_get_column_type(GTK_TREE_MODEL(RVAL2GOBJ(self)),
                                                       c_columns[i]);
        
                g_value_init(&gval, gtype);
                rbgobj_rvalue_to_gvalue(rb_ary_shift(values), &gval);
                c_values[i] = gval;
            }
        } else if(TYPE(values)==T_HASH) {
            VALUE r_columns;
            r_columns = rb_funcall(values, rb_intern("keys"), 0);
            
            for(i=0; i<size; i++) {
                GType gtype;
                GValue gval = {0,};

                c_columns[i] = NUM2INT (rb_ary_entry(r_columns, i));
                gtype = gtk_tree_model_get_column_type(GTK_TREE_MODEL(RVAL2GOBJ(self)),
                                                       c_columns[i]);
                
                g_value_init(&gval, gtype);
                rbgobj_rvalue_to_gvalue(rb_hash_aref(values, INT2NUM(c_columns[i])), &gval);
                c_values[i] = gval;
            }
        } else {
            rb_raise(rb_eArgError, "values must be of type Hash or Array");
        }

        gtk_tree_store_insert_with_valuesv(model, &iter,
                                           NIL_P(parent) ? NULL : RVAL2GTKTREEITER(parent),
                                           NUM2INT(position),
                                           c_columns,
                                           c_values,
                                           size);
        iter.user_data3 = model;

        ret = GTKTREEITER2RVAL(&iter);
        G_CHILD_ADD(self, ret);

        for(i=0; i<size; i++) {
            G_CHILD_ADD(ret, rbgobj_gvalue_to_rvalue(&(c_values[i])));
            g_value_unset(&(c_values[i]));
        }
#else
        rb_warn("Gtk::TreeStore#insert(parent, position, values) requires GTK+-2.10.0 or later");
        gtk_tree_store_insert(model, &iter, 
                              NIL_P(parent) ? NULL : RVAL2GTKTREEITER(parent), 
                              NUM2INT(position));
        iter.user_data3 = model;
        ret = GTKTREEITER2RVAL(&iter);
        G_CHILD_ADD(self, ret);
#endif
    }

    return ret;
}


static VALUE
tstore_insert_before(VALUE self, VALUE parent, VALUE sibling)
{
    VALUE ret;
    GtkTreeIter iter;
    GtkTreeStore* model = _SELF(self);
    gtk_tree_store_insert_before(model, &iter, 
                                 NIL_P(parent) ? NULL : RVAL2GTKTREEITER(parent), 
                                 NIL_P(sibling) ? NULL : RVAL2GTKTREEITER(sibling));
    iter.user_data3 = model;
    ret = GTKTREEITER2RVAL(&iter);
    G_CHILD_ADD(self, ret);
    return ret;
}

static VALUE
tstore_insert_after(VALUE self, VALUE parent, VALUE sibling)
{ 
    VALUE ret;
    GtkTreeIter iter;
    GtkTreeStore* model = _SELF(self);
    gtk_tree_store_insert_after(model, &iter, 
                                NIL_P(parent) ? NULL : RVAL2GTKTREEITER(parent), 
                                NIL_P(sibling) ? NULL : RVAL2GTKTREEITER(sibling));
    iter.user_data3 = model;

    ret = GTKTREEITER2RVAL(&iter);
    G_CHILD_ADD(self, ret);
    return ret;
}

/* The 2 following functions where implemented as TreeStore#insert
   which will use a Hash instead of pairs of parameters

void        gtk_tree_store_insert_with_values
                                            (GtkTreeStore *tree_store,
                                             GtkTreeIter *iter,
                                             GtkTreeIter *parent,
                                             gint position,
                                             ...);

void        gtk_tree_store_insert_with_valuesv
                                            (GtkTreeStore *tree_store,
                                             GtkTreeIter *iter,
                                             GtkTreeIter *parent,
                                             gint position,
                                             gint *columns,
                                             GValue *values,
                                             gint n_values);

*/

static VALUE
tstore_prepend(VALUE self, VALUE parent)
{
    VALUE ret;
    GtkTreeIter iter;
    GtkTreeStore* model = _SELF(self);
    gtk_tree_store_prepend(model, &iter, 
                           NIL_P(parent)?NULL:RVAL2GTKTREEITER(parent));
    iter.user_data3 = model;

    ret = GTKTREEITER2RVAL(&iter);
    G_CHILD_ADD(self, ret);
    return ret;
}

static VALUE
tstore_append(VALUE self, VALUE parent)
{
    VALUE ret;
    GtkTreeIter iter;
    GtkTreeStore* model = _SELF(self);
    gtk_tree_store_append(model, &iter, 
                          NIL_P(parent)?NULL:RVAL2GTKTREEITER(parent));
    iter.user_data3 = model;

    ret = GTKTREEITER2RVAL(&iter);
    G_CHILD_ADD(self, ret);
    return ret;
}

static VALUE
tstore_is_ancestor(VALUE self, VALUE iter, VALUE descendant)
{
    return gtk_tree_store_is_ancestor(_SELF(self), RVAL2GTKTREEITER(iter), 
                                      RVAL2GTKTREEITER(descendant));
}

static VALUE
tstore_iter_depth(VALUE self, VALUE iter)
{
    return INT2NUM(gtk_tree_store_iter_depth(_SELF(self), RVAL2GTKTREEITER(iter)));
}

static VALUE
tstore_clear(VALUE self)
{
    G_CHILD_REMOVE_ALL(self);
    gtk_tree_store_clear(_SELF(self));
    return self;
}

#if GTK_CHECK_VERSION(2,2,0)
static VALUE
tstore_iter_is_valid(VALUE self, VALUE iter)
{
    return (NIL_P(iter)) ? Qfalse : 
        CBOOL2RVAL(gtk_tree_store_iter_is_valid(_SELF(self), RVAL2GTKTREEITER(iter)));
}

static VALUE
tstore_reorder(VALUE self, VALUE rbparent, VALUE rbnew_order)
{
    GtkTreeStore *store = _SELF(self);
    GtkTreeIter *parent = RVAL2GTKTREEITER(rbparent);
    gint columns = gtk_tree_model_get_n_columns(GTK_TREE_MODEL(store));
    long n;
    gint *new_order = RVAL2GINTS(rbnew_order, &n);

    if (n != columns) {
        g_free(new_order);

        rb_raise(rb_eArgError,
                 "new order array must contain same number of elements as the number of columns in the store: %ld != %d",
                 n, columns);
    }
  
    gtk_tree_store_reorder(store, parent, new_order);

    g_free(new_order);

    return self;
}

static VALUE
tstore_swap(VALUE self, VALUE iter1, VALUE iter2)
{
    gtk_tree_store_swap(_SELF(self), RVAL2GTKTREEITER(iter1), RVAL2GTKTREEITER(iter2));
    return self;
}

static VALUE
tstore_move_before(VALUE self, VALUE iter, VALUE position)
{
    gtk_tree_store_move_before(_SELF(self), RVAL2GTKTREEITER(iter), 
                               NIL_P(position) ? NULL : RVAL2GTKTREEITER(position));
    return self;
}

static VALUE
tstore_move_after(VALUE self, VALUE iter, VALUE position)
{
    gtk_tree_store_move_after(_SELF(self), RVAL2GTKTREEITER(iter), 
                               NIL_P(position) ? NULL : RVAL2GTKTREEITER(position));
    return self;
}
#endif

void
Init_gtk_tree_store()
{
    VALUE ts = G_DEF_CLASS(GTK_TYPE_TREE_STORE, "TreeStore", mGtk);

    rbgtk_register_treeiter_set_value_func(GTK_TYPE_TREE_STORE, 
                                           (rbgtkiter_set_value_func)&gtk_tree_store_set_value);

    rb_define_method(ts, "initialize", tstore_initialize, -1);
    rb_define_method(ts, "set_column_types", tstore_set_column_types, -1);
    rb_define_method(ts, "set_value", tstore_set_value, 3);
    rb_define_method(ts, "remove", tstore_remove, 1);
    rb_define_method(ts, "insert", tstore_insert, -1);
    rb_define_method(ts, "insert_before", tstore_insert_before, 2);
    rb_define_method(ts, "insert_after", tstore_insert_after, 2);
    rb_define_method(ts, "prepend", tstore_prepend, 1);
    rb_define_method(ts, "append", tstore_append, 1);
    rb_define_method(ts, "ancestor?", tstore_is_ancestor, 2);
    rb_define_method(ts, "iter_depth", tstore_iter_depth, 1);
    rb_define_method(ts, "clear", tstore_clear, 0);
#if GTK_CHECK_VERSION(2,2,0)
    rb_define_method(ts, "iter_is_valid?", tstore_iter_is_valid, 1);
    rb_define_method(ts, "reorder", tstore_reorder, 2);
    rb_define_method(ts, "swap", tstore_swap, 2);
    rb_define_method(ts, "move_before", tstore_move_before, 2);
    rb_define_method(ts, "move_after", tstore_move_after, 2);
#endif

}


