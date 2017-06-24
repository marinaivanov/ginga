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
#include "Scheduler.h"

#include "Converter.h"
#include "Parser.h"

#include "mb/Display.h"

GINGA_FORMATTER_BEGIN

Scheduler::Scheduler ()
{
  this->settings = new Settings ();
  this->ruleAdapter = new RuleAdapter (settings);
  this->compiler = new Converter (this->ruleAdapter);
  this->compiler->setLinkActionListener (this);
  this->focusManager = new FocusManager
    (this, this->settings, this, this->compiler);
  this->focusManager->setKeyHandler (true);
}

Scheduler::~Scheduler ()
{
  for (auto action: this->actions)
    action->setSimpleActionListener (NULL);

  this->actions.clear ();

  ruleAdapter = NULL;
  settings = NULL;

  if (focusManager != NULL)
    {
      delete focusManager;
      focusManager = NULL;
    }

  compiler = NULL;
  events.clear ();
}

void
Scheduler::addAction (NclSimpleAction *action)
{
  actions.insert (action);
}

void
Scheduler::removeAction (NclSimpleAction *action)
{
  set<NclSimpleAction *>::iterator i;

  i = actions.find (action);
  if (i != actions.end ())
    actions.erase (i);
}

void
Scheduler::scheduleAction (NclSimpleAction *action)
{
  runAction (action->getEvent (), action);
  return;
}

void
Scheduler::runAction (NclEvent *event, NclSimpleAction *action)
{
  ExecutionObject *obj;

  if (instanceof (SelectionEvent *, event))
    {
      event->start ();
      delete action;
      return;
    }

  obj = event->getExecutionObject ();
  g_assert_nonnull (obj);

  TRACE ("running action '%s' over event '%s'",
         action->getTypeString ().c_str (),
         event->getId ().c_str ());

  if (instanceof (ExecutionObjectSwitch *, obj)
      && instanceof (SwitchEvent *, event))
    {
      this->runActionOverSwitch ((ExecutionObjectSwitch *) obj,
                                 (SwitchEvent *) event, action);
      return;
    }

  if (instanceof (ExecutionObjectContext *, obj))
    {
      this->runActionOverComposition
        ((ExecutionObjectContext *) obj, action);
      return;
    }

  if (instanceof (AttributionEvent *, event))
    {
      AttributionEvent *attevt;
      NclAssignmentAction *attact;
      PropertyAnchor *property;

      string name;
      string from;
      string to;

      Animation *anim;
      GingaTime dur;

      g_assert (instanceof (NclAssignmentAction *, action));
      g_assert (action->getType () == ACT_START);

      attevt = (AttributionEvent *) event;
      attact = (NclAssignmentAction *) action;

      if (event->getCurrentState () != EventState::SLEEPING)
        return;                 // nothing to do

      property = attevt->getAnchor ();
      g_assert_nonnull (property);

      name = property->getName ();
      from = property->getValue ();
      to = attevt->solveImplicitRefAssessment (attact->getValue ());

      if ((anim = attact->getAnimation ()) != nullptr)
        {
          string s;
          s = attevt->solveImplicitRefAssessment (anim->getDuration ());
          dur = ginga_parse_time (s);
        }
      else
        {
          dur = 0;
        }

      attevt->start ();
      attevt->setValue (to);
      obj->setProperty (name, from, to, dur);
      attevt->stop ();          // FIXME!

      return;
    }

  switch (action->getType ())
    {
    case ACT_START:
      obj->prepare (event);
      g_assert (obj->start ());
      event->addListener (this);
      break;
    case ACT_STOP:
      g_assert (obj->stop ());
      event->removeListener (this);
      break;
    case ACT_PAUSE:
      g_assert (obj->pause ());
      break;
    case ACT_RESUME:
      g_assert (obj->resume ());
      break;
    case ACT_ABORT:
      g_assert (obj->abort ());
      break;
    default:
      g_assert_not_reached ();
    }
}

