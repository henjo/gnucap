/*$Id: u_parameter.h,v 26.96 2008/10/09 05:36:27 al Exp $ -*- C++ -*-
 * Copyright (C) 2005 Albert Davis
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
 * A class for parameterized values
 * Used for spice compatible .param statements
 * and passing arguments to models and subcircuits
 */
//testing=script 2006.07.14
#ifndef U_PARAMETER_H
#define U_PARAMETER_H
#include "globals.h"
#include "m_expression.h"
#include "u_opt.h"
#include "e_cardlist.h"
/*--------------------------------------------------------------------------*/
class LANGUAGE;
/*--------------------------------------------------------------------------*/
template <class T>
class PARAMETER {
private:
  mutable T _v;
  std::string _s;
public:
  explicit PARAMETER() :_v(NOT_INPUT), _s() {}
  PARAMETER(const PARAMETER<double>& p) :_v(p._v), _s(p._s) {}
  explicit PARAMETER(T v) :_v(v), _s() {}
  //explicit PARAMETER(T v, const std::string& s) :_v(v), _s(s) {untested();}
  ~PARAMETER() {}
  
  bool	has_hard_value()const {return (_s != "");}
  bool	has_good_value()const {return (_v != NOT_INPUT);}
  //bool has_soft_value()const {untested(); return (has_good_value() && !has_hard_value());}

  operator T()const {return _v;}
  T	e_val(const T& def, const CARD_LIST* scope)const;
  void	parse(CS& cmd);

  std::string string()const {
    if (_s == "#") {
      return to_string(_v);
    }else if (_s == "") {
      return "NA(" + to_string(_v) + ")";
    }else{
      return _s;
    }
  }
  void	print(OMSTREAM& o)const		{o << string();}
  void	set_default(const T& v)		{_v = v; _s = "";}
  void	operator=(const PARAMETER& p)	{_v = p._v; _s = p._s;}
  void	operator=(const T& v)		{_v = v; _s = "#";}
  //void	operator=(const std::string& s)	{untested();_s = s;}

  void	operator=(const std::string& s)	{
    CS cmd(CS::_STRING, s); // conversion
    parse(cmd);
    //assert(!cmd.more()); // can fail when assigning float to int
  }
  bool  operator==(const PARAMETER& p)const {
    return (_v == p._v  &&  _s == p._s);
  }
  bool  operator==(const T& v)const {
    if (v != NOT_INPUT) {
      return _v == v;
    }else{
      return (_v == NOT_INPUT || !has_hard_value());
    }
  }
  //bool	operator!=(const PARAMETER& p)const {untested();
  //  return !(*this == p);
  //}
  //bool	operator!=(const T& v)const {untested();
  //  return !(*this == v);
  //}
  T*	pointer_hack()	 {return &_v;}
private:
  T lookup_solve(const T& def, const CARD_LIST* scope)const;
};
/*--------------------------------------------------------------------------*/
/* non-class interface, so non-paramaters can have same syntax */
/* It is needed by the model compiler */
#if 0
inline bool operator==(const PARAMETER<int>& p, double v)
{untested();
  if (v != NOT_INPUT) {untested();
    return p.operator==(static_cast<int>(v));
  }else{untested();
    return (!(p.has_value()));
  }
}
#endif

template <class T>
bool has_hard_value(const PARAMETER<T>& p)
{
  return p.has_hard_value();
}

#if 0
inline bool has_hard_value(double x)
{untested();
  return (x != NOT_INPUT);
}
inline bool has_good_value(double x)
{untested();
  return (x != NOT_INPUT);
}
#endif

template <class T>
bool has_good_value(const PARAMETER<T>& p)
{
  return p.has_good_value();
}

#if 0
template <class T>
bool has_soft_value(const PARAMETER<T>& p)
{untested();
  return p.has_soft_value();
}
#endif

template <class T>
bool has_nz_value(const T& p)
{
  return (has_good_value(p) && p != 0);
}

template <class T>
void set_default(PARAMETER<T>* p, const T& v)
{
  assert(p);
  p->set_default(v);
}

template <class T>
void set_default(T* p, const T& v)
{
  assert(p);
  *p = v;
}

