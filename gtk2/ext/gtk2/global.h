/* -*- c-file-style: "ruby" -*- */
/************************************************

  global.h -

  $Author: sakai $
  $Date: 2003/03/14 03:12:19 $

  Copyright (C) 2002,2003 Ruby-GNOME2 Project Team
  Copyright (C) 1998-2000 Yukihiro Matsumoto,
                          Daisuke Kanda,
                          Hiroshi Igarashi
************************************************/

#ifndef _RBGTK_GLOBAL_H
#define _RBGTK_GLOBAL_H

#include "rbgtk.h"
#ifdef HAVE_RUBY_ST_H
#  include <ruby/st.h>
#else
#  include <st.h>
#endif

#ifndef HAVE_RB_ERRINFO
#  define rb_errinfo() (ruby_errinfo)
#endif

extern ID id_relative_callbacks;
extern ID id_call;
extern VALUE treeiter_set_value_table;

#endif /* _RBGTK_GLOBAL_H */
