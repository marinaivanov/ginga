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

#ifndef EXECUTION_OBJECT_CONTEXT_H
#define EXECUTION_OBJECT_CONTEXT_H

#include "ExecutionObject.h"
#include "NclEvent.h"
#include "NclLink.h"

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext: public ExecutionObject
{
public:
  ExecutionObjectContext (GingaInternal *, const string &, Node *);
  virtual ~ExecutionObjectContext ();

  const set<ExecutionObject *> *getChildren ();
  ExecutionObject *getChildById (const string &);
  bool addChild (ExecutionObject *);

  const vector<NclLink *> *getLinks ();
  bool addLink (NclLink *);

  bool exec (NclEvent *, EventState, EventState, EventStateTransition) override;

private:
  Context *_context;
  set<ExecutionObject *> _children;
  vector<NclLink *> _links;
};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_CONTEXT_H
