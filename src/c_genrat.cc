/*$Id: c_genrat.cc,v 26.83 2008/06/05 04:46:59 al Exp $ -*- C++ -*-
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
 * set up generator for transient analysis
 */
//testing=script,sparse 2006.07.16
#include "s__.h"
#include "globals.h"
/*--------------------------------------------------------------------------*/
static double freq = 0;
static double ampl = 1;
static double phaz = 0.;
static double maxv = 1.;
static double minv = 0.;
static double offset = 0.;
static double init = 0.;
static double rise = 1e-12;
static double fall = 1e-12;
static double delay = 0.;
static double width = 0.;
static double period = 0.;
/*--------------------------------------------------------------------------*/
namespace {
/*--------------------------------------------------------------------------*/
class CMD_ : public CMD {
public:
  void do_it(CS& cmd, CARD_LIST*)
  {
    OMSTREAM where = (cmd.more())  ?  OMSTREAM()  :  IO::mstdout;
    unsigned here = cmd.cursor();
    do{
      ONE_OF
	|| ::Get(cmd, "f{requency}",&freq,   mPOSITIVE)
	|| ::Get(cmd, "a{mplitude}",&ampl)
	|| ::Get(cmd, "p{hase}",    &phaz)
	|| ::Get(cmd, "ma{x}",	    &maxv)
	|| ::Get(cmd, "mi{n}",	    &minv)
	|| ::Get(cmd, "o{ffset}",   &offset)
	|| ::Get(cmd, "i{nitial}",  &init)
	|| ::Get(cmd, "r{ise}",	    &rise,   mPOSITIVE)
	|| ::Get(cmd, "f{all}",	    &fall,   mPOSITIVE)
	|| ::Get(cmd, "d{elay}",    &delay,  mPOSITIVE)
	|| ::Get(cmd, "w{idth}",    &width,  mPOSITIVE)
	|| ::Get(cmd, "pe{riod}",   &period, mPOSITIVE)
	;
    }while (cmd.more() && !cmd.stuck(&here));
    cmd.check(bWARNING, "what's this");
    
    where.setfloatwidth(7);
    where <<   "freq="	<< freq;
    where << "  ampl="	<< ampl;
    where << "  phase="	<< phaz;
    where << "  max="	<< maxv;
    where << "  min="	<< minv;
    where << "  offset="<< offset;
    where << "  init="	<< init;
    where << "  rise="	<< rise;
    where << "  fall="	<< fall;
    where << "  delay="	<< delay;
    where << "  width="	<< width;
    where << "  period="<< period;
    where << "\n";
  }
} p;
DISPATCHER<CMD>::INSTALL d(&command_dispatcher, "generator", &p);
}
/*--------------------------------------------------------------------------*/
double gen()
{
  if (SIM::time0 <= delay) {
    return init;
  }
  double loctime = SIM::time0 - delay;
  if (period > 0.) {
    untested();
    loctime = fmod(loctime, period);
  }

  double level;
  if (SIM::time0 <= delay + rise) {			/* initial rise */
    level = (maxv - 0) * (loctime/rise) + 0;
  }else if (loctime <= rise) {				/* rising       */
    untested();
    level = (maxv - minv) * (loctime/rise) + minv;
  }else if (width==0.  ||  (loctime-=rise) <= width) {	/* pulse on     */
    level = maxv;
  }else if ((loctime-=width) <= fall) {			/* falling      */
    untested();
    level = (minv - maxv) * (loctime/fall) + maxv;
  }else{						/* pulse off    */
    untested();
    level = minv;
  }
  level *= (freq == 0.) 
    ? ampl
    : ampl * sin(M_TWO_PI * freq*(SIM::time0-delay) + phaz * DTOR);
  return (SIM::time0 <= delay + rise)
    ? level + (offset - init) * (loctime/rise) + init
    : level + offset;
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
