/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "Event.h"
#include "Context.h"
#include "Media.h"
#include "Parser.h"

#define PARSE_AND_START(fmt, doc, str)                                     \
  G_STMT_START                                                             \
  {                                                                        \
    string buf = str;                                                      \
    string errmsg;                                                         \
    *fmt = new Formatter (0, nullptr, nullptr);                            \
    g_assert_nonnull (*fmt);                                               \
    if (!(*fmt)->start (buf.c_str (), buf.length (), &errmsg))             \
      {                                                                    \
        g_printerr ("*** Unexpected error: %s", errmsg.c_str ());          \
        g_assert_not_reached ();                                           \
      }                                                                    \
    *doc = (*fmt)->getDocument ();                                         \
    g_assert_nonnull (*doc);                                               \
  }                                                                        \
  G_STMT_END

int
main (void)
{
  // Presentation events ---------------------------------------------------

  // @lambda: PAUSE from state OCCURRING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <media id='m'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
 </body>\n\
</ncl>");

    // Check lambda
    Context *c = cast (Context *, doc->getRoot());
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);

    Media *m = cast (Media *, c->getChildById ("m"));
    g_assert_nonnull (m);

    Event *lambdaMedia = m->getLambda ();
    g_assert_nonnull (lambdaMedia);

    Event *a1 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a1);
    Event *a2 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a2);
    Event *p1 = m->getAttributionEvent ("p1");
    g_assert_nonnull (p1);

    // before START lambda, anchors events an properties
    // events are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::SLEEPING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert_true (lambdaMedia->transition (Event::START));

    // after START lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::OCCURRING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (1, 1, 1);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::OCCURRING);
    g_assert (a1->getState () == Event::OCCURRING);
    g_assert (a2->getState () == Event::OCCURRING);
    g_assert (p1->getState () == Event::SLEEPING);

    // PAUSE is done and return true
    g_assert_true (lambdaMedia->transition (Event::PAUSE));

    // after PAUSE all events are PAUSED
    // anchors are in PAUSED and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::PAUSED);
    g_assert (a1->getState () == Event::PAUSED);
    g_assert (a2->getState () == Event::PAUSED);
    g_assert (p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: PAUSE from state PAUSED.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <media id='m'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
 </body>\n\
</ncl>");

    // Check lambda
    Context *c = cast (Context *, doc->getRoot());
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);

    Media *m = cast (Media *, c->getChildById ("m"));
    g_assert_nonnull (m);

    Event *lambdaMedia = m->getLambda ();
    g_assert_nonnull (lambdaMedia);

    Event *a1 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a1);
    Event *a2 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a2);
    Event *p1 = m->getAttributionEvent ("p1");
    g_assert_nonnull (p1);

    // before START lambda, anchors events an properties
    // events are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::SLEEPING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // START is done and return true
    g_assert_true (lambdaMedia->transition (Event::START));

    // after START lambda is in OCCURRING and
    // anchors are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::OCCURRING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // advance time
    fmt->sendTick (1, 1, 1);

    // when advance time, anchors events go to OCCURRING
    // and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::OCCURRING);
    g_assert (a1->getState () == Event::OCCURRING);
    g_assert (a2->getState () == Event::OCCURRING);
    g_assert (p1->getState () == Event::SLEEPING);

    // PAUSE is done and return true
    g_assert_true (lambdaMedia->transition (Event::PAUSE));

    // after PAUSE all events are PAUSED
    // anchors are in PAUSED and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::PAUSED);
    g_assert (a1->getState () == Event::PAUSED);
    g_assert (a2->getState () == Event::PAUSED);
    g_assert (p1->getState () == Event::SLEEPING);

    // PAUSE is done and return false
    g_assert_false (lambdaMedia->transition (Event::PAUSE));

    // after PAUSE all events are PAUSED
    // anchors are in PAUSED and properties events are SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::PAUSED);
    g_assert (a1->getState () == Event::PAUSED);
    g_assert (a2->getState () == Event::PAUSED);
    g_assert (p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // @lambda: PAUSE from state SLEEPING.
  {
    Formatter *fmt;
    Document *doc;
    PARSE_AND_START (&fmt, &doc, "\
<ncl>\n\
 <body>\n\
  <media id='m'>\n\
   <property name='p1' value='0'/>\n\
   <area id='a1'/>\n\
   <area id='a2'/>\n\
  </media>\n\
 </body>\n\
</ncl>");

    Context *c = cast (Context *, doc->getRoot());
    g_assert_nonnull (c);

    Event *lambda = c->getLambda ();
    g_assert_nonnull (lambda);

    Media *m = cast (Media *, c->getChildById ("m"));
    g_assert_nonnull (m);

    Event *lambdaMedia = m->getLambda ();
    g_assert_nonnull (lambdaMedia);

    Event *a1 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a1);
    Event *a2 = m->getPresentationEvent ("a1");
    g_assert_nonnull (a2);
    Event *p1 = m->getAttributionEvent ("p1");
    g_assert_nonnull (p1);

    // before START lambda, anchors events an properties
    // events are in SLEEPING
    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::SLEEPING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    // PAUSE is not done and return false
    g_assert_false (lambdaMedia->transition (Event::PAUSE));

    g_assert (lambda->getState () == Event::OCCURRING);
    g_assert (lambdaMedia->getState () == Event::SLEEPING);
    g_assert (a1->getState () == Event::SLEEPING);
    g_assert (a2->getState () == Event::SLEEPING);
    g_assert (p1->getState () == Event::SLEEPING);

    delete fmt;
  }

  // Attribution events ----------------------------------------------------

  // Selection events ------------------------------------------------------

  exit (EXIT_SUCCESS);
}
