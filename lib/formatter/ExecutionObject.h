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

#ifndef EXECUTION_OBJECT_H
#define EXECUTION_OBJECT_H

#include "GingaInternal.h"
#include "NclAction.h"
#include "NclEvent.h"

#include "player/Player.h"
using namespace ::ginga::player;

GINGA_FORMATTER_BEGIN

class ExecutionObjectContext;
class ExecutionObjectSettings;

class ExecutionObject
{
public:
  ExecutionObject (GingaInternal *, const string &, Node *);
  virtual ~ExecutionObject ();

  Node *getNode ();
  string getId ();

  const vector <string> *getAliases ();
  bool hasAlias (const string &);
  bool addAlias (const string &);

  ExecutionObjectContext *getParent ();
  void initParent (ExecutionObjectContext *);

  const set<NclEvent *> *getEvents ();
  NclEvent *getEvent (EventType, Anchor *, const string &);
  NclEvent *getEventByAnchorId (EventType type, const string &,
                                const string &);
  NclEvent *obtainEvent (EventType, Anchor *, const string &);
  bool addEvent (NclEvent *);

  NclEvent *getLambda (EventType);
  EventState getLambdaState ();

  bool isFocused ();
  string getProperty (const string &);
  void setProperty (const string &, const string &, GingaTime dur=0);

  bool getZ (int *, int *);
  void redraw (cairo_t *);
  void sendKeyEvent (const string &, bool);
  virtual void sendTickEvent (GingaTime, GingaTime, GingaTime);

  virtual bool exec (NclEvent *, EventState, EventState,
                     EventStateTransition);

protected:
  GingaInternal *_ginga;                         // ginga handle
  Scheduler *_scheduler;                         // scheduler
  Node *_node;                                   // NCL node
  string _id;                                    // object id
  vector<string> _aliases;                       // aliases
  ExecutionObjectContext *_parent;               // parent object
  Player *_player;                               // associated player
  GingaTime _time;                               // playback time
  set<NclEvent *> _events;                       // object events
  vector<pair<NclAction *, GingaTime>> _delayed; // delayed actions
};

GINGA_FORMATTER_END

#endif // EXECUTION_OBJECT_H
