/*$Id: bm_model.cc,v 26.127 2009/11/09 16:06:11 al Exp $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis
 * Author: Albert Davis <aldavis@gnu.org>
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * behavioral modeling ".model" stub
 * accepts an unknown name for later linking to a .model
 */
//testing=script 2006.07.13
#include "e_model.h"
#include "bm.h"
/*--------------------------------------------------------------------------*/
EVAL_BM_MODEL::EVAL_BM_MODEL(int c)
  :EVAL_BM_ACTION_BASE(c),
   _arglist(),
   _func(0)
{
}
/*--------------------------------------------------------------------------*/
EVAL_BM_MODEL::EVAL_BM_MODEL(const EVAL_BM_MODEL& p)
  :EVAL_BM_ACTION_BASE(p),
   _arglist(p._arglist),
   _func(0)
{
  attach_common(p._func, &_func);
}
/*--------------------------------------------------------------------------*/
bool EVAL_BM_MODEL::operator==(const COMMON_COMPONENT& x)const
{
  const EVAL_BM_MODEL* p = dynamic_cast<const EVAL_BM_MODEL*>(&x);
  bool rv = p
    && _arglist == p->_arglist
    && EVAL_BM_ACTION_BASE::operator==(x);
  if (rv) {
    incomplete();
    untested();
  }
  return rv;
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::parse_common_obsolete_callback(CS& cmd) //used
{
  assert(!_func);
  assert(!has_model());
  parse_modelname(cmd);
  _arglist = cmd.ctos("", "(", ")");
  assert(!_func);
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::print_common_obsolete_callback(OMSTREAM& o, LANGUAGE* lang)const
{
  assert(lang);
  if (_func) {
    _func->print_common_obsolete_callback(o, lang);
  }else{
    o << modelname();
    if (_arglist != "") {untested();
      o << "(" << _arglist << ")";
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void EVAL_BM_MODEL::expand(const COMPONENT* d)
{
  EVAL_BM_ACTION_BASE::expand(d);
  // not sure what kind of model it is yet.
  // see what we find.

  EVAL_BM_ACTION_BASE* c = NULL;
  try {
    attach_model(d);
    assert(has_model());
    c = dynamic_cast<EVAL_BM_ACTION_BASE*>(model()->new_common());
  }catch (Exception& e) {
    error(bTRACE, e.message() + "++++\n");
    assert(!has_model());
    c = new EVAL_BM_VALUE;
  }

  if (!c) {
    throw Exception(d->long_label() + ": model type mismatch");
  }else{
  }
  
  // now we have one
  // link to the real one
  // later, a "deflate" will push it down to proper place.

  c->set_modelname(modelname());
  CS args(CS::_STRING, _arglist); //obsolete_callback
  c->parse_common_obsolete_callback(args); //BUG//callback
  c->expand(d);
  attach_common(c, &_func);
  assert(_func);
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
