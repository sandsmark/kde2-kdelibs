/*
 *  This file is part of the KDE libraries
 *  Copyright (C) 1999 Harri Porten (porten@kde.org)
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <kjs/kjs.h>
#include <kjs/object.h>
#include <kjs/function.h>
#include <kjs/operations.h>

#include <khtml_part.h>
#include <html_element.h>
#include <html_head.h>
#include <html_inline.h>
#include <html_image.h>
#include <dom_string.h>
#include <dom_exception.h>

#include "kjs_binding.h"
#include "kjs_dom.h"
#include "kjs_html.h"
#include "kjs_text.h"
#include "kjs_window.h"
#include "kjs_navigator.h"

using namespace KJS;

extern "C" {
  // initialize HTML module
  KJSProxy *kjs_html_init(KHTMLPart *khtml)
  {
    KJScript *script = new KJScript();
    script->enableDebug();

    KJS::Global global(Global::current());
    DOM::HTMLDocument doc;
    doc = khtml->htmlDocument();
    global.put("document", KJSO(new KJS::HTMLDocument(doc)));
    global.put("window", KJSO(new KJS::Window(khtml->view())));
    global.put("navigator", KJSO(new Navigator()));
    global.put("Image", KJSO(new ImageObject(global)));

    // this is somewhat ugly. But the only way I found to control the
    // dlopen'ed interpreter (*no* linking!) were callback functions.
    return new KJSProxy(script, &kjs_eval, &kjs_clear,
			&kjs_special, &kjs_destroy);
  }
  // evaluate code
  bool kjs_eval(KJScript *script, const QChar *c, unsigned int len)
  {
    return script->evaluate(c, len);
  }
  // clear resources allocated by the interpreter
  void kjs_clear(KJScript *script)
  {
    script->clear();
  }
  // for later extensions.
  const char *kjs_special(KJScript *, const char *)
  {
    // return something like a version number for now
    return "1";
  }
  void kjs_destroy(KJScript *script)
  {
    delete script;
  }
};

KJSO DOMObject::get(const UString &p) const
{
  KJSO result;
  try {
    result = tryGet(p);
  }
  catch (DOM::DOMException e) {
    result = Undefined();
  }
  return result;
}

void DOMObject::put(const UString &p, const KJSO& v)
{
  try {
    tryPut(p,v);
  }
  catch (DOM::DOMException e) {
  }
}

KJSO DOMFunction::get(const UString &p) const
{
  KJSO result;
  try {
    result = tryGet(p);
  }
  catch (DOM::DOMException e) {
    result = Undefined();
  }
  return result;
}

Completion DOMFunction::execute(const List &args)
{
  Completion completion;
  try {
    completion = tryExecute(args);
  }
  catch (DOM::DOMException e) {
    completion = Completion(Normal,Undefined());
  }
  return completion;
}


UString::UString(const DOM::DOMString &d)
{
  unsigned int len = d.length();
  UChar *dat = new UChar[len];
  memcpy(dat, d.unicode(), len * sizeof(UChar));
  rep = UString::Rep::create(dat, len);
}

DOM::DOMString UString::string() const
{
  return DOM::DOMString((QChar*) data(), size());
}

QString UString::qstring() const
{
  return QString((QChar*) data(), size());
}

QConstString UString::qconststring() const
{
  return QConstString((QChar*) data(), size());
}

KJSO NodeObject::toPrimitive(Type preferred) const
{
  if (preferred == HostType) {
    // return a unique handle for comparisons
    DOM::Node n = toNode();
    if (n.isNull())
      return Null();
    else
      return Number((int)n.handle());
  }

  return String("");
}

DOM::Node KJS::toNode(const KJSO& obj)
{
  if (!obj.derivedFrom("Node")) {
    //    printf("Can't convert %s to Node.\n", obj.imp()->typeInfo()->name);
    return DOM::Node();
  }

  //  printf("Converting %s to Node.\n", obj.imp()->typeInfo()->name);
  const NodeObject *dobj = static_cast<const NodeObject*>(obj.imp());
  DOM::Node n = dobj->toNode();

  return n;
}

KJSO KJS::getDOMNode(DOM::Node n)
{
  if (n.isNull())
    return Null();

  switch (n.nodeType()) {
    case DOM::Node::ELEMENT_NODE:
      if (static_cast<DOM::Element>(n).isHTMLElement())
        return new HTMLElement(static_cast<DOM::HTMLElement>(n));
      else
        return new DOMElement(static_cast<DOM::Element>(n));
    case DOM::Node::ATTRIBUTE_NODE:
      return new DOMAttr(static_cast<DOM::Attr>(n));
    case DOM::Node::TEXT_NODE:
    case DOM::Node::CDATA_SECTION_NODE:
      return new DOMText(static_cast<DOM::Text>(n));
    case DOM::Node::ENTITY_REFERENCE_NODE:
      return new DOMNode(n);
    case DOM::Node::ENTITY_NODE:
      return new DOMEntity(static_cast<DOM::Entity>(n));
    case DOM::Node::PROCESSING_INSTRUCTION_NODE:
      return new DOMProcessingInstruction(static_cast<DOM::ProcessingInstruction>(n));
    case DOM::Node::COMMENT_NODE:
      return new DOMCharacterData(static_cast<DOM::CharacterData>(n));
    case DOM::Node::DOCUMENT_NODE:
      if (static_cast<DOM::Document>(n).isHTMLDocument())
        return new HTMLDocument(static_cast<DOM::HTMLDocument>(n));
      else
        return new DOMDocument(static_cast<DOM::Document>(n));
    case DOM::Node::DOCUMENT_TYPE_NODE:
      return new DOMDocumentType(static_cast<DOM::DocumentType>(n));
    case DOM::Node::DOCUMENT_FRAGMENT_NODE:
      return new DOMNode(n);
    case DOM::Node::NOTATION_NODE:
      return new DOMNotation(static_cast<DOM::Notation>(n));
    default:
      return new DOMNode(n);
  }
}

bool NodeObject::equals(const KJSO& other) const
{
  if (other.derivedFrom("Node")) {
    NodeObject *otherNode = static_cast<NodeObject*>(other.imp());
    return toNode() == otherNode->toNode();
  }
  return false;
}

KJSO KJS::getDOMNamedNodeMap(DOM::NamedNodeMap m)
{
  if (m.isNull())
    return Null();
  else
    return new DOMNamedNodeMap(m);
}

KJSO KJS::getString(DOM::DOMString s)
{
  if (s.isNull())
    return Null();
  else
    return String(s);
}
