/*$Id: e_card.h,v 26.109 2009/02/02 06:39:10 al Exp $ -*- C++ -*-
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
 * base class for anything in a netlist or circuit file
 */
//testing=script 2007.07.13
#ifndef E_CARD_H
#define E_CARD_H
#include "e_cardlist.h"
#include "u_time_pair.h"
#include "e_base.h"
/*--------------------------------------------------------------------------*/
// this file
class CARD;
/*--------------------------------------------------------------------------*/
// external
class node_t;
class CARD_LIST;
class CS;
class COMMON_COMPONENT;
class PARAM_LIST;
/*--------------------------------------------------------------------------*/
class INTERFACE CARD : public CKT_BASE {
private:
  mutable int	_evaliter;	// model eval iteration number
  CARD_LIST*	_subckt;
  CARD* 	_owner;
protected:
  node_t*	_n;
private:
  bool		_constant;	// eval stays the same every iteration
public:
  int		_net_nodes;	// actual number of "nodes" in the netlist
  //--------------------------------------------------------------------
public:   				// traversal functions
  CARD* find_in_my_scope(const std::string& name);
  const CARD* find_in_parent_scope(const std::string& name, int warn)const;
  const CARD* find_looking_out(const std::string& name, int warn)const;
  const CARD* find_looking_out(CS& cmd, int warn)const;
  //--------------------------------------------------------------------
public:					// virtuals. -- the important stuff
  virtual bool use_obsolete_callback_parse()const {return false;}
  virtual bool use_obsolete_callback_print()const {return false;}
  virtual void print_args_obsolete_callback(OMSTREAM&,LANGUAGE*)const {unreachable();}
  virtual char	 id_letter()const	{unreachable(); return '\0';}
  virtual CARD*	 clone()const		{unreachable(); return NULL;}
  virtual CARD*	 clone_instance()const  {itested(); return clone();}
  virtual void	 expand_first()		{}
  virtual void	 expand()		{}
  virtual void	 expand_last()		{}
  virtual void	 precalc()		{}
  virtual void	 map_nodes()		{}
  virtual void	 tr_iwant_matrix()	{}
  virtual void	 tr_begin()		{}
  virtual void	 tr_restore()		{}
  virtual void	 dc_advance()		{}
  virtual void	 tr_advance()		{}
  virtual void	 tr_regress()		{}
  virtual bool	 tr_needs_eval()const	{return false;}
  virtual void	 tr_queue_eval()	{}
  virtual bool	 do_tr()		{return true;}
  virtual bool	 do_tr_last()		{return true;}
  virtual void	 tr_load()		{}
  virtual TIME_PAIR tr_review()		{return TIME_PAIR(NEVER,NEVER);}
  virtual void	 tr_accept()		{}
  virtual void	 tr_unload()		{untested();}
  virtual void	 ac_iwant_matrix()	{}
  virtual void	 ac_begin()		{}
  virtual void	 do_ac()		{}
  virtual void	 ac_load()		{}
  virtual std::string comment()const {unreachable(); return "";}
  //--------------------------------------------------------------------
  // virtuals defined in base, not overridden here
  // shown here only to complete the list
  //virtual double tr_probe_num(const std::string&)const;
  //virtual XPROBE ac_probe_ext(const std::string&)const;
  //--------------------------------------------------------------------
protected:				// create and destroy.
  explicit CARD();
  explicit CARD(const CARD&);
public:
  virtual  ~CARD()			{delete _subckt;}
  //--------------------------------------------------------------------
public:					// query functions.
  static double	probe(const CARD*,const std::string&);
  int		connects_to(const node_t& node)const;
  virtual CARD_LIST*		scope();
  virtual const CARD_LIST*	scope()const;
  //--------------------------------------------------------------------
public:					// query functions. deferred inline
  bool	evaluated()const;
  //--------------------------------------------------------------------
public:					// query functions. virtual constant
  virtual int	max_nodes()const	{unreachable(); return 0;}
  virtual int	ext_nodes()const	{return max_nodes();}
  virtual int	min_nodes()const	{unreachable(); return 0;}
  virtual int	matrix_nodes()const	{return 0;}
  virtual int	net_nodes()const	{untested();return 0;}
  virtual int	int_nodes()const	{return 0;}
  virtual bool	is_2port()const		{return false;}
  virtual bool	is_source()const	{return false;}
  virtual bool	f_is_value()const	{return false;}
  virtual bool	is_device()const	{return false;}
  virtual bool	makes_own_scope()const  {return false;}
  virtual bool	has_inode()const	{return false;}
  virtual bool	has_iprobe()const	{untested(); return false;}
  //--------------------------------------------------------------------
public:					// query functions.
  CARD_LIST*	     subckt()		{return _subckt;}
  const CARD_LIST*   subckt()const	{return _subckt;}
  CARD*		     owner()		{return _owner;}
  const CARD*	     owner()const	{return _owner;}
  bool		     is_constant()const	{return _constant;}
  bool	node_is_grounded(int i)const;
  virtual bool	     node_is_connected(int i)const;
  //--------------------------------------------------------------------
public:					// modifiers.
  virtual void set_slave()	{untested(); assert(!subckt());}

  void	  set_owner(CARD* o)	{assert(!_owner||_owner==o); _owner=o;}
  void	  set_constant(bool c)	{_constant = c;}

  void	  new_subckt();
  void	  new_subckt(const CARD* model, CARD* owner, const CARD_LIST* scope, PARAM_LIST* p);
  void	  renew_subckt(const CARD* model, CARD* owner, const CARD_LIST* scope, PARAM_LIST* p);
  //--------------------------------------------------------------------
public:	// type
  virtual std::string dev_type()const	{unreachable(); return "";}
  virtual void set_dev_type(const std::string&);
  //--------------------------------------------------------------------
public:	// label -- in BASE
  // non-virtual void set_label(const std::string& s) //BASE
  // non-virtual const std::string& short_label()const //BASE
  /*virtual*/ const std::string long_label()const; // no further override
  //--------------------------------------------------------------------
public:	// ports
  // bool port_exists(int i)const //COMPONENT
  virtual std::string port_name(int)const {unreachable(); return "";}
  // const std::string port_value(int i)const; //COMPONENT
  virtual void set_port_by_name(std::string& name, std::string& value);
  virtual void set_port_by_index(int index, std::string& value);
  void	set_port_to_ground(int index);

  node_t& n_(int i)const;// {untested(); assert(i < matrix_nodes()); return _n[i];}

  // more in COMPONENT
  //--------------------------------------------------------------------
public: // parameters
  virtual std::string value_name()const = 0;

  virtual bool param_exists(int i)const {return param_name(i) != "";}
  virtual bool param_is_printable(int)const;
  virtual std::string param_name(int)const;
  virtual std::string param_name(int,int)const;
  virtual std::string param_value(int)const; 
  virtual void set_param_by_name(std::string, std::string);
  virtual void set_param_by_index(int, std::string&, int);
  virtual int param_count()const {return 0;}
  //--------------------------------------------------------------------
};
INTERFACE CARD_LIST::fat_iterator findbranch(CS&,CARD_LIST::fat_iterator);
/*--------------------------------------------------------------------------*/
inline bool CARD::evaluated()const
{
  if (_evaliter == iteration_tag()) {
    return true;
  }else{
    _evaliter = iteration_tag();
    return false;
  }
}
/*--------------------------------------------------------------------------*/
inline CARD_LIST::fat_iterator findbranch(CS& cmd, CARD_LIST* cl)
{
  assert(cl);
  return findbranch(cmd, CARD_LIST::fat_iterator(cl, cl->begin()));
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