void
Scheduler::runActionOverComposition (ExecutionObjectContext *compObj,
                                     NclSimpleAction *action)
{
  CompositeNode *compositeNode;
  Port *port;
  NclNodeNesting *compositionPerspective;
  NclNodeNesting *perspective;

  map<string, ExecutionObject *> *objects;
  map<string, ExecutionObject *>::iterator j;
  ExecutionObject *childObject;

  PlayerAdapter *pAdapter;
  AttributionEvent *attrEvent;
  NclEvent *event;
  string propName;
  string propValue;

  vector<Node *> *nestedSeq;

  NclEvent *childEvent;
  size_t i, size = 0;
  vector<NclEvent *> *events;
  EventType eventType = EventType::UNKNOWN;

  if (action->getType () == ACT_START
      || action->getType () == ACT_SET)
    {
      event = action->getEvent ();
      if (event != NULL)
        {
          eventType = event->getType ();
          if (eventType == EventType::UNKNOWN)
            {
              if (instanceof (PresentationEvent *, event))
                {
                  eventType = EventType::PRESENTATION;
                }
              else if (instanceof (AttributionEvent *, event))
                {
                  eventType = EventType::ATTRIBUTION;
                }
              else if (instanceof (SwitchEvent *, event))
                {
                  eventType = EventType::PRESENTATION;
                }
            }
        }

      if (eventType == EventType::ATTRIBUTION)
        {
          event = action->getEvent ();
          if (!instanceof (AttributionEvent *, event))
            return;

          attrEvent = cast (AttributionEvent *, event);
          g_assert_nonnull (attrEvent);

          propName = attrEvent->getAnchor ()->getName ();
          propValue = ((NclAssignmentAction *) action)->getValue ();
          event = compObj->getEventFromAnchorId (propName);

          if (event != NULL)
            {
              event->start ();
              ((AttributionEvent *)event)->setValue (propValue);
              event->stop ();
            }
          else
            {
              attrEvent->stop ();
            }

          objects = compObj->getExecutionObjects ();
          if (objects == NULL)
              return;

          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;
              if (instanceof (ExecutionObjectContext *, childObject))
                {
                  runActionOverComposition (
                      (ExecutionObjectContext *)childObject, action);
                }
              else
                {
                  childEvent = childObject->getEventFromAnchorId (propName);
                  if (childEvent != NULL)
                    { // attribution with transition
                      runAction (childEvent, action);
                    }
                  else
                    { // force attribution
                      pAdapter = childObject->getPlayer ();
                      g_assert_nonnull (pAdapter);
                      pAdapter->setProperty
                        (attrEvent->getAnchor ()->getName (), propValue);
                    }
                }
              ++j;
            }
        }
      else if (eventType == EventType::PRESENTATION)
        {
          compObj->suspendLinkEvaluation (false);

          compositeNode
              = (CompositeNode *)(compObj->getDataObject ()
                                      ->getDataEntity ());

          size = compositeNode->getNumPorts ();

          if (compositeNode->getParentComposition () == NULL)
            {
              compositionPerspective
                  = new NclNodeNesting (compositeNode->getPerspective ());
            }
          else
            {
              compositionPerspective
                  = compObj->getNodePerspective ();
            }

          events = new vector<NclEvent *>;
          for (i = 0; i < size; i++)
            {
              port = compositeNode->getPort ((unsigned int) i);
              g_assert_nonnull (port);

              perspective = compositionPerspective->copy ();
              g_assert_nonnull (perspective);

              nestedSeq = port->getMapNodeNesting ();
              g_assert_nonnull (nestedSeq);

              perspective->append (nestedSeq);
              try
                {
                  childObject = compiler->getExecutionObjectFromPerspective (
                                                  perspective, NULL);

                  if (childObject != NULL
                      && port->getEndInterfacePoint () != NULL
                      && instanceof (ContentAnchor *,
                                     port->getEndInterfacePoint ()))
                    {
                      childEvent
                          = (PresentationEvent
                                 *)(compiler
                                        ->getEvent (
                                            childObject,
                                            port->getEndInterfacePoint (),
                                            EventType::PRESENTATION,
                                            ""));

                      if (childEvent != NULL)
                        {
                          events->push_back (childEvent);
                        }
                    }
                }
              catch (exception *exc)
                {
                  g_assert_not_reached ();
                }

              delete nestedSeq;
              delete perspective;
            }

          delete compositionPerspective;

          size = events->size ();

          for (i = 0; i < size; i++)
            {
              runAction ((*events)[i], action);
            }
          delete events;
          events = NULL;
        }
    }
  else
    {
      event = action->getEvent ();
      if (event != NULL)
        {
          eventType = event->getType ();
        }

      if ((eventType == EventType::PRESENTATION)
          && (action->getType () == ACT_STOP
              || action->getType () == ACT_ABORT))
        {
          if (compObj->getWholeContentPresentationEvent () == event)
            {
              compObj->suspendLinkEvaluation (true);
            }
        }

      events = new vector<NclEvent *>;

      compositeNode = (CompositeNode *)(compObj->getDataObject ()
                                            ->getDataEntity ());

      objects = compObj->getExecutionObjects ();
      if (objects != NULL)
        {
          j = objects->begin ();
          while (j != objects->end ())
            {
              childObject = j->second;

              childEvent = childObject->getMainEvent ();
              if (childEvent == NULL)
                {
                  childEvent
                      = childObject->getWholeContentPresentationEvent ();
                }

              if (childEvent != NULL)
                {
                  events->push_back (childEvent);
                }
              ++j;
            }
          delete objects;
          objects = NULL;
        }

      if (compositeNode->getParentComposition () == NULL)
        {
          compositionPerspective
              = new NclNodeNesting (compositeNode->getPerspective ());

          compObj
              = (ExecutionObjectContext *) (compiler
                    ->getExecutionObjectFromPerspective (
                        compositionPerspective, NULL));

          delete compositionPerspective;

          objects = compObj->getExecutionObjects ();
          if (objects != NULL)
            {
              j = objects->begin ();
              while (j != objects->end ())
                {
                  childObject = j->second;

                  childEvent = childObject->getMainEvent ();
                  if (childEvent == NULL)
                    {
                      childEvent
                          = childObject
                                ->getWholeContentPresentationEvent ();
                    }

                  if (childEvent != NULL)
                    {
                      events->push_back (childEvent);
                    }
                  ++j;
                }
              delete objects;
              objects = NULL;
            }

          // *** QUIT PRESENTATION ***
          Ginga_Display->quit ();
        }

      size = events->size ();
      for (i = 0; i < size; i++)
        {
          runAction ((*events)[i], action);
        }

      delete events;
      events = NULL;
    }
}

