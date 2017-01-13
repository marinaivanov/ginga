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

#include "config.h"
#include "ncl/time/TemporalFlexibilityFunction.h"

namespace br {
namespace pucrio {
namespace telemidia {
namespace ncl {
namespace time {
namespace flexibility {
	TemporalFlexibilityFunction::TemporalFlexibilityFunction(
		    string id,
		    double shrinkingFactor,
		    double stretchingFactor) : Entity(id) {

		setShrinkingFactor(shrinkingFactor);
		setStretchingFactor(stretchingFactor);
		typeSet.insert("TemporalFlexibilityFunction");
	}

	bool TemporalFlexibilityFunction::instanceOf(string s) {
		if (typeSet.empty())
			return false;
		else
			return (typeSet.find(s) != typeSet.end());
	}

	double TemporalFlexibilityFunction::getShrinkingFactor() {
		return shrinkingFactor;
	}

	double TemporalFlexibilityFunction::getStretchingFactor() {
		return stretchingFactor;
	}

	void TemporalFlexibilityFunction::setShrinkingFactor(double factor) {
		if (factor < 0)
			factor = 0;
		else if (factor > 1)
			factor = 1;

		shrinkingFactor = factor;
	}

	void TemporalFlexibilityFunction::setStretchingFactor(double factor) {
		stretchingFactor = factor;
	}
}
}
}
}
}
}
