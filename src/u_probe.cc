/*$Id: u_probe.cc,v 26.99 2008/11/13 17:55:40 al Exp $ -*- C++ -*-
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
 * general probe object
 */
//testing=none
#include "s__.h"
#include "u_probe.h"
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const std::string& what,const CKT_BASE *brh)
  :_what(what),
   _brh(brh),
   _lo(0.),
   _hi(0.)
{
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
PROBE::PROBE(const PROBE& p)
  :_what(p._what),
   _brh(p._brh),
   _lo(p._lo),
   _hi(p._hi)
{
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
}
/*--------------------------------------------------------------------------*/
/* operator=  ...  assignment
 * copy a probe
 */
PROBE& PROBE::operator=(const PROBE& p)
{
  detach();
  _what = p._what;
  _brh  = p._brh;
  _lo   = p._lo;
  _hi   = p._hi;
  if (_brh) {
    _brh->inc_probes();
  }else{
  }
  return *this;
}
/*--------------------------------------------------------------------------*/
/* "detach" a probe from a device
 * which means ...  1. tell the device that the probe has been removed
 *		    2. blank out the probe, so it doesn't reference anything
 * does not remove the probe from the list
 */
void PROBE::detach()
{
  if (_brh) {
    _brh->dec_probes();
  }else{
  }
  _what = "";
  _brh = 0;
}
/*--------------------------------------------------------------------------*/
/* label: returns a string corresponding to a possible probe point
 * (suitable for printing)
 * It has nothing to do with whether it was selected or not
 */
const std::string PROBE::label(void)const
{
  if (_brh) {
    return _what + '(' + _brh->long_label() + ')';
  }else{
    return _what + "(0)";
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::value(void)const
{
  if (_brh) {
    return _brh->probe_num(_what);
  }else{
    return probe_node();
  }
}
/*--------------------------------------------------------------------------*/
double PROBE::probe_node(void)const
{
  if (Umatch(_what, "iter ")) {
    assert(iPRINTSTEP - sCOUNT == 0);
    assert(iSTEP      - sCOUNT == 1);
    assert(iTOTAL     - sCOUNT == 2);
    assert(iCOUNT     - sCOUNT == 3);
    return ::status.iter[sCOUNT];
  }else if (Umatch(_what, "bypass ")) {
    return OPT::bypass + 10*SIM::bypass_ok;
  }else if (Umatch(_what, "control ")) {
    return ::status.control;
  }else if (Umatch(_what, "damp ")) {
    return SIM::damp;
  }else if (Umatch(_what, "gen{erator} ")) {
    return SIM::genout;
  }else if (Umatch(_what, "hidden ")) {
    return ::status.hidden_steps;
  }else if (Umatch(_what, "temp{erature} ")) {
    return SIM::temp_c;
  }else if (Umatch(_what, "time ")) {
    return SIM::time0;
  }else{
    return NOT_VALID;
  }
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