void
Scheduler::runActionOverSwitch (ExecutionObjectSwitch *switchObj,
                                SwitchEvent *event,
                                NclSimpleAction *action)
{
  ExecutionObject *selectedObject;
  NclEvent *selectedEvent;

  selectedObject = switchObj->getSelectedObject ();
  if (selectedObject == NULL)
    {
      selectedObject = compiler->processExecutionObjectSwitch (switchObj);

      if (selectedObject == NULL)
        {
          return;
        }
    }

  selectedEvent = event->getMappedEvent ();
  if (selectedEvent != NULL)
    {
      runAction (selectedEvent, action);
    }
  else
    {
      runSwitchEvent (switchObj, event, selectedObject, action);
    }

  if (action->getType () == ACT_STOP
      || action->getType () == ACT_ABORT)
    {
      switchObj->select (NULL);
    }
}

void
Scheduler::runSwitchEvent (ExecutionObjectSwitch *switchObj,
                                    SwitchEvent *switchEvent,
                                    ExecutionObject *selectedObject,
                                    NclSimpleAction *action)
{
  NclEvent *selectedEvent;
  SwitchPort *switchPort;
  vector<Port *> *mappings;
  vector<Port *>::iterator i;
  Port *mapping;
  NclNodeNesting *nodePerspective;
  vector<Node *> *nestedSeq;
  ExecutionObject *endPointObject;

  selectedEvent = NULL;
  switchPort = (SwitchPort *)(switchEvent->getInterfacePoint ());
  mappings = switchPort->getPorts ();
  if (mappings != NULL)
    {
      i = mappings->begin ();
      while (i != mappings->end ())
        {
          mapping = *i;
          if (mapping->getNode () == selectedObject->getDataObject ())
            {
              nodePerspective = switchObj->getNodePerspective ();
              nestedSeq = mapping->getMapNodeNesting ();
              nodePerspective->append (nestedSeq);
              delete nestedSeq;
              try
                {
                  endPointObject
                      = compiler
                            ->getExecutionObjectFromPerspective (
                                  nodePerspective, NULL);

                  if (endPointObject != NULL)
                    {
                      selectedEvent
                          = compiler
                                ->getEvent (
                                    endPointObject,
                                    mapping->getEndInterfacePoint (),
                                    switchEvent->getType (),
                                    switchEvent->getKey ());
                    }
                }
              catch (exception *exc)
                {
                  // continue
                }

              break;
            }
          ++i;
        }
    }

  if (selectedEvent != NULL)
    {
      switchEvent->setMappedEvent (selectedEvent);
      runAction (selectedEvent, action);
    }
}

