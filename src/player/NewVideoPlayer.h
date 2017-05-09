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

#ifndef NEW_VIDEO_PLAYER_H
#define NEW_VIDEO_PLAYER_H

#include "ginga.h"

//#include "system/Thread.h"
//using namespace ginga::system;

//#ifdef IPROVIDERLISTENER
//#undef IPROVIDERLISTENER
//#endif

//#include "mb/IContinuousMediaProvider.h"
//#include "mb/SDLWindow.h"
//#include "mb/IProviderListener.h"

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wcast-qual)
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
#include <glib.h>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
GINGA_PRAGMA_DIAG_POP ()

//#include "ginga.h"  
#include "Player.h"
#include "mb/Display.h"
#include "mb/SDLWindow.h"

using namespace ginga::mb;

GINGA_PLAYER_BEGIN

class NewVideoPlayer : public Thread, public Player
{
public:
  NewVideoPlayer (const string &mrl); //works
  virtual ~NewVideoPlayer ();

  static void initializeAudio (int numArgs, char *args[]);
  static void releaseAudio ();

  void getOriginalResolution (int *width, int *height);
  int64_t getVPts ();
  void timeShift (const string &direction);

  guint32 getMediaTime (); //works
  void setMediaTime (guint32 pos);
  void setStopTime (double pos);
  double getStopTime ();
  void setScope (const string &scope, short type = TYPE_PRESENTATION,
                 double begin = -1, double end = -1,
                 double outTransDur = -1);
  bool play (); //works
  void pause (); //works
  void stop (); //works
  void resume (); //works

  virtual string getPropertyValue (const string &name); //works
  virtual void setPropertyValue (const string &name, const string &value); //works

  void addListener (IPlayerListener *listener); //need test
  //void release ();
  string getMrl (); //works
  bool isPlaying (); //works
  bool isRunning (); //works

  //void setAVPid (int aPid, int vPid);

  bool setOutWindow (SDLWindow* windowId); //works
  
 
 private:  

  GINGA_MUTEX_DEFN ();
  GINGA_COND_DEFN (DisplayJob);

  static void eosCB (GstAppSink *, gpointer); //works
  static GstFlowReturn newPrerollCB (GstAppSink *, gpointer);
  static GstFlowReturn newSampleCB (GstAppSink *, gpointer); //works

  static bool displayJobCallbackWrapper (DisplayJob *,
                                         SDL_Renderer *, void *); //works
  bool displayJobCallback (DisplayJob *, SDL_Renderer *); //works

  double soundLevel;

  GstElement *playbin;
  GstElement *bin;
  GstElement *filter;

  GstSample *sample;

  GstStateChangeReturn ret;  
 // GstBus *bus;
 // GstMessage *msg;

  GstAppSinkCallbacks callbacks;

 // bool textureUpdated;

  void createPipeline (); //works
  void eos(); //works
  void run ();

  void printPipelineState (); //works

};

GINGA_PLAYER_END

#endif /* NEW_VIDEO_PLAYER_H */
