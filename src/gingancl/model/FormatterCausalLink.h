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

#ifndef _FORMATTERCAUSALLINK_H_
#define _FORMATTERCAUSALLINK_H_

#include "ncl/Link.h"
using namespace ::ginga::ncl;

#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "LinkAction.h"
#include "LinkActionProgressionListener.h"
#include "LinkTriggerCondition.h"
#include "LinkTriggerListener.h"
#include "FormatterLink.h"


BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

  class FormatterCausalLink : public FormatterLink,
	    public LinkTriggerListener,
	    public LinkActionProgressionListener {

	private:
		LinkTriggerCondition* condition;
		LinkAction* action;

	public:
		FormatterCausalLink(
			    LinkTriggerCondition* condition,
			    LinkAction* action,
			    Link* ncmLink,
				void* parentObject);

	    virtual ~FormatterCausalLink();

	    LinkAction* getAction();
	    LinkTriggerCondition* getTriggerCondition();
	    void conditionSatisfied(void *condition);
	    virtual vector<FormatterEvent*>* getEvents();
	    void evaluationStarted();
		void evaluationEnded();
		void actionProcessed(bool start);
  };

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_FORMATTERCAUSALLINK_H_