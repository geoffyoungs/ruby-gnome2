/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/**********************************************************************

  rbglib.h -

  $Author: sakai $
  $Date: 2007/07/04 13:13:20 $

  Copyright (C) 2002-2005  Ruby-GNOME2 Project
  Copyright (C) 2002,2003  Masahiro Sakai

**********************************************************************/

#include <glib.h>
#include "ruby.h"

#ifndef __RBGLIB_H__
#define __RBGLIB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define RBGLIB_MAJOR_VERSION 1
#define RBGLIB_MINOR_VERSION 0
#define RBGLIB_MICRO_VERSION 4

#ifndef RSTRING_PTR
#  define RSTRING_PTR(s) (RSTRING(s)->ptr)
#  define RSTRING_LEN(s) (RSTRING(s)->len)
#endif

#ifndef RARRAY_PTR
#  define RARRAY_PTR(s) (RARRAY(s)->ptr)
#  define RARRAY_LEN(s) (RARRAY(s)->len)
#endif

#ifndef DBL2NUM
#  define DBL2NUM(v)      (rb_float_new(v))
#endif

#if ! GLIB_CHECK_VERSION(2,4,0)
typedef int GPid;
#endif

#define RBG_INSPECT(object) (rbg_rval_inspect(object))

#define RVAL2CSTR(v) (rbg_rval2cstr(&v))
#define RVAL2CSTR_ACCEPT_NIL(v) (rbg_rval2cstr_accept_nil(&v))
#define RVAL2CSTR2(v) (RVAL2CSTR_ACCEPT_NIL(v))
#define CSTR2RVAL(s) (rbg_cstr2rval(s))
#define CSTR2RVAL_LEN(s, l) (rbg_cstr2rval_len(s, l))
#define CSTR2RVAL_ENC(s, e) (rbg_cstr2rval_with_encoding(s, e))
#define CSTR2RVAL_LEN_ENC(s, l, e) (rbg_cstr2rval_len_with_encoding(s, l, e))
#define CSTR2RVAL_FREE(s) (rbg_cstr2rval_free(s))
#define CSTR2RVAL2(s) (CSTR2RVAL_FREE(s))

#define RVAL2CSTRFILENAME(v) (rbg_filename_from_ruby(v))
#define CSTRFILENAME2RVAL(s) (rbg_filename_to_ruby(s))
#define CSTRFILENAME2RVAL_FREE(s) (rbg_filename_to_ruby_free(s))
#define CSTRFILENAMEARRAY2RVAL_FREE(s) (rbg_filename_gslist_to_array_free(s))

#define RVAL2STRS(ary, n) rbg_rval2strv(ary, n)
#define RVAL2STRS_ACCEPT_NIL(ary, n) rbg_rval2strv_accept_nil(ary, n)
#define RVAL2STRV(ary) rbg_rval2strv(ary, NULL)
#define RVAL2STRV_ACCEPT_NIL(ary) rbg_rval2strv_accept_nil(ary, NULL)
#define RVAL2STRS_DUP(ary, n) rbg_rval2strv_dup(ary, n)
#define RVAL2STRS_DUP_ACCEPT_NIL(ary, n) rbg_rval2strv_dup_accept_nil(ary, n)
#define RVAL2STRV_DUP(ary) rbg_rval2strv_dup(ary, NULL)
#define RVAL2STRV_DUP_ACCEPT_NIL(ary) rbg_rval2strv_dup_accept_nil(ary, NULL)

#define STRV2RVAL(strings) rbg_strv2rval(strings)
#define STRV2RVAL_FREE(strings) rbg_strv2rval_free(strings)