void
Scheduler::startDocument (const string &file)
{
  string id;
  ContextNode *body;
  vector<Port *> *ports;
  vector<NclEvent *> *entryevts;
  NclNodeNesting *persp;

  // Parse document.
  NclParser compiler;
  this->file = xpathmakeabs (file);
  this->doc = compiler.parse (file);
  g_assert_nonnull (this->doc);

  id = this->doc->getId ();
  body = this->doc->getBody ();
  if (unlikely (body == NULL))
    ERROR_SYNTAX ("document has no body");

  // Get entry events (i.e., those mapped by ports).
  ports = body->getPorts ();
  if (unlikely (ports == NULL))
    ERROR ("document has no ports");

  persp = new NclNodeNesting ();
  persp->insertAnchorNode (body);
  entryevts = new vector<NclEvent *>;
  for (auto port: *ports)
    {
      NclEvent *evt = this->compiler->insertContext (persp, port);
      g_assert_nonnull (evt);
      entryevts->push_back (evt);
    }
  delete persp;

  if (unlikely (entryevts->empty ()))
    {
      WARNING ("document has no ports");
      return;
    }

  // Create execution object for settings node and initialize it.
  vector <Node *> *settings = this->doc->getSettingsNodes ();
  for (auto node: *settings)
    {
      ContentNode *content;
      ExecutionObject *execobj;

      persp = new NclNodeNesting (node->getPerspective ());
      execobj = this->compiler->getExecutionObjectFromPerspective
        (persp, NULL);
      g_assert_nonnull (execobj);

      TRACE ("processing '%s'", persp->getId ().c_str ());
      delete persp;

      content = (ContentNode *) node;
      for (auto anchor: content->getAnchors ())
        {
          PropertyAnchor *prop;
          string name;
          string value;

          if (!instanceof (PropertyAnchor *, anchor))
            continue;           // nothing to do

          prop = cast (PropertyAnchor *, anchor);
          name = prop->getName ();
          value = prop->getValue ();
          if (value == "")
            continue;           // nothing to do

          TRACE ("setting %s='%s'", name.c_str (), value.c_str ());
          //this->settings->set (name, value);
        }
    }
  delete settings;

  // Start entry events.
  for (auto event: *entryevts)
    {
      NclSimpleAction *fakeAction;
      event->addListener (this);
      this->events.push_back (event);
      fakeAction = new NclSimpleAction (event, ACT_START);
      runAction (event, fakeAction);
      delete fakeAction;
    }
  delete entryevts;
}

void
Scheduler::eventStateChanged (
    NclEvent *event,
    EventStateTransition transition,
    arg_unused (EventState previousState))
{
  ExecutionObject *object;
  vector<NclEvent *>::iterator it;
  bool contains;
  bool hasOther;

  hasOther = false;
  contains = false;

  for (auto evt: this->events)
    {
      if (evt == event)
        {
          contains = true;
        }
      else if (evt->getCurrentState () != EventState::SLEEPING)
        {
          hasOther = true;
        }
    }

  if (contains)
    {
      switch (transition)
        {
        case EventStateTransition::STOPS:
        case EventStateTransition::ABORTS:
          if (!hasOther)
            {
              events.clear ();

              // we can't remove the document,
              // since it can be started again
              // removeDocument(event);
            }
          break;
        default:
          break;
        }
    }
  else
    {
      switch (transition)
        {
        case EventStateTransition::STARTS:
          object = event->getExecutionObject ();
          focusManager->showObject (object);
          break;

        case EventStateTransition::STOPS:
          if (((PresentationEvent *)event)->getRepetitions () == 0)
            {
              bool hideObj = true;
              event->removeListener (this);
              object = event->getExecutionObject ();

              if (hideObj)
                {
                  this->focusManager->hideObject (object);
                }
            }
          break;

        case EventStateTransition::ABORTS:
          {
            bool hideObj = true;

            event->removeListener (this);
            object = (ExecutionObject *)(event->getExecutionObject ());

            if (hideObj)
              {
                this->focusManager->hideObject (object);
              }
            break;
          }

        case EventStateTransition::PAUSES:
          {
            break;
          }

        case EventStateTransition::RESUMES:
          {
            break;
          }

        default:
          g_assert_not_reached ();
        }
    }
}

GINGA_FORMATTER_END
