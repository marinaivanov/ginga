/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ExecutionObject.h"
#include "ExecutionObjectContext.h"
#include "ExecutionObjectSwitch.h"
#include "ExecutionObjectSettings.h"
#include "FormatterAction.h"
#include "FormatterCondition.h"
#include "FormatterEvent.h"
#include "FormatterLink.h"
#include "ncl/Ncl.h"

class GingaInternal;

GINGA_FORMATTER_BEGIN

class Scheduler
{
public:
  Scheduler (GingaInternal *);
  virtual ~Scheduler ();
  bool run (NclDocument *);

  ExecutionObjectSettings *getSettings ();
  const set<ExecutionObject *> *getObjects ();
  ExecutionObject *getObjectById (const string &);
  ExecutionObject *getObjectByIdOrAlias (const string &);
  bool getObjectPropertyByRef (const string &, string *);
  bool addObject (ExecutionObject *);

  void redraw (cairo_t *);
  void resize (int, int);
  void sendKeyEvent (const string &, bool);
  void sendTickEvent (GingaTime, GingaTime, GingaTime);

  ExecutionObject *obtainExecutionObject (NclNode *);
  bool eval (Predicate *);

private:
  GingaInternal *_ginga;              // ginga handle
  NclDocument *_doc;                  // document tree
  ExecutionObjectSettings *_settings; // settings object
  set<ExecutionObject *> _objects;    // document objects

  FormatterEvent *obtainFormatterEventFromBind (NclBind *);
  FormatterLink *obtainFormatterLink (NclLink *);
};

GINGA_FORMATTER_END

#endif // SCHEDULER_H
