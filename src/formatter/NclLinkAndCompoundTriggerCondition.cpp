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

#include "ginga.h"
#include "NclLinkAndCompoundTriggerCondition.h"

GINGA_FORMATTER_BEGIN

NclLinkAndCompoundTriggerCondition::NclLinkAndCompoundTriggerCondition ()
    : NclLinkCompoundTriggerCondition ()
{
  typeSet.insert ("NclLinkAndCompoundTriggerCondition");
}

NclLinkAndCompoundTriggerCondition::~NclLinkAndCompoundTriggerCondition ()
{
  vector<NclLinkCondition *>::iterator i;
  NclLinkCondition *l;

  unsatisfiedConditions.clear ();

  i = statements.begin ();
  while (i != statements.end ())
    {
      l = *i;
      if (l != NULL)
        {
          delete l;
          l = NULL;
        }
      ++i;
    }

  statements.clear ();
}

void
NclLinkAndCompoundTriggerCondition::addCondition (
    NclLinkCondition *condition)
{
  if (condition == NULL)
    {
      return;
    }

  if (instanceof (NclLinkTriggerCondition *, condition))
    {
      unsatisfiedConditions.push_back (condition);
      NclLinkCompoundTriggerCondition::addCondition (condition);
    }
  else if (instanceof (NclLinkStatement *, condition))
    {
      statements.push_back (condition);
    }
  else
    {
      clog << "NclLinkAndCompoundTriggerCondition::";
      clog << "addCondition Warning! statements ";
      clog << "trying to add a condition !instanceof(NclLinkStatement)";
      clog << " and !instanceof(NclLinkTriggerCondition)";
      clog << endl;
    }
}

void
NclLinkAndCompoundTriggerCondition::conditionSatisfied (
    NclLinkCondition *condition)
{
  vector<NclLinkCondition *>::iterator i;
  i = unsatisfiedConditions.begin ();
  while (i != unsatisfiedConditions.end ())
    {
      if ((*i) == condition)
        {
          unsatisfiedConditions.erase (i);
          if (unsatisfiedConditions.empty ())
            {
              break;
            }
          else
            {
              i = unsatisfiedConditions.begin ();
            }
        }
      else
        {
          ++i;
        }
    }

  if (unsatisfiedConditions.empty ())
    {
      for (i = conditions.begin (); i != conditions.end (); ++i)
        {
          unsatisfiedConditions.push_back (*i);
        }

      for (i = statements.begin (); i != statements.end (); ++i)
        {
          if (!(((NclLinkStatement *)(*i))->evaluate ()))
            {
              notifyConditionObservers (
                  NclLinkTriggerListener::EVALUATION_ENDED);

              return;
            }
        }

      NclLinkTriggerCondition::conditionSatisfied (condition);
    }
  else
    {
      notifyConditionObservers (NclLinkTriggerListener::EVALUATION_ENDED);
    }
}

vector<NclEvent *>
NclLinkAndCompoundTriggerCondition::getEvents ()
{
  vector<NclEvent *> events = NclLinkCompoundTriggerCondition::getEvents ();
  for (NclLinkCondition *cond : statements)
    {
      for (NclEvent *evt : cond->getEvents ())
        {
          events.push_back (evt);
        }
    }

  return events;
}

GINGA_FORMATTER_END
