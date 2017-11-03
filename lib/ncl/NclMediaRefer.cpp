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

#include "aux-ginga.h"
#include "NclMediaRefer.h"

GINGA_BEGIN

NclMediaRefer::NclMediaRefer (NclDocument *ncl, const string &id)
  : NclNode (ncl, id)
{
  _referred = nullptr;
}

NclMedia *
NclMediaRefer::getReferred ()
{
  return _referred;
}

void
NclMediaRefer::initReferred (NclMedia *media)
{
  g_assert_nonnull (media);
  g_assert (_referred == nullptr || _referred == media);
  _referred = media;
  media->addSameInstance (this);
}

GINGA_END
