/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/**********************************************************************

  rbgobj_boxed.c -

  $Author: sakai $
  $Date: 2007/07/14 13:33:07 $
  created at: Sat Jul 27 16:56:01 JST 2002

  Copyright (C) 2002,2003  Masahiro Sakai

**********************************************************************/

#include "rbgprivate.h"

VALUE rbgobj_cBoxed;

static void
boxed_mark(boxed_holder *holder)
{
    const RGObjClassInfo *cinfo;

    cinfo = GTYPE2CINFO_NO_CREATE(holder->type);
    if (cinfo && cinfo->mark)
        cinfo->mark(holder->boxed);
}

static void
boxed_free(boxed_holder *holder)
{
    const RGObjClassInfo *cinfo;

    cinfo = GTYPE2CINFO_NO_CREATE(holder->type);
    if (cinfo && cinfo->free)
        cinfo->free(holder->boxed);

    if (holder->own && holder->boxed)
        g_boxed_free(holder->type, holder->boxed);

    free(holder);
}

/**********************************************************************/

static VALUE
rbgobj_boxed_s_allocate(VALUE klass)
{
    const RGObjClassInfo* cinfo = rbgobj_lookup_class(klass);
    boxed_holder* holder;
    VALUE result;

    if (cinfo->gtype == G_TYPE_BOXED)
        rb_raise(rb_eTypeError, "abstract class");

    result = Data_Make_Struct(klass, boxed_holder, 
                              boxed_mark, boxed_free, holder);
    holder->type  = cinfo->gtype;
    holder->boxed = NULL;
    holder->own   = FALSE;

    return result;
}

static VALUE
rbgobj_boxed_init(VALUE self)
{
    rb_raise(rb_eTypeError, "can't initialize %s",
             rb_class2name(CLASS_OF(self)));
}

static VALUE
rbgobj_boxed_inspect(VALUE self)
{
    boxed_holder* holder;
    gchar* s;
    VALUE result;

    Data_Get_Struct(self, boxed_holder, holder);

    s = g_strdup_printf("#<%s:%p ptr=%p own=%s>",
                        rb_class2name(CLASS_OF(self)),
                        (void *)self,
                        holder->boxed,
                        holder->own ? "true" : "false");

    result = rb_str_new2(s);
    g_free(s);

    return result;
}

static VALUE
rbgobj_boxed_init_copy(VALUE self, VALUE orig)
{
    boxed_holder* holder1;
    boxed_holder* holder2;

    if (self == orig) return self;

    if (!rb_obj_is_instance_of(orig, rb_obj_class(self))) {
	rb_raise(rb_eTypeError, "wrong argument class");
    }

    Data_Get_Struct(self, boxed_holder, holder1);
    Data_Get_Struct(orig, boxed_holder, holder2);

    holder1->boxed = g_boxed_copy(holder2->type, holder2->boxed);
    holder1->own   = TRUE;

    if (!holder1->boxed)
      rb_raise(rb_eRuntimeError, "g_boxed_copy() failed");

    return self;
}

/* deprecated */
VALUE
rbgobj_boxed_create(VALUE klass)
{
    return rbgobj_boxed_s_allocate(klass);
}

/**********************************************************************/

void
rbgobj_boxed_initialize(VALUE obj, gpointer boxed)
{ 
    boxed_holder* holder;
    Data_Get_Struct(obj, boxed_holder, holder);
    holder->boxed = g_boxed_copy(holder->type, boxed);
    holder->own   = TRUE;
}

gpointer
rbgobj_boxed_get(VALUE obj, GType gtype)
{
    boxed_holder* holder;

    if (NIL_P(obj))
        return NULL;

    if (!RVAL2CBOOL(rb_obj_is_kind_of(obj, GTYPE2CLASS(gtype))))
        rb_raise(rb_eArgError, "invalid argument %s (expect %s)",
                 rb_class2name(CLASS_OF(obj)), 
                 rb_class2name(GTYPE2CLASS(gtype)));

    Data_Get_Struct(obj, boxed_holder, holder);
    if (!holder->boxed)
        rb_raise(rb_eArgError, "uninitialize %s", rb_class2name(CLASS_OF(obj)));

    return holder->boxed;
}

VALUE
rbgobj_make_boxed(gpointer p, GType gtype)
{
    const RGObjClassInfo* cinfo;
    VALUE result;
    boxed_holder* holder;

    if (!p)
        return Qnil;
    
    cinfo = GTYPE2CINFO(gtype);
    result = rbgobj_boxed_s_allocate(cinfo->klass);
    
    Data_Get_Struct(result, boxed_holder, holder);
    
    if (cinfo->flags & RBGOBJ_BOXED_NOT_COPY){
        holder->boxed = p;
        holder->own   = FALSE;
    } else {
        holder->boxed = g_boxed_copy(gtype, p);
        holder->own   = TRUE;
    }

    return result;
}

void
rbgobj_boxed_not_copy_obj(GType gtype)
{
    RGObjClassInfo* cinfo = (RGObjClassInfo*)GTYPE2CINFO(gtype);
    cinfo->flags |= RBGOBJ_BOXED_NOT_COPY;
}

/**********************************************************************/

static VALUE
boxed_to_ruby(const GValue* from)
{
    gpointer boxed = g_value_get_boxed(from);
    if (!boxed)
        return Qnil;
    else
        return rbgobj_make_boxed(boxed, G_VALUE_TYPE(from));
}

static void
boxed_from_ruby(VALUE from, GValue* to)
{
    boxed_holder* holder;

    if (NIL_P(from)) {
        g_value_set_boxed(to, NULL);
        return;
    }

    Data_Get_Struct(from, boxed_holder, holder);

    if (g_type_is_a(holder->type, G_VALUE_TYPE(to)))
        g_value_set_boxed(to, holder->boxed);
    else
        rb_raise(rb_eRuntimeError, "%s is not a subtype of %s",
                 g_type_name(holder->type),
                 g_type_name(G_VALUE_TYPE(to)));
}

/**********************************************************************/

void
Init_gobject_gboxed()
{
    VALUE gBoxed;

    rbgobj_cBoxed = G_DEF_CLASS(G_TYPE_BOXED, "Boxed", mGLib);
    gBoxed = rbgobj_cBoxed;

    rbgobj_register_g2r_func(G_TYPE_BOXED, boxed_to_ruby);
    rbgobj_register_r2g_func(G_TYPE_BOXED, boxed_from_ruby);

    rb_define_alloc_func(gBoxed, (VALUE(*)_((VALUE)))rbgobj_boxed_s_allocate);
    rb_define_singleton_method(gBoxed, "gtype", generic_s_gtype, 0);
    rb_define_method(gBoxed, "gtype", generic_gtype, 0);
    rb_define_method(gBoxed, "initialize", rbgobj_boxed_init, 0);
    rb_define_method(gBoxed, "inspect", rbgobj_boxed_inspect, 0);
    rb_define_method(gBoxed, "initialize_copy", rbgobj_boxed_init_copy, 1);
    rb_define_alias(gBoxed, "copy", "dup");
}
