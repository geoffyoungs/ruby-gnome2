/* -*- c-file-style: "ruby"; indent-tabs-mode: nil -*- */
/************************************************

  rbgtkcellrendererpixbuf.c -

  $Author: mutoh $
  $Date: 2003/01/19 14:28:24 $

  Copyright (C) 2002,2003 Masao Mutoh
************************************************/

#include "global.h"


#define _SELF(s) (GTK_CELL_RENDERER_PIXBUF(RVAL2GOBJ(s)))

static VALUE
crpixbuf_initialize(VALUE self)
{
    RBGTK_INITIALIZE(self, gtk_cell_renderer_pixbuf_new());
    return Qnil;
}

void
Init_gtk_cellrendererpixbuf()
{
    VALUE renderer = G_DEF_CLASS(GTK_TYPE_CELL_RENDERER_PIXBUF, "CellRendererPixbuf", mGtk);
    rb_define_method(renderer, "initialize", crpixbuf_initialize, 0);
}


 
