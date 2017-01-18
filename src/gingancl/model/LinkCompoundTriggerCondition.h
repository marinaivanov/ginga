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

#ifndef _LINKCOMPOUNDTRIGGERCONDITION_H_
#define _LINKCOMPOUNDTRIGGERCONDITION_H_

#include "FormatterEvent.h"
using namespace ::br::pucrio::telemidia::ginga::ncl::model::event;

#include "LinkCondition.h"
#include "LinkTriggerListener.h"
#include "LinkTriggerCondition.h"
#include "LinkTriggerListener.h"


BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN

	class LinkCompoundTriggerCondition :
			public LinkTriggerCondition,
			public LinkTriggerListener {

		protected:
			vector<LinkCondition*> conditions;

		public:
			LinkCompoundTriggerCondition();
			virtual ~LinkCompoundTriggerCondition();
			virtual void addCondition(LinkCondition* condition);
			virtual void conditionSatisfied(void *condition);
			virtual vector<FormatterEvent*>* getEvents();

			void evaluationStarted();
			void evaluationEnded();
	};

BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END
#endif //_LINKCOMPOUNDTRIGGERCONDITION_H_