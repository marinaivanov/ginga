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

#ifndef I_MEDIA_PROVIDER_H
#define I_MEDIA_PROVIDER_H

#include "ginga.h"

GINGA_MB_BEGIN

class IMediaProvider
{
public:
  enum IMediaProviderType
    {
      AudioProvider,
      FontProvider,
      ImageProvider,
      VideoProvider,
    };

  virtual ~IMediaProvider(){};
  virtual string getLoadSymbol()=0;
  virtual void playOver(GingaSurfaceID surface) = 0;
  IMediaProviderType getType () const { return type; }
  GingaProviderID getId () const { return id; }
  void setId (const GingaProviderID& id) {this->id = id; }

protected:
  GingaProviderID id;
  IMediaProviderType type;
};

GINGA_MB_END

#endif /* I_MEDIA_PROVIDER_H */