/*$Id: e_card.cc,v 26.109 2009/02/02 06:39:10 al Exp $ -*- C++ -*-
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
 * Base class for "cards" in the circuit description file
 */
//testing=script 2006.07.12
#include "globals.h"
#include "ap.h"
#include "e_node.h"
/*--------------------------------------------------------------------------*/
const int POOLSIZE = 4;
/*--------------------------------------------------------------------------*/
CARD::CARD()
  :CKT_BASE(),
   _evaliter(-100),
   _subckt(0),
   _owner(0),
   _n(0),
   _constant(false),
   _net_nodes(0)
{
}
/*--------------------------------------------------------------------------*/
CARD::CARD(const CARD& p)
  :CKT_BASE(p),
   _evaliter(-100),
   _subckt(0), //BUG// isn't this supposed to copy????
   _owner(0),
   _n(0),
   _constant(p._constant),
   _net_nodes(p._net_nodes)
{
}
/*--------------------------------------------------------------------------*/
const std::string CARD::long_label()const
{
  std::string buffer(short_label());
  for (const CARD* brh = owner(); exists(brh); brh = brh->owner()) {
    //buffer += '.' + brh->short_label();
    buffer = brh->short_label() + '.' + buffer;
  }
  return buffer;
}
/*--------------------------------------------------------------------------*/
/*static*/ double CARD::probe(const CARD *This, const std::string& what)
{
  if (exists(This)) {
    return This->probe_num(what);
  }else{				/* return 0 if doesn't exist */
    return 0.0;				/* happens when optimized models */
  }					/* don't have all parts */
}
/*--------------------------------------------------------------------------*/
/* connects_to: does this part connect to this node?
 * input: a node
 * returns: how many times this part connects to it.
 * does not traverse subcircuits
 */
