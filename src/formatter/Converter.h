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

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "ncl/Action.h"
#include "ncl/AssessmentStatement.h"
#include "ncl/CausalConnector.h"
#include "ncl/ValueAssessment.h"
#include "ncl/CompoundAction.h"
#include "ncl/CompoundStatement.h"
#include "ncl/CompoundCondition.h"
#include "ncl/ConditionExpression.h"
#include "ncl/AttributeAssessment.h"
#include "ncl/Role.h"
#include "ncl/SimpleAction.h"
#include "ncl/SimpleCondition.h"
#include "ncl/Statement.h"
#include "ncl/TriggerExpression.h"
#include "ncl/EventUtil.h"
#include "ncl/Bind.h"
#include "ncl/CausalLink.h"
#include "ncl/Link.h"
#include "ncl/Node.h"
#include "ncl/InterfacePoint.h"
#include "ncl/Parameter.h"
using namespace ::ginga::ncl;

#include "ExecutionObjectSwitch.h"
#include "NclSwitchEvent.h"

#include "NclAttributionEvent.h"
#include "NclPresentationEvent.h"
#include "NclSelectionEvent.h"
#include "INclEventListener.h"
#include "NclFormatterEvent.h"
#include "NclPresentationEvent.h"

#include "NclFormatterCausalLink.h"
#include "NclFormatterLink.h"
#include "NclLinkAction.h"
#include "NclLinkCompoundAction.h"
#include "NclLinkSimpleAction.h"
#include "NclLinkAssessmentStatement.h"
#include "NclLinkCompoundTriggerCondition.h"

#include "NclCascadingDescriptor.h"

#include "ExecutionObjectContext.h"
#include "ExecutionObjectApplication.h"
#include "ExecutionObject.h"

#include "RuleAdapter.h"

GINGA_FORMATTER_BEGIN

class FormatterScheduler;

class Converter : public INclEventListener
{
public:
  explicit Converter (RuleAdapter *);
  virtual ~Converter ();

  void setHandlingStatus (bool handling);
  ExecutionObject *getObjectFromNodeId (const string &id);

  void setLinkActionListener (INclLinkActionListener *actionListener);

  ExecutionObject *getExecutionObjectFromPerspective (
      NclNodeNesting *perspec, GenericDescriptor *desc);

  set<ExecutionObject *> *getSettingNodeObjects ();

  NclFormatterEvent *getEvent (ExecutionObject *exeObj,
                               InterfacePoint *interfacePoint,
                               int ncmEventType,
                               const string &key);

  ExecutionObject *
  processExecutionObjectSwitch (ExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertContext (NclNodeNesting *contextPerspective,
                                    Port *port);

private:
  static int _dummyCount;
  map<string, ExecutionObject *> _executionObjects;
  set<NclFormatterEvent *> _listening;
  set<ExecutionObject *> _settingObjects;
  INclLinkActionListener *_actionListener;
  RuleAdapter *_ruleAdapter;
  bool _handling;

  void addExecutionObject (ExecutionObject *exeObj,
                           ExecutionObjectContext *parentObj);

  bool removeExecutionObject (ExecutionObject *exeObj);

  NclFormatterCausalLink *
  createCausalLink (CausalLink *ncmLink,
                    ExecutionObjectContext *parentObject);

  ExecutionObjectContext *
  addSameInstance (ExecutionObject *exeObj, ReferNode *referNode);

  ExecutionObjectContext *getParentExecutionObject (
      NclNodeNesting *perspective);

  ExecutionObject *
  createExecutionObject (const string &id, NclNodeNesting *perspective,
                         NclCascadingDescriptor *descriptor);

  void compileExecutionObjectLinks (ExecutionObject *exeObj, Node *dataObj,
                                    ExecutionObjectContext *parentObj);

  void processLink (Link *ncmLink,
                    Node *dataObject,
                    ExecutionObject *exeObj,
                    ExecutionObjectContext *parentObj);

  void setActionListener (NclLinkAction *action);

  void resolveSwitchEvents (ExecutionObjectSwitch *switchObject);

  NclFormatterEvent *insertNode (NclNodeNesting *perspective,
                                 InterfacePoint *interfacePoint,
                                 GenericDescriptor *descriptor);

  void eventStateChanged (NclFormatterEvent *someEvent, short transition,
                          short previousState) override;

  static Descriptor *createDummyDescriptor (Node *node);
  static NclCascadingDescriptor *createDummyCascadingDescriptor (Node *node);

  static NclCascadingDescriptor *
  getCascadingDescriptor (NclNodeNesting *nodePerspective,
                          GenericDescriptor *descriptor);

  static NclCascadingDescriptor *checkCascadingDescriptor (Node *node);
  static NclCascadingDescriptor *checkContextCascadingDescriptor (
      NclNodeNesting *nodePerspective,
      NclCascadingDescriptor *cascadingDescriptor, Node *ncmNode);

  static bool hasDescriptorPropName (const string &name);
  static bool isEmbeddedApp (NodeEntity *dataObject);
  static bool isEmbeddedAppMediaType (const string &mediaType);

  void setImplicitRefAssessment (const string &roleId, CausalLink *ncmLink,
                                 NclFormatterEvent *event);

  NclLinkAction *createAction (Action *actionExpression,
                               CausalLink *ncmLink,
                               ExecutionObjectContext *parentObject);

  NclLinkCondition *
  createCondition (ConditionExpression *ncmExpression, CausalLink *ncmLink,
                   ExecutionObjectContext *parentObject);

  NclLinkCompoundTriggerCondition *createCompoundTriggerCondition (
      short op, GingaTime delay,
      vector<ConditionExpression *> *ncmChildConditions,
      CausalLink *ncmLink, ExecutionObjectContext *parentObject);

  NclLinkCondition *createCondition (
      TriggerExpression *triggerExpression, CausalLink *ncmLink,
      ExecutionObjectContext *parentObject);

  NclLinkAssessmentStatement *createAssessmentStatement (
      AssessmentStatement *assessmentStatement, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclLinkStatement *
  createStatement (Statement *statementExpression, Link *ncmLink,
                   ExecutionObjectContext *parentObject);

  NclLinkAttributeAssessment *createAttributeAssessment (
      AttributeAssessment *attributeAssessment, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclLinkSimpleAction *
  createSimpleAction (SimpleAction *sae, Bind *bind, Link *ncmLink,
                      ExecutionObjectContext *parentObject);

  NclLinkCompoundAction *createCompoundAction (
      short op, GingaTime delay, vector<Action *> *ncmChildActions,
      CausalLink *ncmLink, ExecutionObjectContext *parentObject);

  NclLinkTriggerCondition *createSimpleCondition (
      SimpleCondition *condition, Bind *bind, Link *ncmLink,
      ExecutionObjectContext *parentObject);

  NclFormatterEvent *createEvent (Bind *bind, Link *ncmLink,
                                  ExecutionObjectContext *parentObject);

  GingaTime getDelayParameter (Link *ncmLink, Parameter *connParam,
                               Bind *ncmBind);

  string getBindKey (Link *ncmLink, Bind *ncmBind);

  GingaTime
  compileDelay (Link *ncmLink, const string &delayObject,Bind *bind);
};

GINGA_FORMATTER_END

#endif /*CONVERTER_H_*/