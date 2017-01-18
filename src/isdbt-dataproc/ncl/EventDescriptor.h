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

#ifndef _EventDescriptor_H_
#define _EventDescriptor_H_


BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_BEGIN

	class EventDescriptor {
		public:
			static string getEventId(string event);
			static uint64_t getEventNPT(string event);
			static string getCommandTag(string event);
			static int getSequenceNumber(string event);
			static bool getFinalFlag(string event);
			static string getPrivateDataPayload(string event);
			static bool checkFCS(string event);

			static string extractMarks(string eventParam);
	};

BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_END
#endif //_EventDescriptor_H_