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
#include "util/Color.h"

#include "SDLImageProvider.h"
#include "SDLWindow.h"
#include "SDLSurface.h"
#include "SDLDeviceScreen.h"

#include "LocalScreenManager.h"
#include "ScreenManagerFactory.h"

#if WITH_LIBRSVG
#include "SDLSvgDecoder.h"
#endif

#if WITH_LIBBPG
#include "SDLBpgDecoder.h"
#endif

GINGA_MB_BEGIN


	bool SDLImageProvider::mutexInit   = false;
	bool SDLImageProvider::initialized = false;
	short SDLImageProvider::imageRefs  = 0;
	pthread_mutex_t SDLImageProvider::pMutex;

	SDLImageProvider::SDLImageProvider(
			GingaScreenID screenId, const char* mrl) {

		type = ImageProvider;

		if (!mutexInit) {
			mutexInit = true;
			Thread::mutexInit(&pMutex, true);
		}

		//Thread::mutexLock(&pMutex);
		//imageRefs++;

		imgUri   = "";
		myScreen = screenId;

		imgUri.assign(mrl);

		//Thread::mutexUnlock(&pMutex);
	}

	SDLImageProvider::~SDLImageProvider() {
		//Thread::mutexLock(&pMutex);
		//imageRefs--;

		//if (imageRefs == 0) {
			// FIXME: Find a better way to do this!
			// IMG_Quit();
			// initialized = false;
		//}

		//Thread::mutexUnlock(&pMutex);
	}

	void SDLImageProvider::playOver(GingaSurfaceID surface) {
		SDL_Surface* renderedSurface;
		SDLWindow* parent;

		Thread::mutexLock(&pMutex);

		if (!initialized) {
			initialized = true;
			if (IMG_Init(0) < 0) {
				clog << "SDLFontProvider::playOver ";
				clog << "Couldn't initialize IMG: " << SDL_GetError();
				clog << endl;
			}
		}

		if (surface != 0 && ScreenManagerFactory::getInstance()->hasSurface(
				myScreen, surface)) {

#if WITH_LIBRSVG
                    if( imgUri.substr(imgUri.find_last_of(".") + 1) == "svg" ||
                        imgUri.substr(imgUri.find_last_of(".") + 1) == "svgz")
                    {
                        int w = 0, h = 0;
                        // We need the region dimensions for SVG. How to do it?

                        SDLSvgDecoder *svgdec = new SDLSvgDecoder(imgUri);
                        SDLDeviceScreen::lockSDL();
                        renderedSurface = svgdec->decode(w, h);
			SDLDeviceScreen::unlockSDL();
                        delete svgdec;
                    } else
#endif
#if WITH_LIBBPG
                    if ( imgUri.substr(imgUri.find_last_of(".") + 1) == "bpg")
                    {
                        SDLBpgDecoder *bpgdec = new SDLBpgDecoder(imgUri);
                        SDLDeviceScreen::lockSDL();
                        renderedSurface = bpgdec->decode();
			SDLDeviceScreen::unlockSDL();
                    } else
#endif
                    {
                        SDLDeviceScreen::lockSDL();
			renderedSurface = IMG_Load(imgUri.c_str());
			SDLDeviceScreen::unlockSDL();
                    }

			if (renderedSurface != NULL) {
				SDLDeviceScreen::addUnderlyingSurface(renderedSurface);
				GingaWindowID parentId = ScreenManagerFactory::getInstance()->
						getSurfaceParentWindow(surface);
				parent = (SDLWindow*)ScreenManagerFactory::getInstance()->
						getIWindowFromId(myScreen, parentId);

				if (parent != NULL) {
					parent->setRenderedSurface(renderedSurface);
				}
				ScreenManagerFactory::getInstance()->setSurfaceContent(
						surface, (void*)renderedSurface);

                        }

		} else {
			clog << "SDLImageProvider::playOver Warning! NULL content";
			clog << endl;
		}

		Thread::mutexUnlock(&pMutex);
	}

	bool SDLImageProvider::releaseAll() {
		return false;
	}

GINGA_MB_END