int CARD::connects_to(const node_t& node)const
{
  untested();
  int count = 0;
  if (is_device()) {
    for (int ii = 0;  ii < net_nodes();  ++ii) {
      untested();
      if (node.n_() == _n[ii].n_()) {
        ++count;
      }
    }
  }
  return count;
}
/*--------------------------------------------------------------------------*/
CARD_LIST* CARD::scope()
{
  if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
const CARD_LIST* CARD::scope()const
{
  if (owner()) {
    return owner()->subckt();	// normal element, owner determines scope
  }else{
    return &(CARD_LIST::card_list);	// root circuit
  }
}
/*--------------------------------------------------------------------------*/
/* find_in_my_scope: find in same scope as myself
 * whatever is found will have the same owner as me.
 * capable of finding me.
 * throws exception, prints error, if can't find.
 */
CARD* CARD::find_in_my_scope(const std::string& name)
{
  assert(scope());

  CARD_LIST::iterator i = scope()->find_(name);
  if (i == scope()->end()) {
    throw Exception_Cant_Find(long_label(), name,
			      ((owner()) ? owner()->long_label() : "(root)"));
  }else{
  }
  return *i;
}
/*--------------------------------------------------------------------------*/
/* find_in_parent_scope: find in parent's scope
 * parent is what my scope is a copy of.
 * capable of finding my parent, who should be just like me.
 * If there is no parent (I'm an original), use my scope.
 * throws exception, prints error, if can't find.
 */
const CARD* CARD::find_in_parent_scope(const std::string& name,
				       int warnlevel)const
{
  assert(name != "");
  const CARD_LIST* p_scope = (scope()->parent()) ? scope()->parent() : scope();
  if (scope()->parent()) {
  }else{
  }

  CARD_LIST::const_iterator i = p_scope->find_(name);
  if (i == p_scope->end()) {
    error(warnlevel, long_label() + ": can't find: " + name + " in " 
	  + long_label() + "\n"); // wrong??
	  //	  + ((owner()) ? owner()->long_label() : "(root)") + "\n");
    return NULL;
  }else{
    return *i;
  }
}
/*--------------------------------------------------------------------------*/
/* find_looking_out: find in my or enclosing scope
 * capable of finding me, or anything back to root.
 * throws exception, prints error, if can't find.
 */
const CARD* CARD::find_looking_out(const std::string& name, int warnlevel)const
{
  const CARD* c = find_in_parent_scope(name, warnlevel);
  if (!c) {
    if (owner()) {
      c = owner()->find_looking_out(name, warnlevel);
    }else if (makes_own_scope()) {
      // probably a subckt or "module"
      CARD_LIST::const_iterator i = CARD_LIST::card_list.find_(name);
      if (i != CARD_LIST::card_list.end()) {itested();
	c = *i;
      }else{
	assert(!c);
      }
    }else{
      assert(!c);
    }
  }else{
    assert(c);
  }
  if (!c) {
    error(warnlevel, "can't find: " + name + " in " + long_label() + "\n");
  }else{
    assert(c);
  }
  return c;
}
/*--------------------------------------------------------------------------*/
const CARD* CARD::find_looking_out(CS& cmd, int warnlevel)const
{
  unsigned here = cmd.cursor();
  std::string name;
  cmd >> name;
  const CARD* c = find_looking_out(name, warnlevel);
  if (!c) {itested();
    cmd.reset(here);
  }else{itested();
  }
  return c;
}
/*--------------------------------------------------------------------------*/
bool CARD::node_is_grounded(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_grounded();
}
/*--------------------------------------------------------------------------*/
bool CARD::node_is_connected(int i)const 
{
  assert(_n);
  assert(i >= 0);
  assert(i < net_nodes());
  return _n[i].is_connected();
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt()
{
  delete _subckt;
  _subckt = new CARD_LIST;
}
/*--------------------------------------------------------------------------*/
void CARD::new_subckt(const CARD* Model, CARD* Owner,
		      const CARD_LIST* Scope, PARAM_LIST* Params)
{
  delete _subckt;
  _subckt = new CARD_LIST(Model, Owner, Scope, Params);
}
/*--------------------------------------------------------------------------*/
void CARD::renew_subckt(const CARD* Model, CARD* Owner,
		      const CARD_LIST* Scope, PARAM_LIST* Params)
{
  if (nstat) {
    assert(subckt());
    subckt()->attach_params(Params, scope());
  }else{
    new_subckt(Model, Owner, Scope, Params);
  }
}
/*--------------------------------------------------------------------------*/
void CARD::set_port_by_name(std::string& int_name, std::string& ext_name)
{itested();
  for (int i=0; i<max_nodes(); ++i) {itested();
    if (int_name == port_name(i)) {itested();
      set_port_by_index(i, ext_name);
      return;
    }
    itested();
  }
  untested();
  throw Exception_No_Match(int_name);
}
/*--------------------------------------------------------------------------*/
void CARD::set_port_by_index(int num, std::string& ext_name)
{
  if (num <= max_nodes()) {
    _n[num].new_node(ext_name, this);
    if (num+1 > _net_nodes) {
      // make the list bigger
      _net_nodes = num+1;
    }else{
      // it's already big enough, probably assigning out of order
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
void CARD::set_port_to_ground(int num)
{itested();
  if (num <= max_nodes()) {itested();
    _n[num].set_to_ground(this);
    if (num+1 > _net_nodes) {
      _net_nodes = num+1;
    }else{
    }
  }else{untested();
    throw Exception_Too_Many(num, max_nodes(), 0/*offset*/);
  }
}
/*--------------------------------------------------------------------------*/
node_t& CARD::n_(int i)const
{
  assert(i < ext_nodes() + int_nodes());
  return _n[i];
}
/*--------------------------------------------------------------------------*/
void CARD::set_param_by_name(std::string Name, std::string Value)
{
  //BUG// ugly linear search
  for (int i = param_count() - 1;  i >= 0;  --i) {
    for (int j = 0;  param_name(i,j) != "";  ++j) { // multiple names
      if (Umatch(Name, param_name(i,j) + ' ')) {
	set_param_by_index(i, Value, 0/*offset*/);
	return; //success
      }else{
	//keep looking
      }
    }
  }
  throw Exception_No_Match(Name);
}
/*--------------------------------------------------------------------------*/
void CARD::set_param_by_index(int i, std::string&, int offset)
{untested();
  throw Exception_Too_Many(i, 0, offset);
}
/*--------------------------------------------------------------------------*/
bool CARD::param_is_printable(int)const
{untested();
  return false;
}
/*--------------------------------------------------------------------------*/
std::string CARD::param_name(int)const
{
  return "";
}
/*--------------------------------------------------------------------------*/
std::string CARD::param_name(int i, int j)const
{
  return (j==0) ? param_name(i) : "";
}
/*--------------------------------------------------------------------------*/
std::string CARD::param_value(int)const
{untested();
  return "";
}
/*--------------------------------------------------------------------------*/
/* set_dev_type: Attempt to change the type of an existing device.
 * Usually, it just throws an exception, unless there is no change.
 * Practical use is to override, so you can set things like NPN vs. PNP.
 */
void CARD::set_dev_type(const std::string& New_Type)
{
  if (!Umatch(New_Type, dev_type() + ' ')) {itested();
    //throw Exception_Cant_Set_Type(dev_type(), New_Type);
  }else{
    // it matches -- ok.
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
