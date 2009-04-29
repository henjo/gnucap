/*$Id: m_expression_dump.cc,v 26.81 2008/05/27 05:34:00 al Exp $ -*- C++ -*-
 * Copyright (C) 2003 Albert Davis
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
 * Reconstructs an infix expression from the RPN.
 */
#include "m_expression.h"
/*--------------------------------------------------------------------------*/
void Token::dump(std::ostream& out)const
{
  out << _name << ' ';
}
/*--------------------------------------------------------------------------*/
void Expression::dump(std::ostream& out)const
{
  std::vector<const Token*> locals; // a way of faking garbage collection.
  std::vector<const Token*> stack;  // actually use this
  // The _list is the expression in RPN.
  // Un-parse it -- back to infix.
  for (const_iterator i = begin(); i != end(); ++i) {
    if (dynamic_cast<const Token_STOP*>(*i)) {
      stack.push_back(*i);
    }else if (dynamic_cast<const Token_PARLIST*>(*i)) {
      bool been_here = false;
      std::string tmp(")");
      for (;;) {
	assert(!stack.empty());
	const Token* t = stack.back();
	stack.pop_back();
	if (dynamic_cast<const Token_STOP*>(t)) {
	  tmp = "(" + tmp;
	  break;
	}else if (dynamic_cast<const Token_SYMBOL*>(t)) {
	  if (been_here) {
	    tmp = ", " + tmp;
	  }else{
	    been_here = true;
	  }
	  tmp = t->name() + tmp;
	}else{
	  unreachable();
	}
      }
      Token* t = new Token_PARLIST(tmp);
      locals.push_back(t);
      stack.push_back(t);
    }else if (dynamic_cast<const Token_CONSTANT*>(*i)|| dynamic_cast<const Token_SYMBOL*>(*i)) {
      if (!stack.empty() && (dynamic_cast<const Token_PARLIST*>(stack.back()))) {
	// has parameters (table or function)
	const Token* t1 = stack.back();
	stack.pop_back();
	Token* t = new Token_SYMBOL((**i).name(), t1->full_name());
	locals.push_back(t);
	stack.push_back(t);
      }else{
	// has no parameters (scalar)
	stack.push_back(*i);
      }
    }else if (dynamic_cast<const Token_BINOP*>(*i)) {
      assert(!stack.empty());
      const Token* t2 = stack.back();
      stack.pop_back();
      assert(!stack.empty());
      const Token* t1 = stack.back();
      stack.pop_back();
      std::string tmp('(' + t1->full_name() + ' ' + (**i).name() + ' ' + t2->full_name() + ')');
      Token* t = new Token_SYMBOL(tmp, "");
      locals.push_back(t);
      stack.push_back(t);
    }else if (dynamic_cast<const Token_UNARY*>(*i)) {
      assert(!stack.empty());
      const Token* t1 = stack.back();
      stack.pop_back();
      Token* t = new Token_SYMBOL((**i).name(), t1->name());
      locals.push_back(t);
      stack.push_back(t);
    }else{
      unreachable();
    }
  }
  if (stack.empty()) {
    out << "empty";
  }else{
    out << stack.back()->full_name();
    assert(stack.size() == 1);
  }
  while (!locals.empty()) {
    delete locals.back();
    locals.pop_back();
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
