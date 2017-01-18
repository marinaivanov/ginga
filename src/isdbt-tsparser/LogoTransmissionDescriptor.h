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

#ifndef LOGOTRANSMISSIONDESCRIPTOR_H_
#define LOGOTRANSMISSIONDESCRIPTOR_H_

//#include "ILogoTransmissionDescriptor.h"

#include "isdbt-tsparser/IMpegDescriptor.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;



BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_BEGIN

    class LogoTransmissionDescriptor : public IMpegDescriptor {
		protected:
			unsigned char logoType;
			unsigned short logoId;
			unsigned short logoVersion;
			unsigned short downloadDataId;
			string logoName;

		public:
			LogoTransmissionDescriptor();
			~LogoTransmissionDescriptor();
			unsigned int getDescriptorLength();
			void setType(unsigned char type);
			unsigned char getType();
			void setLogoId(unsigned short id);
			unsigned short getLogoId();
			void setLogoVersion(unsigned short version);
			unsigned short getLogoVersion();
			void setDownloadDataId(unsigned short id);
			unsigned short getDownloadDataId();
			void setName(string name);
			string getName();
			unsigned int getNameLength();
			unsigned char getDescriptorTag();
			void print();
			size_t process (char* data, size_t pos);
    };

BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_END
#endif /*LOGOTRANSMISSIONDESCRIPTOR_H_*/