template <class T>
void e_val(PARAMETER<T>* p, const PARAMETER<T>& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

template <class T>
void e_val(PARAMETER<T>* p, const T& def, const CARD_LIST* scope)
{
  assert(p);
  p->e_val(def, scope);
}

#if 0
template <class T>
void e_val(T* p, const T& def, const CARD_LIST*)
{untested();
  assert(p);
  if (*p == NOT_INPUT) {untested();
    *p = def;
  }else{untested();
  }
}
#endif
/*--------------------------------------------------------------------------*/
class INTERFACE PARAM_LIST {
private:
  std::map<const std::string, PARAMETER<double> > _pl;
public:
  PARAM_LIST* _try_again; // if you don't find it, also look here
public:
  typedef std::map<const std::string, PARAMETER<double> >::const_iterator
		const_iterator;
  typedef std::map<const std::string, PARAMETER<double> >::iterator
		iterator;
  explicit PARAM_LIST() :_try_again(NULL) {}
  explicit PARAM_LIST(const PARAM_LIST& p) 
				:_pl(p._pl), _try_again(p._try_again) {}
  //explicit PARAM_LIST(PARAM_LIST* ta) :_try_again(ta) {untested();}
  ~PARAM_LIST() {}
  void	parse(CS& cmd);
  void	print(OMSTREAM&, LANGUAGE*)const;
  
  size_t size()const {return _pl.size();}
  bool	 is_empty()const {return _pl.empty();}
  bool	 is_printable(int)const;
  std::string name(int)const;
  std::string value(int)const;

  void	eval_copy(PARAM_LIST&, const CARD_LIST*);
  bool  operator==(const PARAM_LIST& p)const {return _pl == p._pl;}
  const PARAMETER<double>& deep_lookup(std::string);
  const PARAMETER<double>& operator[](std::string i) {return deep_lookup(i);}
  void set(std::string, const std::string&);
  void set_try_again(PARAM_LIST* t) {_try_again = t;}

  iterator begin() {return _pl.begin();}
  iterator end() {return _pl.end();}
private:
  mutable int _index;
  mutable const_iterator _previous;
};
/*--------------------------------------------------------------------------*/
template <>
inline double PARAMETER<double>::lookup_solve(const double& def, const CARD_LIST* scope)const
{
  CS cmd(CS::_STRING, _s);
  Expression e(cmd);
  Expression reduced(e, scope->params());
  double v = reduced.eval();
  if (v != NOT_INPUT) {
    return v;
  }else{
    PARAM_LIST* pl = const_cast<PARAM_LIST*>(scope->params());
    return double(pl->deep_lookup(_s).e_val(def, scope));
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline T PARAMETER<T>::lookup_solve(const T& def, const CARD_LIST* scope)const
{untested();
  PARAM_LIST* pl = const_cast<PARAM_LIST*>(scope->params());
  return T(pl->deep_lookup(_s).e_val(def, scope));
}
/*--------------------------------------------------------------------------*/
template <class T>
T PARAMETER<T>::e_val(const T& def, const CARD_LIST* scope)const
{
  assert(scope);

  static int recursion=0;
  static const std::string* first_name = NULL;
  if (recursion == 0) {
    first_name = &_s;
  }else{
  }
  assert(first_name);
  
  ++recursion;
  if (_s == "") {
    // blank string means to use default value
    _v = def;
    if (recursion > 1) {      
      error(bWARNING, "parameter " + *first_name + " has no value\n");
    }else{
    }
  }else if (_s != "#") {
    // anything else means look up the value
    if (recursion <= OPT::recursion) {
      _v = lookup_solve(def, scope);
      if (_v == NOT_INPUT) {itested();
	error(bDANGER, "parameter " + *first_name + " has no value\n");
      }else{
      }
    }else{untested();
      _v = def;
      error(bDANGER, "parameter " + *first_name + " recursion too deep\n");
    }
  }else{
    // start with # means we have a final value
  }
  --recursion;
  return _v;
}
/*--------------------------------------------------------------------------*/
template <>
inline void PARAMETER<bool>::parse(CS& cmd) 
{
  bool new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {untested();
      if (name == "NA") {untested();
	_s = "";
      }else{untested();
	_s = name;
      }
    }else{
    }
  }
  if (!cmd) {
    _v = true;
    _s = "#";
  }else{
  }
}
/*--------------------------------------------------------------------------*/
template <class T>
inline void PARAMETER<T>::parse(CS& cmd) 
{
  T new_val;
  cmd >> new_val;
  if (cmd) {
    _v = new_val;
    _s = "#";
  }else{
    std::string name;
    //cmd >> name;
    name = cmd.ctos(",=();", "'{\"", "'}\"");
    if (cmd) {
      if (name == "NA") {
	_s = "";
      }else{
	_s = name;
      }
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<bool>* val);
INTERFACE bool Get(CS& cmd, const std::string& key, PARAMETER<int>* val);
/*--------------------------------------------------------------------------*/
template <class T>
inline OMSTREAM& operator<<(OMSTREAM& o, const PARAMETER<T> p)
{
  p.print(o);
  return o;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
