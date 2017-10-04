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

#include <config.h>
#include <string.h>

#include <cairo.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib/gstdio.h>
#include <glib.h>
#include <SDL2/SDL.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include "ginga.h"
#include "aux-ginga.h"

using namespace ::std;

Display* getDisplay();
void createAndShowWindow (int width, int height,
                          Window* window,
                          cairo_device_t** cairoDevice,
                          cairo_surface_t** windowSurface);

#define deconst(t, x) ((t)(ptrdiff_t)(const void *)(x))
#define gpointerof(p) ((gpointer)((ptrdiff_t)(p)))


// Global formatter.
static Ginga *GINGA = nullptr;

// Options.

#define OPTION_LINE "FILE..."
#define OPTION_DESC                             \
  "Report bugs to: " PACKAGE_BUGREPORT "\n"     \
  "Ginga home page: " PACKAGE_URL

static gboolean opt_debug = FALSE;        // toggle debug
static gboolean opt_experimental = FALSE; // toggle experimental stuff
static gboolean opt_fullscreen = FALSE;   // toggle fullscreen-mode
static string opt_background = "";        // background color
static gint opt_width = 800;              // initial window width
static gint opt_height = 600;             // initial window height

static gboolean
opt_background_cb (const gchar *opt, const gchar *arg,
                   gpointer data, GError **err)
{
  (void) opt;
  (void) data;
  (void) err;

  g_assert_nonnull (arg);
  opt_background = string (arg);
  return TRUE;
}

static gboolean
opt_size_cb (const gchar *opt, const gchar *arg,
             gpointer data, GError **err)
{
  gint64 width;
  gint64 height;
  gchar *end;

  (void) opt;
  (void) arg;
  (void) data;

  width = g_ascii_strtoll (arg, &end, 10);
  if (width == 0)
    goto syntax_error;
  opt_width = (gint) (CLAMP (width, 0, G_MAXINT));

  if (*end != 'x')
    goto syntax_error;

  height = g_ascii_strtoll (++end, NULL, 10);
  if (height == 0)
    goto syntax_error;
  opt_height = (gint) (CLAMP (height, 0, G_MAXINT));

  return TRUE;

 syntax_error:
  g_set_error (err, G_OPTION_ERROR, G_OPTION_ERROR_BAD_VALUE,
               "Invalid size string '%s'", arg);
  return FALSE;
}

static gboolean
opt_version_cb (void)
{
  puts (PACKAGE_STRING);
  exit (EXIT_SUCCESS);
}

static GOptionEntry options[] = {
  {"background", 'b', 0, G_OPTION_ARG_CALLBACK,
   gpointerof (opt_background_cb), "Set background color", "COLOR"},
  {"debug", 'd', 0, G_OPTION_ARG_NONE,
   &opt_debug, "Enable debugging", NULL},
  {"experimental", 'x', 0, G_OPTION_ARG_NONE,
   &opt_experimental, "Enable experimental stuff", NULL},
  {"fullscreen", 'f', 0, G_OPTION_ARG_NONE,
   &opt_fullscreen, "Enable full-screen mode", NULL},
  {"size", 's', 0, G_OPTION_ARG_CALLBACK,
   gpointerof (opt_size_cb), "Set initial window size", "WIDTHxHEIGHT"},
  {"version", 0, G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
   gpointerof (opt_version_cb), "Print version information and exit", NULL},
  {NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL}
};


// Error handling.

#define usage_error(format, ...) _error (TRUE, format, ## __VA_ARGS__)

static G_GNUC_PRINTF (2,3) void
_error (gboolean try_help, const gchar *format, ...)
{
  const gchar *me = g_get_application_name ();
  va_list args;

  va_start (args, format);
  g_fprintf (stderr, "%s: ", me);
  g_vfprintf (stderr, format, args);
  g_fprintf (stderr, "\n");
  va_end (args);

  if (try_help)
    g_fprintf (stderr, "Try '%s --help' for more information.\n", me);
}

void
sendTickEvent ()
{
  guint64 time;
  static guint64 frame = (guint64) -1;
  static guint64 last;
  static guint64 first;

  time = ginga_gettime ();
  frame++;

  if (frame == 0)
    {
      first = time;
      last = time;
    }

  if (!GINGA->sendTickEvent (time - first, time - last, frame))
    {
      g_assert (GINGA->getState () == GINGA_STATE_STOPPED);
      // all done
      return;
    }

  last = time;
}

// Main.

int
main (int argc, char **argv)
{
  int saved_argc;
  char **saved_argv;

  GingaOptions opts;
  GOptionContext *ctx;
  gboolean status;
  GError *error = NULL;

  SDL_Window *window;

  saved_argc = argc;
  saved_argv = g_strdupv (argv);

  // Parse command-line options.
  ctx = g_option_context_new (OPTION_LINE);
  g_assert_nonnull (ctx);
  g_option_context_set_description (ctx, OPTION_DESC);
  g_option_context_add_main_entries (ctx, options, NULL);
  status = g_option_context_parse (ctx, &saved_argc, &saved_argv, &error);
  g_option_context_free (ctx);

  if (!status)
    {
      g_assert_nonnull (error);
      usage_error ("%s", error->message);
      g_error_free (error);
      exit (EXIT_FAILURE);
    }

  if (saved_argc < 2)
    {
      usage_error ("Missing file operand");
      exit (EXIT_FAILURE);
    }

  // Create Ginga state.
  opts.width = opt_width;
  opts.height = opt_height;
  opts.debug = opt_debug;
  opts.experimental = opt_experimental;
  opts.background = string (opt_background);
  GINGA = Ginga::create (argc, argv, &opts);
  g_assert_nonnull (GINGA);

  SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

  // Create an application window with the following settings:
  window = SDL_CreateWindow (
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        opt_width,                         // width, in pixels
        opt_height,                        // height, in pixels
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE     // flags - see below
  );

  if (window == NULL)
    {
      // In the case that the window could not be made...
      printf("Could not create window: %s\n", SDL_GetError());
      return 1;
    }

  SDL_GLContext glcontext = SDL_GL_CreateContext (window);
  SDL_GL_SetSwapInterval (1);
  string errmsg;

  if (!GINGA->start (string (saved_argv[1]), &errmsg))
    {
      g_printerr ("error: ");
      if (saved_argc > 2)
        g_printerr ("%s: ", saved_argv[1]);

      g_printerr ("%s\n", errmsg.c_str ());
    }

  SDL_Event event;
  bool quit = false;
  while (!quit)
    {
      while( SDL_PollEvent( &event ) != 0 )
        {
          // User requests quit
          if( event.type == SDL_QUIT )
            {
              quit = true;
            }
          else if (event.type == SDL_WINDOWEVENT)
            {
              switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                  GINGA->resize (event.window.data1, event.window.data2);
                }
            }
        }

      sendTickEvent ();
      GINGA->redraw_gl ();

      SDL_GL_SwapWindow (window);
      SDL_Delay (33);
    }

  GINGA->stop ();

  // Done.
  delete GINGA;
  g_strfreev (saved_argv);
  SDL_DestroyWindow(window);
  SDL_Quit();

  exit (0);
}