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

#include "ginga.h"
#include "NclDocumentParser.h"

#include "NclConnectorsParser.h"
#include "NclImportParser.h"
#include "NclTransitionParser.h"
#include "NclPresentationControlParser.h"
#include "NclComponentsParser.h"
#include "NclStructureParser.h"
#include "NclPresentationSpecificationParser.h"
#include "NclLayoutParser.h"
#include "NclInterfacesParser.h"
#include "NclLinkingParser.h"
#include "NclMetainformationParser.h"

GINGA_NCLCONV_BEGIN

NclDocumentParser::NclDocumentParser ()
{
  this->deviceLayout = NULL;
  this->presentationSpecificationParser = NULL;
  this->structureParser = NULL;
  this->componentsParser = NULL;
  this->connectorsParser = NULL;
  this->linkingParser = NULL;
  this->interfacesParser = NULL;
  this->layoutParser = NULL;
  this->presentationControlParser = NULL;
  this->importParser = NULL;
  this->metainformationParser = NULL;
  this->transitionParser = NULL;
}

NclDocumentParser::~NclDocumentParser () {}

void
NclDocumentParser::setDeviceLayout (DeviceLayout *deviceLayout)
{
  this->deviceLayout = deviceLayout;
}

void
NclDocumentParser::setDependencies ()
{
  getConnectorsParser ()->setImportParser (importParser);
  getPresentationControlParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getPresentationControlParser ()->setComponentsParser (componentsParser);
  getPresentationControlParser ()->setInterfacesParser (interfacesParser);
  getPresentationControlParser ()->setImportParser (importParser);
  getComponentsParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getComponentsParser ()->setLinkingParser (linkingParser);
  getComponentsParser ()->setInterfacesParser (interfacesParser);
  getComponentsParser ()->setPresentationControlParser (
      presentationControlParser);

  getStructureParser ()->setPresentationSpecificationParser (
      presentationSpecificationParser);

  getStructureParser ()->setComponentsParser (componentsParser);
  getStructureParser ()->setLinkingParser (linkingParser);
  getStructureParser ()->setLayoutParser (layoutParser);
  getStructureParser ()->setInterfacesParser (interfacesParser);
  getStructureParser ()->setPresentationControlParser (
      presentationControlParser);

  getStructureParser ()->setConnectorsParser (connectorsParser);
  getStructureParser ()->setImportParser (importParser);
  getStructureParser ()->setTransitionParser (transitionParser);
  getStructureParser ()->setMetainformationParser (metainformationParser);
  getPresentationSpecificationParser ()->setImportParser (importParser);
  getPresentationSpecificationParser ()->setPresentationControlParser (
      presentationControlParser);

  getLayoutParser ()->setImportParser (importParser);
  getTransitionParser ()->setImportParser (importParser);
}

NclTransitionParser *
NclDocumentParser::getTransitionParser ()
{
  return transitionParser;
}

NclConnectorsParser *
NclDocumentParser::getConnectorsParser ()
{
  return connectorsParser;
}

void
NclDocumentParser::setConnectorsParser (
    NclConnectorsParser *connectorsParser)
{
  this->connectorsParser = connectorsParser;
}

NclImportParser *
NclDocumentParser::getImportParser ()
{
  return importParser;
}

void
NclDocumentParser::setBaseReuseParser (NclImportParser *importParser)
{
  this->importParser = importParser;
}

NclPresentationControlParser *
NclDocumentParser::getPresentationControlParser ()
{
  return presentationControlParser;
}

void
NclDocumentParser::setPresentationControlParser (
    NclPresentationControlParser *presentationControlParser)
{
  this->presentationControlParser = presentationControlParser;
}

NclComponentsParser *
NclDocumentParser::getComponentsParser ()
{
  return componentsParser;
}

void
NclDocumentParser::setComponentsParser (
    NclComponentsParser *componentsParser)
{
  this->componentsParser = componentsParser;
}

NclStructureParser *
NclDocumentParser::getStructureParser ()
{
  return structureParser;
}

void
NclDocumentParser::setStructureParser (NclStructureParser *structureParser)
{
  this->structureParser = structureParser;
}

NclPresentationSpecificationParser *
NclDocumentParser::getPresentationSpecificationParser ()
{
  return presentationSpecificationParser;
}

