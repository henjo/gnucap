/*$Id: globals.h,v 26.113 2009/08/12 03:37:19 al Exp $ -*- C++ -*-
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
 *------------------------------------------------------------------
 * declarations of globals that are exported to plugins
 */
#include "l_dispatcher.h"

class CMD;
class COMMON_COMPONENT;
class MODEL_CARD;
class CARD;
class LANGUAGE;
class FUNCTION;

extern INTERFACE DISPATCHER<CMD> command_dispatcher;
extern INTERFACE DISPATCHER<COMMON_COMPONENT> bm_dispatcher;
extern INTERFACE DISPATCHER<MODEL_CARD> model_dispatcher;
extern INTERFACE DISPATCHER<CARD> device_dispatcher;
extern INTERFACE DISPATCHER<LANGUAGE> language_dispatcher;
extern INTERFACE DISPATCHER<FUNCTION> function_dispatcher;
extern INTERFACE DISPATCHER<FUNCTION> measure_dispatcher;

class LOGIC_NODE;
extern INTERFACE std::string head;
extern LOGIC_NODE* nstat;