#define RVAL2GBOOLEANS(ary, n) rbg_rval2gbooleans(ary, n)
#define RVAL2GINTS(ary, n) rbg_rval2gints(ary, n)
#define RVAL2GINT8S(ary, n) rbg_rval2gint8s(ary, n)
#define RVAL2GUINT16S(ary, n) rbg_rval2guint16s(ary, n)
#define RVAL2GUINT32S(ary, n) rbg_rval2guint32s(ary, n)
#define RVAL2GDOUBLES(ary, n) rbg_rval2gdoubles(ary, n)

#define CBOOL2RVAL(b)   ((b) ? Qtrue : Qfalse)
#define RVAL2CBOOL(b)   (RTEST(b))
#define GERROR2RVAL(error) (rbgerr_gerror2exception(error))
#define RAISE_GERROR(error) rb_exc_raise(GERROR2RVAL(error))
#define G_DEF_ERROR(domain, name, module, parent, gtype)         \
    rbgerr_define_gerror(domain, name, module, parent, gtype)
#define G_DEF_ERROR2(domain, name, module, parent) \
    rbgerr_define_gerror(domain, name, module, parent, Qnil)

#if defined(G_PLATFORM_WIN32) && !defined(RUBY_GLIB2_STATIC_COMPILATION)
#  ifdef RUBY_GLIB2_COMPILATION
#    define RUBY_GLIB2_VAR __declspec(dllexport)
#  else
#    define RUBY_GLIB2_VAR extern __declspec(dllimport)
#  endif
#else
#  define RUBY_GLIB2_VAR extern
#endif

RUBY_GLIB2_VAR VALUE mGLib;

extern const gchar *rbg_rval_inspect(VALUE object);

extern gchar* rbg_string_value_ptr(volatile VALUE* ptr); /* no longer used */
extern const gchar *rbg_rval2cstr(VALUE *str);
extern const gchar *rbg_rval2cstr_accept_nil(VALUE *str);
extern VALUE rbg_cstr2rval(const gchar* str);
extern VALUE rbg_cstr2rval_len(const gchar* str, gsize len);
extern VALUE rbg_cstr2rval_with_encoding(const gchar* str,
                                         const gchar *encoding);
extern VALUE rbg_cstr2rval_len_with_encoding(const gchar* str, gsize len,
                                             const gchar *encoding);
extern VALUE rbg_cstr2rval_free(gchar *str);
/* just for backward compatibility. */
extern VALUE rbg_cstr2rval_with_free(gchar *str);

VALUE rbg_filename_to_ruby(const gchar *filename);
extern VALUE rbg_filename_to_ruby_free(gchar *filename);
extern gchar *rbg_filename_from_ruby(VALUE filename);
extern VALUE rbg_filename_gslist_to_array_free(GSList *list);

const gchar **rbg_rval2strv(VALUE ary, long *n);
const gchar **rbg_rval2strv_accept_nil(VALUE ary, long *n);
gchar **rbg_rval2strv_dup(VALUE ary, long *n);
gchar **rbg_rval2strv_dup_accept_nil(VALUE ary, long *n);
VALUE rbg_strv2rval(const gchar **strings);
VALUE rbg_strv2rval_free(gchar **strings);

gboolean *rbg_rval2gbooleans(VALUE value, long *n);
gint *rbg_rval2gints(VALUE value, long *n);
gint8 *rbg_rval2gint8s(VALUE value, long *n);
guint16 *rbg_rval2guint16s(VALUE value, long *n);
guint32 *rbg_rval2guint32s(VALUE value, long *n);
gdouble *rbg_rval2gdoubles(VALUE value, long *n);

/* rbgerror.h */
extern VALUE rbgerr_gerror2exception(GError *error);
extern VALUE rbgerr_define_gerror(GQuark domain, const gchar* name, VALUE module, VALUE parent, VALUE gtype);

extern VALUE rbglib_int64_to_num(guint64 val);
extern VALUE rbglib_uint64_to_num(guint64 val);
extern gint64 rbglib_num_to_int64(VALUE val);
extern guint64 rbglib_num_to_uint64(VALUE val);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RBGLIB_H__ */