void
NclDocumentParser::setPresentationSpecificationParser (
    NclPresentationSpecificationParser *presentationSpecificationParser)
{
  this->presentationSpecificationParser = presentationSpecificationParser;
}

NclLayoutParser *
NclDocumentParser::getLayoutParser ()
{
  return layoutParser;
}

void
NclDocumentParser::setLayoutParser (NclLayoutParser *layoutParser)
{
  this->layoutParser = layoutParser;
}

NclInterfacesParser *
NclDocumentParser::getInterfacesParser ()
{
  return interfacesParser;
}

void
NclDocumentParser::setInterfacesParser (
    NclInterfacesParser *interfacesParser)
{
  this->interfacesParser = interfacesParser;
}

NclMetainformationParser *
NclDocumentParser::getMetainformationParser ()
{
  return metainformationParser;
}

void
NclDocumentParser::setMetainformationParser (
    NclMetainformationParser *metainformationParser)
{
  this->metainformationParser = metainformationParser;
}

NclLinkingParser *
NclDocumentParser::getLinkingParser ()
{
  return linkingParser;
}

void
NclDocumentParser::setLinkingParser (NclLinkingParser *linkingParser)
{
  this->linkingParser = linkingParser;
}

void *
NclDocumentParser::parseRootElement (DOMElement *rootElement)
{
  string tagName;

  tagName = XMLString::transcode (rootElement->getTagName ());
  if (unlikely (tagName != "ncl"))
    syntax_error ("bad root element '%s'", tagName.c_str ());

  return getStructureParser ()->parseNcl (rootElement, NULL);
}


string
NclDocumentParser::getDirName ()
{
  return this->dirname;
}

string
NclDocumentParser::getPath ()
{
  return this->path;
}

NclDocument *
NclDocumentParser::getNclDocument ()
{
  return this->ncl;
}

void
NclDocumentParser::setNclDocument (NclDocument *ncl)
{
  this->ncl = ncl;
}

void
NclDocumentParser::warning (const SAXParseException &e)
{
  const char *file = XMLString::transcode (e.getSystemId ());
  if (file == NULL || strlen (file) <= 0)
    g_warning ("%s", XMLString::transcode (e.getMessage ()));
  else
    g_warning ("%s:%u.%u: %s", file,
               (guint)e.getLineNumber (),
               (guint)e.getColumnNumber (),
               XMLString::transcode (e.getMessage ()));
}

void G_GNUC_NORETURN
NclDocumentParser::error (const SAXParseException &e)
{
  const char *file = XMLString::transcode (e.getSystemId ());
  if (file == NULL || strlen (file) <= 0)
    g_error ("%s", XMLString::transcode (e.getMessage ()));
  else
    g_error ("%s:%u.%u: %s", file,
             (guint)e.getLineNumber (),
             (guint)e.getColumnNumber (),
             XMLString::transcode (e.getMessage ()));
  exit (EXIT_FAILURE);
}

void
NclDocumentParser::fatalError (const SAXParseException &e)
{
  this->error (e);
}

NclDocument *
NclDocumentParser::parse (const string &path)
{
  DOMDocument *dom;
  DOMElement *elt;
  XercesDOMParser *parser;

  this->path = xpathmakeabs (path);
  this->dirname = xpathdirname (path);

  XMLPlatformUtils::Initialize ();
  parser = new XercesDOMParser ();
  g_assert_nonnull (parser);

  parser->setValidationScheme (XercesDOMParser::Val_Auto);
  parser->setDoNamespaces (false);
  parser->setDoSchema (false);
  parser->setErrorHandler (this);
  parser->setCreateEntityReferenceNodes (false);

  LocalFileInputSource src (XMLString::transcode (path.c_str ()));
  try
    {
      parser->parse (src);
    }
  catch (...)
    {
      g_assert_not_reached ();
    }

  dom = parser->getDocument ();
  g_assert_nonnull (dom);

  elt = (DOMElement *) dom->getDocumentElement ();
  g_assert_nonnull (elt);

  this->ncl = (NclDocument *) parseRootElement (elt);
  g_assert_nonnull (this->ncl);

  delete parser;

  // FIXME: Should we call this?
  //XMLPlatformUtils::Terminate ();

  return this->ncl;
}
GINGA_NCLCONV_END
