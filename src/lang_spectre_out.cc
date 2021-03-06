/*$Id: lang_spectre_out.cc,v 26.111 2009/06/11 04:20:10 al Exp $ -*- C++ -*-
 * Copyright (C) 2007 Albert Davis
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
 */
#include "d_dot.h"
#include "d_coment.h"
#include "d_subckt.h"
#include "e_model.h"
#include "lang_spectre.h"
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_args(OMSTREAM& o, const CARD* x)
{
  assert(x);
  o << ' ';
  if (x->use_obsolete_callback_print()) {
    x->print_args_obsolete_callback(o, this);  //BUG//callback//
  }else{
    for (int ii = x->param_count() - 1;  ii >= 0;  --ii) {
      if (x->param_is_printable(ii)) {
	std::string arg = " " + x->param_name(ii) + "=" + x->param_value(ii);
	o << arg;
      }else{
      }
    }
  }
}
/*--------------------------------------------------------------------------*/
static void print_type(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << ' ' << x->dev_type();
}
/*--------------------------------------------------------------------------*/
static void print_label(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);
  o << x->short_label();
}
/*--------------------------------------------------------------------------*/
static void print_ports(OMSTREAM& o, const COMPONENT* x)
{
  assert(x);

  o << " (";
  std::string sep = "";
  for (int ii = 0;  x->port_exists(ii);  ++ii) {
    o << sep << x->port_value(ii);
    sep = " ";
  }
  for (int ii = 0;  x->current_port_exists(ii);  ++ii) {
    o << sep << x->current_port_value(ii);
    sep = " ";
  }
  o << ")";
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_paramset(OMSTREAM& o, const MODEL_CARD* x)
{
  assert(x);
  o << "model " << x->short_label() << ' ' << x->dev_type() << ' ';
  print_args(o, x);
  o << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_module(OMSTREAM& o, const MODEL_SUBCKT* x)
{
  assert(x);
  assert(x->subckt());

  o << "subckt " <<  x->short_label();
  print_ports(o, x);
  o << "\n";

  for (CARD_LIST::const_iterator 
	 ci = x->subckt()->begin(); ci != x->subckt()->end(); ++ci) {
    print_item(o, *ci);
  }
  
  o << "ends " << x->short_label() << "\n\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_instance(OMSTREAM& o, const COMPONENT* x)
{
  print_label(o, x);
  print_ports(o, x);
  print_type(o, x);
  print_args(o, x);
  o << "\n";
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_comment(OMSTREAM& o, const DEV_COMMENT* x)
{
  assert(x);
  o << x->comment() << '\n';
}
/*--------------------------------------------------------------------------*/
void LANG_SPECTRE::print_command(OMSTREAM& o, const DEV_DOT* x)
{
  assert(x);
  o << x->s() << '\n';
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
