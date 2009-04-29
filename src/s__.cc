/*$Id: s__.cc,v 26.105 2008/12/03 07:18:01 al Exp $ -*- C++ -*-
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
 * base class for simulation methods
 */
//testing=trivial 2006.07.17
#include "s__.h"

double	SIM::freq = 0.;
COMPLEX	SIM::jomega = 0.;
double	SIM::time0 = 0.;
double	SIM::time1 = 0.;
double	SIM::_dtmin = 0.;
double	SIM::temp_c_in = 0.;
double	SIM::temp_c = 0.;
double	SIM::last_time = 0.;
double	SIM::damp = 1.0;
bool	SIM::uic = false;
bool	SIM::bypass_ok = false;
bool	SIM::fulldamp = false;
bool	SIM::limiting = false;
bool	SIM::freezetime = false;
int*	SIM::nm = NULL;
double*	SIM::i = NULL;
double*	SIM::v0 = NULL;
double*	SIM::vt1 = NULL;
double*	SIM::fw = NULL;
double*	SIM::vdc = NULL;
COMPLEX*SIM::ac = NULL;
double	SIM::genout = 0.;
std::priority_queue<double, std::vector<double> > SIM::eq;
std::deque<CARD*> SIM::evalq1;
std::deque<CARD*> SIM::evalq2;
std::deque<CARD*> SIM::late_evalq;
std::vector<CARD*> SIM::loadq;
std::vector<CARD*> SIM::acceptq;
std::deque<CARD*>* SIM::evalq    = &SIM::evalq1;
std::deque<CARD*>* SIM::evalq_uc = &SIM::evalq2;
