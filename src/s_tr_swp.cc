/*$Id: s_tr_swp.cc,v 26.121 2009/09/22 20:30:18 al Exp $ -*- C++ -*-
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
 * sweep time and simulate.  output results.
 * manage event queue
 */
//testing=script 2007.11.22
#include "declare.h"	/* gen */
#include "s_tr.h"
/*--------------------------------------------------------------------------*/
//	void	TRANSIENT::sweep(void);
//	void	TRANSIENT::first(void);
//	bool	TRANSIENT::next(void);
//	void	TRANSIENT::accept(void);
//	void	TRANSIENT::reject(void);
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
namespace TR {
  static std::string step_cause[] = {
    "impossible",
    "user requested",
    "event queue",
    "command line \"skip\"",
    "convergence failure, reducing (itl4)",
    "slow convergence, holding (itl3)",
    "truncation error",
    "ambiguous event",
    "limit growth",
    "initial guess"
  };
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::sweep()
{
  _phase = p_INIT_DC;
  head(_tstart, _tstop, "Time");
  bypass_ok = false;
  set_inc_mode_bad();
  
  if (_cont) {  // use the data from last time
    _phase = p_RESTORE;
    restore_voltages();
    CARD_LIST::card_list.tr_restore();
  }else{
    clear_limit();
    CARD_LIST::card_list.tr_begin();
  }
  
  first();
  genout = gen();
  
  if (uic_now()) {
    advance_time();
    zero_voltages();
    CARD_LIST::card_list.do_tr();    //evaluate_models
    while (!late_evalq.empty()) {untested(); //BUG// encapsulation violation
      late_evalq.front()->do_tr_last();
      late_evalq.pop_front();
    }
    _converged = true;
  }else{
    _converged = solve_with_homotopy(OPT::DCBIAS,_trace);
    if (!_converged) {
      error(bWARNING, "did not converge\n");
    }else{
    }
  }
  review(); 
  _accepted = true;
  accept();
  
  {
    bool printnow = (time0 == _tstart || _trace >= tALLTIME);
    if (printnow) {
      keep_voltages();
      outdata(time0);
    }else{untested();
    }
  }
  
  while (next()) {
    bypass_ok = false;
    _phase = p_TRAN;
    genout = gen();
    _converged = solve(OPT::TRHIGH,_trace);

    _accepted = _converged && review();

    if (_accepted) {
      assert(_converged);
      assert(time0 <= _time_by_user_request);
      accept();
      if (step_cause() == scUSER) {
	assert(up_order(time0-_dtmin, _time_by_user_request, time0+_dtmin));
	++_stepno;
	_time_by_user_request += _tstep;	/* advance user time */
      }else{
      }
      assert(time0 < _time_by_user_request);
    }else{
      reject();
      assert(time1 < _time_by_user_request);
    }
    {
      bool printnow =
	(_trace >= tREJECTED)
	|| (_accepted && ((_trace >= tALLTIME) 
			  || (step_cause() == scUSER && time0+_dtmin > _tstart)));
      if (printnow) {
	keep_voltages();
	outdata(time0);
      }else{
      }
    }
    
    if (!_converged && OPT::quitconvfail) {untested();
      outdata(time0);
      throw Exception("convergence failure, giving up");
    }else{
    }
  }
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::set_step_cause(STEP_CAUSE C)
{
  switch (C) {
  case scITER_A:untested();
  case scADT:untested();
  case scITER_R:
  case scINITIAL:
  case scSKIP:
  case scTE:
  case scAMBEVENT:
  case scEVENTQ:
  case scUSER:
    ::status.control = C;
    break;
  case scNO_ADVANCE:untested();
  case scZERO:untested();
  case scSMALL:itested();
  case scREJECT:
    ::status.control += C;
    break;
  }
}
/*--------------------------------------------------------------------------*/
int TRANSIENT::step_cause()const
{
  return ::status.control;
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::first()
{
  /* usually, time0, time1 == 0, from setup */
  assert(time0 == time1);
  assert(time0 <= _tstart);
  ::status.review.start();
  _time_by_user_request = time0 + _tstep; /* set next user step */
  //eq.Clear();				/* empty the queue */
  while (!eq.empty()) {
    eq.pop();
  }
  _stepno = 0;
  set_step_cause(scUSER);
  ++::status.hidden_steps;
  ::status.review.stop();
}
/*--------------------------------------------------------------------------*/
#define check_consistency() {						\
    trace4("", __LINE__, newtime, almost_fixed_time, fixed_time);	\
    assert(almost_fixed_time <= fixed_time);				\
    assert(newtime <= fixed_time);					\
    /*assert(newtime == fixed_time || newtime <= fixed_time -_dtmin);*/	\
    assert(newtime <= almost_fixed_time);				\
    /*assert(newtime == almost_fixed_time || newtime <= almost_fixed_time - _dtmin);*/ \
    assert(newtime > time1);						\
    assert(newtime > reftime);						\
    assert(new_dt > 0.);						\
    assert(new_dt >= _dtmin);						\
    assert(newtime <= _time_by_user_request);				\
    /*assert(newtime == _time_by_user_request*/				\
    /*	   || newtime < _time_by_user_request - _dtmin);	*/	\
  }
#define check_consistency2() {						\
    assert(newtime > time1);						\
    assert(new_dt > 0.);						\
    assert(new_dt >= _dtmin);						\
    assert(newtime <= _time_by_user_request);				\
    /*assert(newtime == _time_by_user_request	*/			\
    /*	   || newtime < _time_by_user_request - _dtmin);*/		\
  }
/*--------------------------------------------------------------------------*/
/* next: go to next time step
 * Set time0 to the next time step, store the old one in time1.
 * Try several methods.  Take the one that gives the shortest step.
 */
bool TRANSIENT::next()
{
  ::status.review.start();

  double old_dt = time0 - time1;
  assert(old_dt >= 0);
  
  double newtime = NEVER;
  double new_dt = NEVER;
  STEP_CAUSE new_control = scNO_ADVANCE;

  if (time0 == time1) {
    // initial step -- could be either t==0 or continue
    // for the first time, just guess
    // make it 100x smaller than expected
    new_dt = std::max(_dtmax/100., _dtmin);
    newtime = time0 + new_dt;
    new_control = scINITIAL;
  }else if (!_converged) {
    new_dt = old_dt / OPT::trstepshrink;
    newtime = _time_by_iteration_count = time1 + new_dt;
    new_control = scITER_R;
  }else{
    double reftime;
    if (_accepted) {
      reftime = time0;
      trace0("accepted");
    }else{
      reftime = time1;
      trace0("rejected");
    }
    trace2("", step_cause(), old_dt);
    trace3("", time1, time0, reftime);

    newtime = _time_by_user_request;
    new_dt = newtime - reftime;
    new_control = scUSER;
    double fixed_time = newtime;
    double almost_fixed_time = newtime;
    check_consistency();
    
    // event queue, events that absolutely will happen
    // exact time.  NOT ok to move or omit, even by _dtmin
    // some action is associated with it.
    if (!eq.empty() && eq.top() < newtime) {
      newtime = eq.top();
      new_dt = newtime - reftime;
      if (new_dt < _dtmin) {
	//new_dt = _dtmin;
	//newtime = reftime + new_dt;
      }else{
      }
      new_control = scEVENTQ;
      fixed_time = newtime;
      almost_fixed_time = newtime;
      check_consistency();
    }else{
    }
    
    // device events that may not happen
    // not sure of exact time.  will be rescheduled if wrong.
    // ok to move by _dtmin.  time is not that accurate anyway.
    if (_time_by_ambiguous_event < newtime - _dtmin) {  
      if (_time_by_ambiguous_event < time1 + 2*_dtmin) {untested();
	double mintime = time1 + 2*_dtmin;
	if (newtime - _dtmin < mintime) {untested();
	  newtime = mintime;
	  new_control = scAMBEVENT;
	}else{untested();
	}
      }else{
	newtime = _time_by_ambiguous_event;
	new_control = scAMBEVENT;
      }
      new_dt = newtime - reftime;
      almost_fixed_time = newtime;
      check_consistency();
    }else{
    }
    
    // device error estimates
    if (_time_by_error_estimate < newtime - _dtmin) {
      newtime = _time_by_error_estimate;
      new_dt = newtime - reftime;
      new_control = scTE;
      check_consistency();
    }else{
    }
    
    // skip parameter
    if (new_dt > _dtmax) {
      if (new_dt > _dtmax + _dtmin) {
	new_control = scSKIP;
      }else{
      }
      new_dt = _dtmax;
      newtime = reftime + new_dt;
      check_consistency();
    }else{
    }

    // converged but with more iterations than we like
    if ((new_dt > (old_dt + _dtmin) * OPT::trstephold)
	&& exceeds_iteration_limit(OPT::TRLOW)) {untested();
      assert(_accepted);
      new_dt = old_dt * OPT::trstephold;
      newtime = reftime + new_dt;
      new_control = scITER_A;
      check_consistency();
    }else{
    }

    // limit growth
    if (analysis_is_tran_dynamic() && new_dt > old_dt * OPT::trstepgrow) {untested();
      new_dt = old_dt * OPT::trstepgrow;
      newtime = reftime + new_dt;
      new_control = scADT;
      check_consistency();
    }else{
    }

    // quantize
    if (newtime < almost_fixed_time) {
      assert(new_dt >= 0);
      if (newtime > reftime + old_dt*.8
	  && newtime < reftime + old_dt*1.5
	  && reftime + old_dt <= almost_fixed_time) {
	// new_dt is close enough to old_dt.
	// use old_dt, to avoid a step change.
	new_dt = old_dt;
	newtime = reftime + new_dt;
	if (newtime > almost_fixed_time) {untested();
	  new_control = scAMBEVENT;
	  newtime = almost_fixed_time;
	  new_dt = newtime - reftime;
	}else{
	}
	check_consistency();
      }else{
	// There will be a step change.
	// Try to choose one that we will keep for a while.
	// Choose new_dt to be in integer fraction of target_dt.
	double target_dt = fixed_time - reftime;
	assert(target_dt >= new_dt);
	double steps = 1 + floor((target_dt - _dtmin) / new_dt);
	assert(steps > 0);
	new_dt = target_dt / steps;
	newtime = reftime + new_dt;
	check_consistency();
      }
    }else{
      assert(newtime == almost_fixed_time);
    }

    // trap time step too small
    if (!_accepted && new_dt < _dtmin) {untested();
      new_dt = _dtmin;
      newtime = reftime + new_dt;
      new_control = scSMALL;
      check_consistency();
    }else{
    }

    // if all that makes it close to user_requested, make it official
    if (up_order(newtime-_dtmin, _time_by_user_request, newtime+_dtmin)) {
      //newtime = _time_by_user_request;
      //new_dt = newtime - reftime;
      new_control = scUSER;
      check_consistency();			   
    }else{
    }
    check_consistency();
    assert(!_accepted || newtime > time0);
    assert(_accepted || newtime <= time0);
  }
  
  set_step_cause(new_control);

  /* got it, I think */
  
  /* check to be sure */
  if (newtime < time1 + _dtmin) {itested();
    /* It's really bad. */
    /* Reject the most recent step, back up as much as possible, */
    /* and creep along */
    assert(!_accepted);
    assert(step_cause() < scREJECT);
    assert(step_cause() >= 0);
    error(bDANGER,"non-recoverable " + TR::step_cause[step_cause()] + "\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e  using=%e\n",
	  newtime, time0, time1, time1 + _dtmin);
    newtime = time1 + _dtmin;
    set_step_cause(scSMALL);
    //check_consistency2();
    throw Exception("tried everything, still doesn't work, giving up");
    //}else if (newtime <= time0 - _dtmin) {
  }else if (newtime < time0) {
    /* Reject the most recent step. */
    /* We have faith that it will work with a smaller time step. */
    assert(!_accepted);
    assert(newtime >= time1 + _dtmin);
    error(bLOG, "backwards time step\n");
    error(bLOG, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    set_step_cause(scREJECT);
    mark_inc_mode_bad();
    check_consistency2();
  }else if (newtime < time0 + _dtmin) {untested();
    /* Another evaluation at the same time. */
    /* Keep the most recent step, but creep along. */
    assert(newtime > time0 - _dtmin);
    error(bDANGER, "zero time step\n");
    error(bDANGER, "newtime=%e  rejectedtime=%e  oldtime=%e\n",
	  newtime, time0, time1);
    if (_accepted) {untested();
      time1 = time0;
    }else{untested();
      assert(_converged);
    }
    check_consistency2();
    newtime = time0 + _dtmin;
    if (newtime > _time_by_user_request) {untested();
      newtime = _time_by_user_request;
      set_step_cause(scUSER);
    }else{untested();
    }
    set_step_cause(scZERO);
    check_consistency2();
  }else{
    assert(_accepted);
    assert(newtime >= time0 + _dtmin);
    /* All is OK.  Moving on. */
    /* Keep value of newtime */
    time1 = time0;
    check_consistency2();
  }
  time0 = newtime;
  
  /* advance event queue (maybe) */
  /* We already looked at it.  Dump what's on top if we took it. */
  while (!eq.empty() && eq.top() <= time0) {
    trace1("eq", eq.top());
    eq.pop();
  }
  while (!eq.empty() && eq.top() < time0 + _dtmin) {itested();
    trace1("eq-extra", eq.top());
    eq.pop();
  }
  //BUG// what if it is later rejected?  It's lost!

  check_consistency2();
  ++::status.hidden_steps;
  ++steps_total_;
  ::status.review.stop();
  trace0("next");
  return (time0 <= _tstop + _dtmin);
}
/*--------------------------------------------------------------------------*/
bool TRANSIENT::review()
{
  ::status.review.start();
  count_iterations(iTOTAL);

  TIME_PAIR time_by = CARD_LIST::card_list.tr_review();
  _time_by_error_estimate = time_by._error_estimate;

  // limit minimum time step
  // 2*_dtmin because _time[1] + _dtmin might be == _time[0].
  if (time_by._event < time1 + 2*_dtmin) {
    _time_by_ambiguous_event = time1 + 2*_dtmin;
  }else{
    _time_by_ambiguous_event = time_by._event;
  }
  // force advance when time too close to previous
  if (std::abs(_time_by_ambiguous_event - time0) < 2*_dtmin) {
    _time_by_ambiguous_event = time0 + 2*_dtmin;
  }else{
  }

  if (time_by._error_estimate < time1 + 2*_dtmin) {
    _time_by_error_estimate = time1 + 2*_dtmin;
  }else{
    _time_by_error_estimate = time_by._error_estimate;
  }
  if (std::abs(_time_by_error_estimate - time0) < 1.1*_dtmin) {
    _time_by_error_estimate = time0 + 1.1*_dtmin;
  }else{
  }

  ::status.review.stop();

  return (_time_by_error_estimate > time0  &&  _time_by_ambiguous_event > time0);
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::accept()
{
  ::status.accept.start();
  set_limit();
  if (OPT::traceload) {
    while (!acceptq.empty()) {
      acceptq.back()->tr_accept();
      acceptq.pop_back();
    }
  }else{itested();
    acceptq.clear();
    CARD_LIST::card_list.tr_accept();
  }
  ++steps_accepted_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
void TRANSIENT::reject()
{
  ::status.accept.start();
  acceptq.clear();
  ++steps_rejected_;
  ::status.accept.stop();
}
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
