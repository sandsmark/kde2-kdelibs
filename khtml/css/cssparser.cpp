/**
 * This file is part of the DOM implementation for KDE.
 *
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *               1999 Waldo Bastian (bastian@kde.org)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id$
 */

//#define CSS_DEBUG
//#define CSS_AURAL

#include "css_stylesheetimpl.h"

#include "css_stylesheet.h"
#include "css_rule.h"
#include "css_ruleimpl.h"
#include "css_valueimpl.h"
#include "csshelper.h"

#include "dom_string.h"
#include "xml/dom_nodeimpl.h"
#include "html/html_documentimpl.h"
#include "dom_exception.h"
using namespace DOM;

#include <kdebug.h>
#include <kglobal.h>

#include "htmlhashes.h"
#include "misc/helper.h"

//
// The following file defines the function
//     const struct props *findProp(const char *word, int len)
//
// with 'props->id' a CSS property in the range from CSS_PROP_MIN to
// (and including) CSS_PROP_TOTAL-1
#include "cssproperties.c"
#include "cssvalues.c"

int DOM::getPropertyID(const char *tagStr, int len)
{
    const struct props *propsPtr = findProp(tagStr, len);
    if (!propsPtr)
        return 0;

    return propsPtr->id;
}

// ------------------------------------------------------------------------------------------------------

bool StyleBaseImpl::deleteMe()
{
    if(!m_parent && _ref <= 0) return true;
    return false;
}

void StyleBaseImpl::checkLoaded()
{
    if(m_parent) m_parent->checkLoaded();
}

DOMString StyleBaseImpl::baseUrl()
{
    // try to find the style sheet. If found look for it's url.
    // If it has none, look for the parentsheet, or the parentNode and
    // try to find out about their url
    StyleBaseImpl *b = this;
    while(b && !b->isStyleSheet())
        b = b->parent();

    if(!b) return 0;

    StyleSheetImpl *sheet = static_cast<StyleSheetImpl *>(b);
    if(!sheet->href().isNull())
        return sheet->href();

    // find parent
    if(sheet->parent()) return sheet->parent()->baseUrl();

    if(!sheet->ownerNode()) return 0;

    DocumentImpl *doc = static_cast<DocumentImpl*>(sheet->ownerNode()->nodeType() == Node::DOCUMENT_NODE ? sheet->ownerNode() : sheet->ownerNode()->ownerDocument());

    return doc->baseURL();
}

/*
 * parsing functions for stylesheets
 */

inline bool isspace(const QChar &c)
{
     return (c == ' ' || c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == QChar(0xa0));
}

const QChar *
StyleBaseImpl::parseSpace(const QChar *curP, const QChar *endP)
{
  bool sc = false;     // possible start comment?
  bool ec = false;     // possible end comment?
  bool ic = false;     // in comment?

  while (curP < endP)
  {
      if (ic)
      {
          if (ec && (*curP == '/'))
              ic = false;
          else if (*curP == '*')
              ec = true;
          else
              ec = false;
      }
      else if (sc && (*curP == '*'))
      {
          ic = true;
      }
      else if (*curP == '/')
      {
          sc = true;
      }
      else if (!isspace(*curP))
      {
          return(curP);
      }
      else
      {
          sc = false;
      }
      curP++;
  }

  return(0);
}

/*
 * ParseToChar
 *
 * Search for an expected character.  Deals with escaped characters,
 * quoted strings, and pairs of braces/parens/brackets.
 */
const QChar *
StyleBaseImpl::parseToChar(const QChar *curP, const QChar *endP, QChar c, bool chkws, bool endAtBlock)
{
    //kdDebug( 6080 ) << "parsetochar: \"" << QString(curP, endP-curP) << "\" searching " << c << " ws=" << chkws << endl;

    bool sq = false; /* in single quote? */
    bool dq = false; /* in double quote? */
    bool esc = false; /* escape mode? */

    while (curP < endP)
    {
        if (esc)
            esc = false;
        else if (*curP == '\\')
            esc = true;
        else if (!sq && (*curP == '"'))
            dq = !dq;
        else if (!dq && (*curP == '\''))
            sq = !sq;
        else if (!sq && !dq && *curP == c)
            return(curP);
        else if (!sq && !dq && chkws && isspace(*curP))
            return(curP);
        else if(!sq && !dq ) {
            if (*curP == '{') {
                if(endAtBlock)
                    return curP;
                curP = parseToChar(curP + 1, endP, '}', false);
                if (!curP)
                    return(0);
            } else if (*curP == '(') {
                curP = parseToChar(curP + 1, endP, ')', false);
                if (!curP)
                    return(0);
            } else if (*curP == '[') {
                curP = parseToChar(curP + 1, endP, ']', false);
                if (!curP)
                    return(0);
            }
        }
        curP++;
    }

    return(0);
}

CSSRuleImpl *
StyleBaseImpl::parseAtRule(const QChar *&curP, const QChar *endP)
{
    curP++;
    const QChar *startP = curP;
    while( *curP != ' ' && *curP != '{' && *curP != '\'')
        curP++;

    QString rule(startP, curP-startP);
    rule = rule.lower();

    //kdDebug( 6080 ) << "rule = '" << rule << "'" << endl;

    if(rule == "import")
    {
        // load stylesheet and pass it over
        curP = parseSpace(curP, endP);
        if(!curP) return 0;
        startP = curP++;
        curP = parseToChar(startP, endP, ';', true);
        // Do not allow @import statements after explicity inlined
        // declarations.  They should simply be ignored per CSS-1
        // specification section 3.0.
        if( !curP || hasInlinedDecl ) return 0;
        DOMString url = khtml::parseURL(DOMString(startP, curP - startP));
        startP = curP;
        if(*curP != ';')
            curP = parseToChar(startP, endP, ';', false, true);
        if(!curP) return 0;
        QString media(startP, curP - startP);
        // ### check if at the beginning of the stylesheet (no style rule
        //     before the import rule)
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "at rule: url = " << url.string()
                        << " media = " << media << endl;
#endif
        // ignore block following @import rule
        if( *curP == '{' ) {
            curP++;
            curP = parseToChar(curP, endP, '}', false);
            if(curP)
                curP++;
        }
        // ### only media="", "screen and "all" are imported for the moment...
        // ### add at least "print" here once the MediaList class is implemented
        if( !media.isEmpty() && !(media.contains("all") || media.contains("screen") ) )
            return 0;
        if(!this->isCSSStyleSheet()) return 0;
        return new CSSImportRuleImpl(this, url, 0);
    }
    else if(rule == "charset")
    {
        // ### invoke decoder
        startP = curP++;
        curP = parseToChar(startP, endP, ';', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "charset = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "font-face")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '{', false);
        if ( !curP || curP >= endP ) return 0;
        curP++;
        curP = parseToChar(curP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "font rule = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "media")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '{', false);
        if ( !curP || curP >= endP ) return 0;
        curP++;
        curP = parseToChar(curP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "media rule = " << QString(startP, curP - startP) << endl;
#endif
    }
    else if(rule == "page")
    {
        startP = curP++;
        curP = parseToChar(startP, endP, '{', false);
        if ( !curP || curP >= endP ) return 0;
        curP++;
        curP = parseToChar(curP, endP, '}', false);
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "page rule = " << QString(startP, curP - startP) << endl;
#endif
    }


    return 0;
}

static DOMString getValue( const QChar *curP, const QChar *endP, const QChar *&endVal)
{
    //QString selecString( curP, endP - curP );
    //kdDebug( 6080 ) << "getValue = \"" << selecString << "\"" << endl;
    endVal = curP;
    endVal++; // ignore first char (could be the ':' form the pseudo classes)
    while( endVal < endP && *endVal != '.' && *endVal != ':' && *endVal != '[' )
        endVal++;
    const QChar *end = endVal;
    if(endVal == endP)
        endVal = 0;
    return DOMString( curP, end - curP);
}

CSSSelector *
StyleBaseImpl::parseSelector2(const QChar *curP, const QChar *endP,
                              CSSSelector::Relation relation)
{
    CSSSelector *cs = new CSSSelector();
#ifdef CSS_DEBUG
    QString selecString( curP, endP - curP );
    kdDebug( 6080 ) << "selectString = \"" << selecString << "\"" << endl;
#endif
    const QChar *endVal = 0;

    if (*curP == '#' && (curP < endP && !((*(curP+1)).isDigit())))
    {
        cs->tag = -1;
        cs->attr = ATTR_ID;
        cs->match = CSSSelector::Exact;
        cs->value = getValue( curP+1, endP, endVal);
    }
    else if (*curP == '.' && (curP < endP && !((*(curP+1)).isDigit())))
    {
        cs->tag = -1;
        cs->attr = ATTR_CLASS;
        cs->match = CSSSelector::List;
        cs->value = getValue( curP+1, endP, endVal);
    }
    else if (*curP == ':')
    {
        cs->tag = -1;
        cs->value = getValue(curP, endP, endVal);
        cs->match = CSSSelector::Pseudo;
    }
    else
    {
        const QChar *startP = curP;
        QString tag;
        while (curP < endP)
        {
            if (*curP =='#' && (curP < endP && !((*(curP+1)).isDigit())))
            {
                tag = QString( startP, curP-startP );
                cs->attr = ATTR_ID;
                cs->match = CSSSelector::Exact;
                cs->value = getValue(curP+1, endP, endVal);
                break;
            }
            else if (*curP == '.' && (curP < endP && !((*(curP+1)).isDigit())))
            {
                tag = QString( startP, curP - startP );
                cs->attr = ATTR_CLASS;
                cs->match = CSSSelector::List;
                cs->value = getValue(curP+1, endP, endVal);
                break;
            }
            else if (*curP == ':')
            {
                // pseudo attributes (:link, :hover, ...)
                tag = QString( startP, curP - startP );
                cs->value = getValue(curP, endP, endVal);
                cs->match = CSSSelector::Pseudo;
                break;
            }
            else if (*curP == '[')
            {
                tag = QString( startP, curP - startP );
                curP++;
                if ( curP >= endP ) {
                    delete cs;
                    return 0;
                }
#ifdef CSS_DEBUG
                kdDebug( 6080 ) << "tag = " << tag << endl;
#endif
                const QChar *closebracket = parseToChar(curP, endP, ']', false);
                if (!closebracket)
                {
                    kdWarning()<<"error in css: closing bracket not found!"<<endl;
                    return 0;
                }
                QString attr;
                const QChar *equal = parseToChar(curP, closebracket, '=', false);
                if(!equal)
                {
                    attr = QString( curP, closebracket - curP );
                    attr = attr.stripWhiteSpace();
#ifdef CSS_DEBUG
                    kdDebug( 6080 ) << "attr = '" << attr << "'" << endl;
#endif
                    cs->match = CSSSelector::Set;
                    endVal = closebracket + 1;
                }
                else
                {
                    // check relation: = / ~= / |=
                    if(*(equal-1) == '~')
                    {
                        attr = QString( curP, equal - curP - 1 );
                        cs->match = CSSSelector::List;
                    }
                    else if(*(equal-1) == '|')
                    {
                        attr = QString( curP, equal - curP - 1 );
                        cs->match = CSSSelector::Hyphen;
                    }
                    else
                    {
                        attr = QString(curP, equal - curP );
                        cs->match = CSSSelector::Exact;
                    }
                }
                attr = attr.stripWhiteSpace();
                cs->attr = khtml::getAttrID(attr.ascii(), attr.length());
                if(equal)
                {
                    equal++;
                    while(equal < endP && *equal == ' ')
                        equal++;
                    if(equal >= endP ) {
                        delete cs;
                        return 0;
                    }
                    endVal = equal;
                    bool hasQuote = false;
                    if(*equal == '\'') {
                        equal++;
                        endVal++;
                        while(endVal < endP && *endVal != '\'')
                            endVal++;
                        hasQuote = true;
                    } else if(*equal == '\"') {
                        equal++;
                        endVal++;
                        while(endVal < endP && *endVal != '\"')
                            endVal++;
                        hasQuote = true;
                    } else {
                      while(endVal < endP && *endVal != ']')
                        endVal++;
                    }
                    cs->value = DOMString(equal, endVal - equal);
                    if ( hasQuote ) {
                      while( endVal < endP - 1 && *endVal != ']' )
                        endVal++;
                    }
                    endVal++;
                    // ### fixme we ignore everything after [..]
                    if( endVal == endP )
                        endVal = 0;
                }
                break;
            }
            else
            {
                curP++;
            }
        }
        if (curP == endP)
        {
            tag = QString( startP, curP - startP );
        }
        if(tag == "*")
        {
            //kdDebug( 6080 ) << "found '*' selector" << endl;
            cs->tag = -1;
        }
        else {
            StyleBaseImpl *root = this;
            DocumentImpl *doc = 0;
            while (root->parent())
                root = root->parent();
            if (root->isCSSStyleSheet())
                doc = static_cast<CSSStyleSheetImpl*>(root)->doc();
            if (doc && !doc->isHTMLDocument()) {
                DOMString s = tag;
                cs->tag = doc->elementId(s.implementation());
            }
            else
                cs->tag = khtml::getTagID(tag.lower().ascii(), tag.length());
        }
   }
   if (cs->tag == 0)
   {
       delete cs;
       return(0);
   }
#ifdef CSS_DEBUG
   kdDebug( 6080 ) << "[Selector: tag=" << cs->tag << " Attribute=" << cs->attr << " match=" << (int)cs->match << " value=" << cs->value.string() << " specificity=" << cs->specificity() << "]" << endl;
#endif


   //stack->print();
   if( endVal ) {
       // lets be recursive
       relation = CSSSelector::SubSelector;
       CSSSelector *stack = parseSelector2(endVal, endP, relation);
       cs->tagHistory = stack;
       cs->relation = relation;
   }

   return cs;
}

CSSSelector *
StyleBaseImpl::parseSelector1(const QChar *curP, const QChar *endP)
{
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector1 is \'" << QString(curP, endP-curP) << "\'" << endl;
#endif

    CSSSelector *selecStack=0;

    curP = parseSpace(curP, endP);
    if (!curP)
        return(0);

    CSSSelector::Relation relation = CSSSelector::Descendant;

    const QChar *startP = curP;
    while (curP <= endP)
    {
        if ((curP == endP) || isspace(*curP) || *curP == '+' || *curP == '>')
        {
            CSSSelector *newsel = parseSelector2(startP, curP, relation);
            if (!newsel) {
                delete selecStack;
                return 0;
            }
            CSSSelector *end = newsel;
            while( end->tagHistory )
                end = end->tagHistory;
            end->tagHistory = selecStack;
            end->relation = relation;
            selecStack = newsel;

            curP = parseSpace(curP, endP);
            if (!curP) {
#ifdef CSS_DEBUG
                kdDebug( 6080 ) << "selector stack is:" << endl;
                selecStack->print();
                kdDebug( 6080 ) << endl;
#endif
                return(selecStack);
            }
            relation = CSSSelector::Descendant;
            if(*curP == '+')
            {
                relation = CSSSelector::Sibling;
                curP++;
                curP = parseSpace(curP, endP);
            }
            else if(*curP == '>')
            {
#ifdef CSS_DEBUG
                kdDebug( 6080 ) << "child selector" << endl;
#endif
                relation = CSSSelector::Child;
                curP++;
                curP = parseSpace(curP, endP);
            }
            //if(selecStack)
            //    selecStack->print();
            startP = curP;
        }
        else
        {
            curP++;
        }
    }
#ifdef CSS_DEBUG
    selecStack->print();
#endif
    return(selecStack);
}

QList<CSSSelector> *
StyleBaseImpl::parseSelector(const QChar *curP, const QChar *endP)
{
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector is \'" << QString(curP, endP-curP) << "\'" << endl;
#endif

    QList<CSSSelector> *slist  = 0;
    const QChar *startP;

    while (curP < endP)
    {
        startP = curP;
        curP = parseToChar(curP, endP, ',', false);
        if (!curP)
            curP = endP;

        CSSSelector *selector = parseSelector1(startP, curP);
        if (selector)
        {
            if (!slist)
            {
                slist = new QList<CSSSelector>;
                slist->setAutoDelete(true);
            }
            slist->append(selector);
        }
        else
        {
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "invalid selector" << endl;
#endif
            // invalid selector, delete
            delete slist;
            return 0;
        }
        curP++;
    }
    return slist;
}


void StyleBaseImpl::parseProperty(const QChar *curP, const QChar *endP)
{
    m_bImportant = false;
    // Get rid of space in front of the declaration

    curP = parseSpace(curP, endP);
    if (!curP)
        return;

    // Search for the required colon or white space
    const QChar *colon = parseToChar(curP, endP, ':', true);
    if (!colon)
        return;

    const QString propName( curP, colon - curP );
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "Property-name = \"" << propName << "\"" << endl;
#endif

    // May have only reached white space before
    if (*colon != ':')
    {
        // Search for the required colon
        colon = parseToChar(curP, endP, ':', false);
        if (!colon)
            return;
    }
    curP = colon+1;
    // remove space in front of the value
    while(curP < endP && *curP == ' ')
        curP++;
    if ( curP >= endP )
        return;

    // search for !important
    const QChar *exclam = parseToChar(curP, endP, '!', false);
    if(exclam)
    {
        //const QChar *imp = parseSpace(exclam+1, endP);
        QString s(exclam+1, endP - exclam - 1);
        s = s.stripWhiteSpace();
        s = s.lower();
        if(s != "important")
            return;
        m_bImportant = true;
        endP = exclam;
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "important property!" << endl;
#endif
    }

    // remove space after the value;
    while (endP > curP)
    {
        if (!isspace(*(endP-1)))
            break;
        endP--;
    }

#ifdef CSS_DEBUG
    QString propVal( curP , endP - curP );
    kdDebug( 6080 ) << "Property-value = \"" << propVal.latin1() << "\"" << endl;
#endif

    const struct props *propPtr = findProp(propName.lower().ascii(), propName.length());
    if (!propPtr)
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "Unknown property" << propName << endl;
#endif
         return;
    }

    unsigned int numProps = m_propList->count();
    if(!parseValue(curP, endP, propPtr->id)) {
#ifdef CSS_DEBUG
        kdDebug(6080) << "invalid property, removing added properties from m_propList" << endl;
#endif
        while(m_propList->count() > numProps)
            m_propList->removeLast();
    }
}

QList<CSSProperty> *StyleBaseImpl::parseProperties(const QChar *curP, const QChar *endP)
{
    m_propList = new QList<CSSProperty>;
    m_propList->setAutoDelete(true);

    while (curP < endP)
    {
        const QChar *startP = curP;
        curP = parseToChar(curP, endP, ';', false);
        if (!curP)
            curP = endP;

#ifdef CSS_DEBUG
        QString propVal( startP , curP - startP );
        kdDebug( 6080 ) << "Property = \"" << propVal.latin1() << "\"" << endl;
#endif

        parseProperty(startP, curP);
        curP++;
    }
    if(!m_propList->isEmpty()) {
        return m_propList;
    } else {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "empty property list" << endl;
#endif
        delete m_propList;
        return 0;
    }
}

static const QChar *getNext( const QChar *curP, const QChar *endP, bool &last )
{
  last = false;
  const QChar *nextP = curP;
  bool ignoreSpace = false;
  while(nextP <= endP) {
    if ( *nextP == '(' )
      ignoreSpace = true;
    if ( *nextP == ')' )
      ignoreSpace = false;
    if ( *nextP == ' ' && !ignoreSpace )
      break;
    if ( *nextP == ';' || nextP == endP ) {
      last = true;
      break;
    }
    nextP++;
  }
  return nextP;
}
// ------------------- begin font property ---------------------
/*
  Parser for the font property of CSS.  See
  http://www.w3.org/TR/REC-CSS2/fonts.html#propdef-font for details.

  Written by Jasmin Blanchette (jasmin@trolltech.com) on 2000-08-16.
*/

#include <qstring.h>
#include <qstringlist.h>


class FontParser {
public:
    enum { Tok_None, Tok_Eoi, Tok_Slash, Tok_Comma, Tok_String, Tok_Symbol };

    QChar yyCh;
    QString yyIn;
    unsigned int yyPos;
    QString yyStr;
    bool strictParsing;

    int getChar() {
        return ( yyPos == yyIn.length() ) ? QChar('\0') : QChar(yyIn[yyPos++]);
    }

    void startTokenizer( const QString& str, bool _strictParsing ) {
        yyIn = str.simplifyWhiteSpace();
        yyPos = 0;
        yyCh = getChar();
        strictParsing = _strictParsing;
        yyTok = Tok_None;
    }

    int getToken()
    {
        yyStr = QString::null;

        if ( yyCh == '\0' )
            return Tok_Eoi;
        if ( yyCh == QChar(' ') )
            yyCh = getChar();

        if ( yyCh == QChar('/') ) {
            yyCh = getChar();
            return Tok_Slash;
        } else if ( yyCh == QChar(',') ) {
            yyCh = getChar();
            return Tok_Comma;
        } else if ( yyCh == QChar('"') ) {
            yyCh = getChar();
            while ( yyCh != QChar('"') && yyCh != '\0' ) {
                yyStr += yyCh;
                yyCh = getChar();
            }
            yyCh = getChar();
            return Tok_String;
        } else if ( yyCh == QChar('\'') ) {
            yyCh = getChar();
            while ( yyCh != QChar('\'') && yyCh != '\0' ) {
                yyStr += yyCh;
                yyCh = getChar();
            }
            yyCh = getChar();
            return Tok_String;
        } else {
            while ( yyCh != '/' && yyCh != ',' && yyCh != '\0' && yyCh != ' ') {
                yyStr += yyCh;
                yyCh = getChar();
            }
            return Tok_Symbol;
        }
    }

    int yyTok;


    bool match( int tok )
    {
        bool matched = ( yyTok == tok );
        if ( matched )
            yyTok = getToken();
        return matched;
    }

    bool matchFontStyle( QString *fstyle )
    {
        bool matched = ( yyTok == Tok_Symbol &&
                         (yyStr == "normal" || yyStr == "italic" ||
                          yyStr == "oblique" || yyStr == "inherit") );
        if ( matched ) {
            *fstyle = yyStr;
            yyTok = getToken();
        }
        return matched;
    }

    bool matchFontVariant( QString *fvariant )
    {
        bool matched = ( yyTok == Tok_Symbol &&
                         (yyStr == "normal" || yyStr == "small-caps"
                          || yyStr == "inherit") );
        if ( matched ) {
            *fvariant = yyStr;
            yyTok = getToken();
        }
        return matched;
    }

    bool matchFontWeight( QString *fweight )
    {
        bool matched = ( yyTok == Tok_Symbol );
        if ( matched ) {
            if ( yyStr.length() == 3 ) {
                matched = ( yyStr[0].unicode() >= '1' &&
                            yyStr[0].unicode() <= '9' &&
                            yyStr.right(2) == QString::fromLatin1("00") );
            } else {
                matched = ( yyStr == "normal" || yyStr == "bold" ||
                            yyStr == "bolder" || yyStr == "lighter" ||
                            yyStr == "inherit" );
            }
        }
        if ( matched ) {
            *fweight = yyStr;
            yyTok = getToken();
        }
        return matched;
    }

    bool matchFontSize( QString *fsize )
    {
        bool matched = ( yyTok == Tok_Symbol );
        if ( matched ) {
            *fsize = yyStr;
            yyTok = getToken();
        }
        return matched;
    }

    bool matchLineHeight( QString *lheight )
    {
        bool matched = ( yyTok == Tok_Symbol );
        if ( matched ) {
            *lheight = yyStr;
            yyTok = getToken();
        }
        return matched;
    }

    bool matchNameFamily( QString *ffamily )
    {
        bool matched = false;
        if ( yyTok == Tok_Symbol || ( yyTok == Tok_String && !strictParsing ) ) {
            // accept quoted "serif" only in non strict mode.
            *ffamily = yyStr;
            // unquoted courier new should return courier new
            while( (yyTok = getToken()) == Tok_Symbol )
                *ffamily += " " + yyStr;
            matched = true;
        } else if ( yyTok == Tok_String ) {
            if ( yyStr != "serif" && yyStr != "sans-serif" &&
                 yyStr != "cursive" && yyStr != "fantasy" &&
                 yyStr != "monospace" ) {
                *ffamily = yyStr;
                yyTok = getToken();
                matched = true;
            }
        }
        return matched;
    }

    bool matchFontFamily( QString *ffamily )
    {
        QStringList t;
        if ( !matchFontFamily( &t ) )
            return false;

        *ffamily = t.join(", ");
        return TRUE;
    }

    bool matchFontFamily ( QStringList *ffamily )
    {
        if ( yyTok == Tok_None )
            yyTok = getToken();
#if 0
        // ###
        if ( yyTok == Tok_String && yyStr == "inherit" ) {
            ffamily->clear();
            yyTok = getToken();
            return TRUE;
        }
#endif

        QString name;
        do {
            if ( !matchNameFamily(&name) )
                return FALSE;
            ffamily->append( name );
        } while ( match(Tok_Comma) );

        return true;
    }

    bool matchRealFont( QString *fstyle, QString *fvariant, QString *fweight,
                               QString *fsize, QString *lheight, QString *ffamily )
    {
        bool metFstyle = matchFontStyle( fstyle );
        bool metFvariant = matchFontVariant( fvariant );
        matchFontWeight( fweight );
        if ( !metFstyle )
            metFstyle = matchFontStyle( fstyle );
        if ( !metFvariant )
            matchFontVariant( fvariant );
        if ( !metFstyle )
            matchFontStyle( fstyle );

        if ( !matchFontSize(fsize) )
            return FALSE;
        if ( match(Tok_Slash) ) {
            if ( !matchLineHeight(lheight) )
                return FALSE;
        }
        if ( !matchFontFamily(ffamily) )
            return FALSE;
        return true;
    }
};

bool StyleBaseImpl::parseFont(const QChar *curP, const QChar *endP)
{
    QString str( curP, endP - curP );
    QString fstyle;
    QString fvariant;
    QString fweight;
    QString fsize;
    QString lheight;
    QString ffamily;

    FontParser f;
    f.startTokenizer( str, strictParsing );

    //qDebug( "%s", str.latin1() );

    if ( f.yyIn == "caption" || f.yyIn == "icon" || f.yyIn == "menu" ||
         f.yyIn == "message-box" || f.yyIn == "small-caption" ||
         f.yyIn == "status-bar" || f.yyIn == "inherit" ) {
        kdDebug( 6080 ) << "system font requested..." << endl;
    } else {
        f.yyTok = f.getToken();
        if ( f.matchRealFont(&fstyle, &fvariant, &fweight, &fsize, &lheight,
                           &ffamily) ) {
//          qDebug( "  %s %s %s %s / %s", fstyle.latin1(),
//                     fvariant.latin1(), fweight.latin1(), fsize.latin1(),
//                     lheight.latin1() );
            if(!fstyle.isNull())
                parseValue(fstyle.unicode(), fstyle.unicode()+fstyle.length(),
                           CSS_PROP_FONT_STYLE);
            if(!fvariant.isNull())
                parseValue(fvariant.unicode(), fvariant.unicode()+fvariant.length(),
                           CSS_PROP_FONT_VARIANT);
            if(!fweight.isNull())
                parseValue(fweight.unicode(), fweight.unicode()+fweight.length(),
                           CSS_PROP_FONT_WEIGHT);
            if(!fsize.isNull())
                parseValue(fsize.unicode(), fsize.unicode()+fsize.length(),
                           CSS_PROP_FONT_SIZE);
            if(!lheight.isNull())
                parseValue(lheight.unicode(), lheight.unicode()+lheight.length(),
                           CSS_PROP_LINE_HEIGHT);
            if(!ffamily.isNull())
                parseValue(ffamily.unicode(), ffamily.unicode()+ffamily.length(),
                           CSS_PROP_FONT_FAMILY);

            return true;
        }
    }
    return false;
}

// ---------------- end font property --------------------------

bool StyleBaseImpl::parseValue( const QChar *curP, const QChar *endP, int propId, bool important,
                                QList<CSSProperty> *propList)
{
  m_bImportant = important;
  m_propList = propList;
  return parseValue(curP, endP, propId);
}

bool StyleBaseImpl::parseValue( const QChar *curP, const QChar *endP, int propId)
{
   QString value(curP, endP - curP);
   value = value.lower();
   const char *val = value.ascii();

   CSSValueImpl *parsedValue = 0;
   //kdDebug( 6080 ) << "value: " << value << " val: " << val <<  endl;

   if(!strcmp(val, "inherit")) {
        parsedValue = new CSSInheritedValueImpl(); // inherited value
   } else {
    switch(propId)
    {
    /* The comment to the left defines all valid value of this properties as defined
     * in CSS 2, Appendix F. Property index
     */

    /* These properties are not checked for validity, pure performance consideration */

    case CSS_PROP_BORDER_COLLAPSE:      // collapse | separate | inherit
    case CSS_PROP_CAPTION_SIDE:         // top | bottom | left | right | inherit
    case CSS_PROP_CLEAR:                // none | left | right | both | inherit
    case CSS_PROP_CURSOR:               // [ [<uri> ,]* [ auto | crosshair | default | pointer | move |
                                        // e-resize | ne-resize | nw-resize | n-resize | se-resize |
                                        // sw-resize | s-resize | w-resize| text | wait | help ] ] | inherit
                                        // ### ADD SUPPPORT FOR URI
    case CSS_PROP_DIRECTION:            // ltr | rtl | inherit
    case CSS_PROP_DISPLAY:
        // inline | block | list-item | run-in | compact | marker | table | inline-table |
        // table-row-group | table-header-group | table-footer-group | table-row |
        // table-column-group | table-column | table-cell | table-caption | none | inherit
    case CSS_PROP_EMPTY_CELLS:          // show | hide | inherit
    case CSS_PROP_FLOAT:                // left | right | none | inherit
    case CSS_PROP_FONT_STRETCH:
        // normal | wider | narrower | ultra-condensed | extra-condensed | condensed |
        // semi-condensed |  semi-expanded | expanded | extra-expanded | ultra-expanded |
        // inherit
    case CSS_PROP_LIST_STYLE_POSITION:  // inside | outside | inherit
    case CSS_PROP_LIST_STYLE_TYPE:
        // disc | circle | square | decimal | decimal-leading-zero | lower-roman |
        // upper-roman | lower-greek | lower-alpha | lower-latin | upper-alpha |
        // upper-latin | hebrew | armenian | georgian | cjk-ideographic | hiragana |
        // katakana | hiragana-iroha | katakana-iroha | none | inherit
    case CSS_PROP_OVERFLOW:             // visible | hidden | scroll | auto | inherit
    case CSS_PROP_PAGE:                 // <identifier> | auto // ### CHECK
    case CSS_PROP_PAGE_BREAK_AFTER:     // auto | always | avoid | left | right | inherit
    case CSS_PROP_PAGE_BREAK_BEFORE:    // auto | always | avoid | left | right | inherit
    case CSS_PROP_PAGE_BREAK_INSIDE:    // avoid | auto | inherit
    case CSS_PROP_POSITION:             // static | relative | absolute | fixed | inherit
    case CSS_PROP_TABLE_LAYOUT:         // auto | fixed | inherit
    case CSS_PROP_TEXT_TRANSFORM:       // capitalize | uppercase | lowercase | none | inherit
    case CSS_PROP_UNICODE_BIDI:         // normal | embed | bidi-override | inherit
    case CSS_PROP_VISIBILITY:           // visible | hidden | collapse | inherit
    case CSS_PROP_WHITE_SPACE:          // normal | pre | nowrap | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval)
        {
            parsedValue = new CSSPrimitiveValueImpl(cssval->id);
        }
        break;
    }

    /* All the CSS properties are not supported at the moment. Note that all the CSS2 Aural properties
     * are only checked, if CSS_AURAL is defined (see parseAuralValues). As we don't support them at all
     * this seems reasonable.
     */

    case CSS_PROP_SIZE:                 // <length>{1,2} | auto | portrait | landscape | inherit
    case CSS_PROP_QUOTES:               // [<string> <string>]+ | none | inherit
    case CSS_PROP_CLIP:                 // <shape> | auto | inherit
    case CSS_PROP_TEXT_SHADOW:          // none | [<color> || <length> <length> <length>? ,]*
                                        //    [<color> || <length> <length> <length>?] | inherit
    case CSS_PROP_CONTENT:              // [ <string> | <uri> | <counter> | attr(X) | open-quote |
                                        // close-quote | no-open-quote | no-close-quote ]+ | inherit
    {
            // ### To be done
            break;
    }

    /* Start of supported CSS properties with validation. This is needed for parseShortHand to work
     * correctly
     */

    case CSS_PROP_TEXT_ALIGN:   // left | right | center | justify | <string> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_LEFT || id == CSS_VAL_RIGHT || id == CSS_VAL_CENTER ||
                id == CSS_VAL_JUSTIFY || id == CSS_VAL_KONQ_CENTER) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        } else {
            parsedValue = new CSSPrimitiveValueImpl(DOMString(value), CSSPrimitiveValue::CSS_STRING);
        }
        break;
    }
    case CSS_PROP_OUTLINE_STYLE:        // <border-style> | inherit
      {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_OUTLINE_STYLE: " << val << endl;
#endif
      }
    case CSS_PROP_BORDER_TOP_STYLE:     //// <border-style> | inherit
    case CSS_PROP_BORDER_RIGHT_STYLE:   //   Defined as:    none | hidden | dotted | dashed |
    case CSS_PROP_BORDER_BOTTOM_STYLE:  //   solid | double | groove | ridge | inset | outset
    case CSS_PROP_BORDER_LEFT_STYLE:    ////
      {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            // Sorry Dirk, but I need this for parseShorthand
            if (id == CSS_VAL_NONE || id == CSS_VAL_HIDDEN || id == CSS_VAL_DOTTED ||
                id == CSS_VAL_DASHED || id == CSS_VAL_SOLID || id == CSS_VAL_DOUBLE ||
                id == CSS_VAL_GROOVE || id == CSS_VAL_RIDGE || id == CSS_VAL_INSET ||
                id == CSS_VAL_OUTSET) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        }
        break;
    }
    case CSS_PROP_FONT_WEIGHT:  // normal | bold | bolder | lighter | 100 | 200 | 300 | 400 |
                                // 500 | 600 | 700 | 800 | 900 | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_NORMAL || id == CSS_VAL_BOLD || id == CSS_VAL_BOLDER || id == CSS_VAL_LIGHTER) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            const QString val2( value.stripWhiteSpace() );
            int id = 0;
            if (val2 == "100" || val2 == "200" || val2 == "300" || val2 == "400" || val2 == "500")
                id = CSS_VAL_NORMAL;
            else if ( val2 == "600" || val2 == "700" || val2 == "800" || val2 == "900" )
                id = CSS_VAL_BOLD;
            if ( id )
                parsedValue = new CSSPrimitiveValueImpl(id);
        }
        break;
    }
    case CSS_PROP_BACKGROUND_REPEAT:    // repeat | repeat-x | repeat-y | no-repeat | inherit
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_BACKGROUND_REPEAT: " << val << endl;
#endif
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if ( id == CSS_VAL_REPEAT || id == CSS_VAL_REPEAT_X ||
                 id == CSS_VAL_REPEAT_Y || id == CSS_VAL_NO_REPEAT ) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        }
        break;
    }
    case CSS_PROP_BACKGROUND_ATTACHMENT: // scroll | fixed
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_BACKGROUND_ATTACHEMENT: " << val << endl;
#endif
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if ( id == CSS_VAL_SCROLL || id == CSS_VAL_FIXED ) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        }
        break;
    }
    case CSS_PROP_BACKGROUND_POSITION:
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_BACKGROUND_POSITION: " << val << endl;
#endif
        /* Problem: center is ambigous
         * In case of 'center' center defines X and Y coords
         * In case of 'center top', center defines the Y coord
         * in case of 'center left', center defines the X coord
         */
        bool isLast;
        const QChar* nextP = getNext(curP, endP, isLast);
        QConstString property1(const_cast<QChar*>( curP ), nextP - curP);
        const struct css_value *cssval1 = findValue( property1.string().ascii(),
                                                     property1.string().length());
        if ( !cssval1 ) {
            int properties[2] = { CSS_PROP_KONQ_BGPOS_X, CSS_PROP_KONQ_BGPOS_Y };
            return parseShortHand(curP, endP, properties, 2);
        }
        const struct css_value *cssval2 = 0;
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "prop 1: [" << property1.string() << "]" << " isLast: " << isLast <<  endl;
#endif
        if ( !isLast) {
            curP = nextP+1;
            nextP = getNext(curP, endP, isLast);
            QConstString property2(const_cast<QChar*>( curP ), nextP - curP);
            cssval2 = findValue( property2.string().ascii(), property2.string().length());
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "prop 2: [" << property2.string() << "]" << " isLast: " << isLast <<  endl;
#endif
        }

        int valX = -1;
        int valY = -1;
        int id1 = cssval1 ? cssval1->id : -1;
        int id2 = cssval2 ? cssval2->id : CSS_VAL_CENTER;
        // id1 will influence X and id2 will influence Y
        if ( id2 == CSS_VAL_LEFT || id2 == CSS_VAL_RIGHT ||
             id1 == CSS_VAL_TOP  || id1 == CSS_VAL_BOTTOM) {
            int h = id1; id1 = id2; id2 = h;
        }

        switch( id1 ) {
        case CSS_VAL_LEFT:   valX = 0;   break;
        case CSS_VAL_CENTER: valX = 50;  break;
        case CSS_VAL_RIGHT:  valX = 100; break;
        default: break;
        }

        switch ( id2 ) {
        case CSS_VAL_TOP:    valY = 0;   break;
        case CSS_VAL_CENTER: valY = 50;  break;
        case CSS_VAL_BOTTOM: valY = 100; break;
        default: break;
        }

#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "valX: " << valX << " valY: " << valY <<  endl;
#endif
        /* CSS 14.2
         * Keywords cannot be combined with percentage values or length values.
         * -> No mix between keywords and other units.
         */
        if (valX !=-1 && valY !=-1) {
                setParsedValue( CSS_PROP_KONQ_BGPOS_X,
                                new CSSPrimitiveValueImpl(valX, CSSPrimitiveValue::CSS_PERCENTAGE));
                setParsedValue( CSS_PROP_KONQ_BGPOS_Y,
                                new CSSPrimitiveValueImpl(valY, CSSPrimitiveValue::CSS_PERCENTAGE));
                return true;
        }
        break;
    }
    case CSS_PROP_KONQ_BGPOS_X:
    case CSS_PROP_KONQ_BGPOS_Y:
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_KONQ_BGPOS_{X|Y}: " << val << endl;
#endif
        parsedValue = parseUnit(curP, endP, PERCENT | NUMBER | LENGTH);
        break;
    }
    case CSS_PROP_BORDER_SPACING:
    {
        // ### should be able to have two values
        parsedValue = parseUnit(curP, endP, LENGTH);
        break;
    }
    case CSS_PROP_OUTLINE_COLOR:        // <color> | invert | inherit
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_OUTLINE_COLOR: " << val << endl;
#endif
        // outline has "invert" as additional keyword. we handle
        // it as invalid color and add a special case during rendering
        if ( value == "invert" ) {
            parsedValue = new CSSPrimitiveValueImpl( QColor() );
            break;
        }
        // Break is explictly missing, looking for <color>
    }
    case CSS_PROP_BACKGROUND_COLOR:     // <color> | transparent | inherit
    {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "CSS_PROP_BACKGROUND_COLOR: " << val << endl;
#endif
        if ( value == "transparent" ) {
            // ### What are we to do here?
            break;
        }
        // Break is explictly missing, looking for <color>
    }
    case CSS_PROP_COLOR:                // <color> | inherit
    case CSS_PROP_BORDER_TOP_COLOR:     // <color> | inherit
    case CSS_PROP_BORDER_RIGHT_COLOR:   // <color> | inherit
    case CSS_PROP_BORDER_BOTTOM_COLOR:  // <color> | inherit
    case CSS_PROP_BORDER_LEFT_COLOR:    // <color> | inherit
    case CSS_PROP_TEXT_DECORATION_COLOR:        //
    case CSS_PROP_SCROLLBAR_FACE_COLOR:         // IE5.5
    case CSS_PROP_SCROLLBAR_SHADOW_COLOR:       // IE5.5
    case CSS_PROP_SCROLLBAR_HIGHLIGHT_COLOR:    // IE5.5
    case CSS_PROP_SCROLLBAR_3DLIGHT_COLOR:      // IE5.5
    case CSS_PROP_SCROLLBAR_DARKSHADOW_COLOR:   // IE5.5
    case CSS_PROP_SCROLLBAR_TRACK_COLOR:        // IE5.5
    case CSS_PROP_SCROLLBAR_ARROW_COLOR:        // IE5.5
    {
        const QString val2( value.stripWhiteSpace() );
        //kdDebug(6080) << "parsing color " << val2 << endl;
        QColor c;
        khtml::setNamedColor(c, val2);
        if(!c.isValid() && (val2 != "transparent" ) && !val2.isEmpty() ) return false;
        //kdDebug( 6080 ) << "color is: " << c.red() << ", " << c.green() << ", " << c.blue() << endl;
        parsedValue = new CSSPrimitiveValueImpl(c);
        break;
    }
    case CSS_PROP_BACKGROUND_IMAGE:     // <uri> | none | inherit
#ifdef CSS_DEBUG
    {
        kdDebug( 6080 ) << "CSS_PROP_BACKGROUND_IMAGE: " << val << endl;
    }
#endif
    case CSS_PROP_LIST_STYLE_IMAGE:     // <uri> | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval && cssval->id == CSS_VAL_NONE)
        {
            parsedValue = new CSSImageValueImpl();
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "empty image " << static_cast<CSSImageValueImpl *>(parsedValue)->image() << endl;
#endif
            break;
        } else {
            DOMString value(curP, endP - curP);
            value = khtml::parseURL(value);
#ifdef CSS_DEBUG
            kdDebug( 6080 ) << "image, url=" << value.string() << " base=" << baseUrl().string() << endl;
#endif
            parsedValue = new CSSImageValueImpl(value, baseUrl(), this);
            break;
        }
    }

    case CSS_PROP_OUTLINE_WIDTH:        // <border-width> | inherit
#ifdef CSS_DEBUG
      {
        kdDebug( 6080 ) << "CSS_PROP_OUTLINE_WIDTH: " << val << endl;
      }
#endif
    case CSS_PROP_BORDER_TOP_WIDTH:     //// <border-width> | inherit
    case CSS_PROP_BORDER_RIGHT_WIDTH:   //   Which is defined as
    case CSS_PROP_BORDER_BOTTOM_WIDTH:  //   thin | medium | thick | <length>
    case CSS_PROP_BORDER_LEFT_WIDTH:    ////
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_THIN || id == CSS_VAL_MEDIUM || id == CSS_VAL_THICK) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH);
        }
        break;
    }
    case CSS_PROP_MARKER_OFFSET:        // <length> | auto | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_AUTO) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH);
        }
        break;
    }
    case CSS_PROP_LETTER_SPACING:       // normal | <length> | inherit
    case CSS_PROP_WORD_SPACING:         // normal | <length> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NORMAL) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH);
        }
        break;
    }
    case CSS_PROP_PADDING_TOP:          //// <padding-width> | inherit
    case CSS_PROP_PADDING_RIGHT:        //   Which is defined as
    case CSS_PROP_PADDING_BOTTOM:       //   <length> | <percentage>
    case CSS_PROP_PADDING_LEFT:         ////
    case CSS_PROP_TEXT_INDENT:          // <length> | <percentage> | inherit
    case CSS_PROP_MIN_HEIGHT:           // <length> | <percentage> | inherit
    case CSS_PROP_MIN_WIDTH:            // <length> | <percentage> | inherit
    {
        parsedValue = parseUnit(curP, endP, LENGTH | PERCENT);
        break;
    }
    case CSS_PROP_FONT_SIZE:            // <absolute-size> | <relative-size> |
                                        // <length> | <percentage> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_XX_SMALL || id == CSS_VAL_X_SMALL || id == CSS_VAL_SMALL ||
                id == CSS_VAL_MEDIUM || id == CSS_VAL_LARGE || id == CSS_VAL_X_LARGE ||
                id == CSS_VAL_XX_LARGE || id == CSS_VAL_SMALLER || id == CSS_VAL_LARGER) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH | PERCENT );
        }
        break;
    }
    case CSS_PROP_FONT_STYLE:           // normal | italic | oblique | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if ( id == CSS_VAL_NORMAL || id == CSS_VAL_ITALIC || id == CSS_VAL_OBLIQUE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        }
        break;
    }
    case CSS_PROP_FONT_VARIANT:         // normal | small-caps | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if ( id == CSS_VAL_NORMAL || id == CSS_VAL_SMALL_CAPS) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        }
        break;
    }
    case CSS_PROP_VERTICAL_ALIGN:       // baseline | sub | super | top | text-top | middle |
                                        // bottom | text-bottom | <percentage> | <length> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_BASELINE || id == CSS_VAL_SUB || id == CSS_VAL_SUPER ||
                id == CSS_VAL_TOP || id == CSS_VAL_TEXT_TOP || id == CSS_VAL_MIDDLE ||
                id == CSS_VAL_BOTTOM || id == CSS_VAL_TEXT_BOTTOM) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH | PERCENT );
        }
        break;
    }
    case CSS_PROP_MAX_HEIGHT:           // <length> | <percentage> | none | inherit
    case CSS_PROP_MAX_WIDTH:            // <length> | <percentage> | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NONE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH | PERCENT );
        }
        break;
    }
    case CSS_PROP_BOTTOM:               // <length> | <percentage> | auto | inherit
    case CSS_PROP_LEFT:                 // <length> | <percentage> | auto | inherit
    case CSS_PROP_RIGHT:                // <length> | <percentage> | auto | inherit
    case CSS_PROP_TOP:                  // <length> | <percentage> | auto | inherit
    case CSS_PROP_HEIGHT:               // <length> | <percentage> | auto | inherit
    case CSS_PROP_WIDTH:                // <length> | <percentage> | auto | inherit
    case CSS_PROP_MARGIN_TOP:           //// <margin-width> | inherit
    case CSS_PROP_MARGIN_RIGHT:         //   Which is defined as
    case CSS_PROP_MARGIN_BOTTOM:        //   <length> | <percentage> | auto | inherit
    case CSS_PROP_MARGIN_LEFT:          ////
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_AUTO) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, LENGTH | PERCENT );
        }
        break;
    }
    case CSS_PROP_FONT_SIZE_ADJUST:     // <number> | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NONE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        } else {
        parsedValue = parseUnit(curP, endP, NUMBER);
        }
        break;
    }
    case CSS_PROP_Z_INDEX:              // auto | <integer> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_AUTO) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        }
        // break explicitly missing, looking for <number>
    }
    case CSS_PROP_ORPHANS:              // <integer> | inherit
    case CSS_PROP_WIDOWS:               // <integer> | inherit
    {
        parsedValue = parseUnit(curP, endP, INTEGER);
        break;
    }
    case CSS_PROP_LINE_HEIGHT:          // normal | <number> | <length> | <percentage> | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NORMAL) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, NUMBER | LENGTH | PERCENT);
        }
        break;
    }
    case CSS_PROP_COUNTER_INCREMENT:    // [ <identifier> <integer>? ]+ | none | inherit
    case CSS_PROP_COUNTER_RESET:        // [ <identifier> <integer>? ]+ | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NONE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            CSSValueListImpl *list = new CSSValueListImpl;
            QString str(curP, endP-curP);
            int pos=0, pos2;
            while( 1 )
            {
                pos2 = str.find(',', pos);
                QString face = str.mid(pos, pos2-pos);
                face = face.stripWhiteSpace();
                if(face.length() == 0) break;
                // ### single quoted is missing...
                if(face[0] == '\"') face.remove(0, 1);
                if(face[face.length()-1] == '\"') face = face.left(face.length()-1);
                //kdDebug( 6080 ) << "found face '" << face << "'" << endl;
                list->append(new CSSPrimitiveValueImpl(DOMString(face), CSSPrimitiveValue::CSS_STRING));
                pos = pos2 + 1;
                if(pos2 == -1) break;
            }
            //kdDebug( 6080 ) << "got " << list->length() << " faces" << endl;
            if(list->length())
                parsedValue = list;
            else
                delete list;
            break;
        }
    }
    case CSS_PROP_FONT_FAMILY:          // [[ <family-name> | <generic-family> ],]* [<family-name>
                                        // | <generic-family>] | inherit
    {
        CSSValueListImpl *list = new CSSValueListImpl;
        QString str(curP, endP-curP);
        // css2 compatible parsing...
        FontParser fp;
        fp.startTokenizer( str, strictParsing );
        QStringList families;
        if ( !fp.matchFontFamily( &families ) )
            return false;
        for ( QStringList::Iterator it = families.begin(); it != families.end(); ++it ) {
            if( *it != QString::null ) {
               list->append(new CSSPrimitiveValueImpl(DOMString(*it), CSSPrimitiveValue::CSS_STRING));
               //kdDebug() << "StyleBaseImpl::parsefont: family='" << *it << "'" << endl;
            }
        }
        //kdDebug( 6080 ) << "got " << list->length() << " faces" << endl;
        if(list->length())
            parsedValue = list;
        else
            delete list;
        break;
    }
    case CSS_PROP_TEXT_DECORATION:      // none | [ underline || overline || line-through || blink ] |
                                        // inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
           if (cssval->id == CSS_VAL_NONE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
           } else {
            CSSValueListImpl *list = new CSSValueListImpl;
            QString str(curP, endP-curP);
            str.simplifyWhiteSpace();
            //kdDebug( 6080 ) << "text-decoration: '" << str << "'" << endl;
            int pos=0, pos2;
            while( 1 )
            {
               pos2 = str.find(' ', pos);
               QString decoration = str.mid(pos, pos2-pos);
               decoration = decoration.stripWhiteSpace();
               //kdDebug( 6080 ) << "found decoration '" << decoration << "'" << endl;
               const struct css_value *cssval = findValue(decoration.lower().ascii(),
                                                          decoration.length());
               if (cssval) {
                   list->append(new CSSPrimitiveValueImpl(cssval->id));
               }
               pos = pos2 + 1;
               if(pos2 == -1) break;
            }
            //kdDebug( 6080 ) << "got " << list->length() << "d decorations" << endl;
            if(list->length()) {
                parsedValue = list;
            } else {
                delete list;
            }
           }
        }
        break;
    }

    /* shorthand properties */

    case CSS_PROP_BACKGROUND:           // ['background-color' || 'background-image' ||
                                        // 'background-repeat' || 'background-attachment' ||
                                        // 'background-position'] | inherit
    {
#ifdef CSS_DEBUG
        kdDebug(6080) << "CSS_PROP_BACKGROUND" << endl;
#endif
        return parseBackground(curP, endP);
    }
    case CSS_PROP_BORDER:               // [ 'border-width' || 'border-style' || <color> ] | inherit
    {
        const int properties[3] = { CSS_PROP_BORDER_WIDTH, CSS_PROP_BORDER_STYLE,
                                    CSS_PROP_BORDER_COLOR };
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_BORDER_TOP:           // [ 'border-top-width' || 'border-style' || <color> ] | inherit
    {
        const int properties[3] = { CSS_PROP_BORDER_TOP_WIDTH, CSS_PROP_BORDER_TOP_STYLE,
                                    CSS_PROP_BORDER_TOP_COLOR};
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_BORDER_RIGHT:         // [ 'border-right-width' || 'border-style' || <color> ] | inherit
    {
        const int properties[3] = { CSS_PROP_BORDER_RIGHT_WIDTH, CSS_PROP_BORDER_RIGHT_STYLE,
                                    CSS_PROP_BORDER_RIGHT_COLOR };
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_BORDER_BOTTOM:        // [ 'border-bottom-width' || 'border-style' || <color> ] | inherit
    {
        const int properties[3] = { CSS_PROP_BORDER_BOTTOM_WIDTH, CSS_PROP_BORDER_BOTTOM_STYLE,
                                    CSS_PROP_BORDER_BOTTOM_COLOR };
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_BORDER_LEFT:          // [ 'border-left-width' || 'border-style' || <color> ] | inherit
    {
        const int properties[3] = { CSS_PROP_BORDER_LEFT_WIDTH, CSS_PROP_BORDER_LEFT_STYLE,
                                    CSS_PROP_BORDER_LEFT_COLOR };
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_OUTLINE:              // [ 'outline-color' || 'outline-style' || 'outline-width' ] | inherit
    {
        const int properties[3] = { CSS_PROP_OUTLINE_COLOR, CSS_PROP_OUTLINE_STYLE,
                                    CSS_PROP_OUTLINE_WIDTH };
        return parseShortHand(curP, endP, properties, 3);
    }
    case CSS_PROP_BORDER_COLOR:         // <color>{1,4} | transparent | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval && cssval->id == CSS_VAL_TRANSPARENT)
        {
            // set border colors to invalid
            parsedValue = new CSSPrimitiveValueImpl(CSS_VAL_TRANSPARENT);
            break;
        }
        const int properties[4] = { CSS_PROP_BORDER_TOP_COLOR, CSS_PROP_BORDER_RIGHT_COLOR,
                                    CSS_PROP_BORDER_BOTTOM_COLOR, CSS_PROP_BORDER_LEFT_COLOR };
        return parse4Values(curP, endP, properties);
    }
    case CSS_PROP_BORDER_WIDTH:         // <border-width>{1,4} | inherit
    {
        const int properties[4] = { CSS_PROP_BORDER_TOP_WIDTH, CSS_PROP_BORDER_RIGHT_WIDTH,
                                    CSS_PROP_BORDER_BOTTOM_WIDTH, CSS_PROP_BORDER_LEFT_WIDTH };
        return parse4Values(curP, endP, properties);
    }
    case CSS_PROP_BORDER_STYLE:         // <border-style>{1,4} | inherit
    {
        const int properties[4] = { CSS_PROP_BORDER_TOP_STYLE, CSS_PROP_BORDER_RIGHT_STYLE,
                                    CSS_PROP_BORDER_BOTTOM_STYLE, CSS_PROP_BORDER_LEFT_STYLE };
        return parse4Values(curP, endP, properties);
    }
    case CSS_PROP_MARGIN:               // <margin-width>{1,4} | inherit
    {
        const int properties[4] = { CSS_PROP_MARGIN_TOP, CSS_PROP_MARGIN_RIGHT,
                                    CSS_PROP_MARGIN_BOTTOM, CSS_PROP_MARGIN_LEFT };
        return parse4Values(curP, endP, properties);
    }
    case CSS_PROP_PADDING:              // <padding-width>{1,4} | inherit
    {
        const int properties[4] = { CSS_PROP_PADDING_TOP, CSS_PROP_PADDING_RIGHT,
                                    CSS_PROP_PADDING_BOTTOM, CSS_PROP_PADDING_LEFT };
        return parse4Values(curP, endP, properties);
    }
    case CSS_PROP_FONT:                 // [ [ 'font-style' || 'font-variant' || 'font-weight' ]?
                                        // 'font-size' [ / 'line-height' ]? 'font-family' ] |
                                        // caption | icon | menu | message-box | small-caption |
                                        // status-bar | inherit
    {
        return parseFont(curP, endP);
    }
    case CSS_PROP_LIST_STYLE:
    {
        const int properties[3] = { CSS_PROP_LIST_STYLE_TYPE, CSS_PROP_LIST_STYLE_POSITION,
                                    CSS_PROP_LIST_STYLE_IMAGE };
        return  parseShortHand(curP, endP, properties, 3);
    }

    default:
      {
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "illegal or CSS2 Aural property: " << val << endl;
#endif
      }
    }
   }
   if ( parsedValue ) {
     setParsedValue(propId, parsedValue);
     return true;
   } else {
#ifndef CSS_AURAL
     return false;
#endif
#ifdef CSS_AURAL
     return parseAuralValue(curP, endP, propId);
#endif
   }
}

#ifdef CSS_AURAL
/* parseAuralValue */
bool StyleBaseImpl::parseAuralValue( const QChar *curP, const QChar *endP, int propId )
{
    QString value(curP, endP - curP);
    value = value.lower();
    const char *val = value.ascii();

    CSSValueImpl *parsedValue = 0;
    kdDebug( 6080 ) << "parseAuralValue: " << value << " val: " << val <<  endl;

    /* AURAL Properies */
    switch(propId)
    {
    case CSS_PROP_AZIMUTH:              // <angle> | [[ left-side | far-left | left | center-left | center |
                                        // center-right | right | far-right | right-side ] || behind ] |
                                        // leftwards | rightwards | inherit
    case CSS_PROP_PAUSE_AFTER:          // <time> | <percentage> | inherit
    case CSS_PROP_PAUSE_BEFORE:         // <time> | <percentage> | inherit
    case CSS_PROP_PAUSE:                // [ [<time> | <percentage>]{1,2} ] | inherit
    case CSS_PROP_PLAY_DURING:          // <uri> mix? repeat? | auto | none | inherit
    case CSS_PROP_VOICE_FAMILY:         // [[<specific-voice> | <generic-voice> ],]*
                                        // [<specific-voice> | <generic-voice> ] | inherit
    {
      // ### TO BE DONE
        break;
    }
    case CSS_PROP_CUE:                  // [ 'cue-before' || 'cue-after' ] | inherit
    {
        const int properties[2] = {
                CSS_PROP_CUE_BEFORE,
                CSS_PROP_CUE_AFTER};
        return parse2Values(curP, endP, properties);
    }
    case CSS_PROP_CUE_AFTER:            // <uri> | none | inherit
    case CSS_PROP_CUE_BEFORE:           // <uri> | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            if (cssval->id == CSS_VAL_NONE) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            DOMString value(curP, endP - curP);
            value = khtml::parseURL(value);
            parsedValue = new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_URI);
        }
        break;
    }
    case CSS_PROP_ELEVATION:            // <angle> | below | level | above | higher | lower
                                        // | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_BELOW || id == CSS_VAL_LEVEL || id == CSS_VAL_ABOVE ||
                id == CSS_VAL_HIGHER || id == CSS_VAL_LOWER) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        }
        parsedValue = parseUnit(curP, endP, ANGLE );
        break;
    }
    case CSS_PROP_SPEECH_RATE:          // <number> | x-slow | slow | medium | fast |
                                        // x-fast | faster | slower | inherit --AURAL--
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_X_SLOW || id == CSS_VAL_SLOW || id == CSS_VAL_MEDIUM ||
                id == CSS_VAL_FAST || id == CSS_VAL_X_FAST || id == CSS_VAL_FASTER ||
                id == CSS_VAL_SLOWER) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
                break;
            }
        } else {
          parsedValue = parseUnit(curP, endP, NUMBER );
        }
        break;
    }
    case CSS_PROP_VOLUME:               // <number> | <percentage> | silent | x-soft | soft |
                                        // medium | loud | x-loud | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_SILENT || id == CSS_VAL_X_SOFT || id == CSS_VAL_SOFT ||
                id == CSS_VAL_MEDIUM || id == CSS_VAL_X_LOUD || id == CSS_VAL_X_LOUD) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, PERCENT | NUMBER);
        }
        break;
    }
    case CSS_PROP_PITCH:                 // <frequency> | x-low | low | medium | high | x-high | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval) {
            int id = cssval->id;
            if (id == CSS_VAL_X_LOW || id == CSS_VAL_LOW || id == CSS_VAL_MEDIUM ||
                id == CSS_VAL_HIGH || id == CSS_VAL_X_HIGH ) {
                parsedValue = new CSSPrimitiveValueImpl(cssval->id);
            }
        } else {
            parsedValue = parseUnit(curP, endP, FREQUENCY);
        }
        break;
    }
    case CSS_PROP_SPEAK:                // normal | none | spell-out | inherit
    case CSS_PROP_SPEAK_HEADER:         // once | always | inherit
    case CSS_PROP_SPEAK_NUMERAL:        // digits | continuous | inherit
    case CSS_PROP_SPEAK_PUNCTUATION:    // code | none | inherit
    {
        const struct css_value *cssval = findValue(val, value.length());
        if (cssval)
        {
            parsedValue = new CSSPrimitiveValueImpl(cssval->id);
        }
        break;
    }
    case CSS_PROP_PITCH_RANGE:          // <number> | inherit
    case CSS_PROP_RICHNESS:             // <number> | inherit
    case CSS_PROP_STRESS:               // <number> | inherit
    {
        parsedValue = parseUnit(curP, endP, NUMBER);
        break;
    }
    default:
    {
        kdDebug( 6080 ) << "illegal property: " << val << endl;
    }
   }
   if ( parsedValue ) {
     setParsedValue( propId, parsedValue );
        return true;
   }
   return false;
}
#endif

void StyleBaseImpl::setParsedValue(int propId, const CSSValueImpl *parsedValue)
{
    QListIterator<CSSProperty> propIt(*m_propList);
    propIt.toLast(); // just remove the top one - not sure what should happen if we have multiple instances of the property
    while (propIt.current() && propIt.current()->m_id != propId)
        --propIt;
    if (propIt.current())
        m_propList->removeRef(propIt.current());

    CSSProperty *prop = new CSSProperty();
    prop->m_id = propId;
    prop->setValue((CSSValueImpl *) parsedValue);
    prop->m_bImportant = m_bImportant;

    m_propList->append(prop);
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "added property: " << propId << ", value: " << parsedValue->cssText().string() << endl;
#endif
}

bool StyleBaseImpl::parseShortHand(const QChar *curP, const QChar *endP, const int *properties, int num)
{
  /* We try to match as many properties as possible
   * We setup an array of booleans to mark which property has been found,
   * and we try to search for properties until it makes no longer any sense
   */
  bool isLast = false;
  bool foundAnything = false;
  bool fnd[num];
  for( int i = 0; i < num; i++ )
    fnd[i] = false;
#ifdef CSS_DEBUG
  kdDebug(6080) << "PSH: parsing \"" << QString(curP, endP - curP) << "\"" << endl;
#endif
  for (int j = 0; j < num; j++) {
    int propIndex = 0;
    if (fnd[propIndex] == false) { // We have not yet found such a property
      while (propIndex < num) {
        const QChar *nextP = getNext( curP, endP, isLast );
#ifdef CSS_DEBUG
        kdDebug(6080) << "PSH: parsing \"" << QString(curP, nextP - curP) << "\"" << endl;
#endif
        bool found = parseValue(curP, nextP, properties[propIndex]);
        if (found) {
          fnd[propIndex] = true;
          foundAnything = true;
          if (!isLast) {
            nextP++;
            curP = nextP;
          } else {
            // we've found a property for the last token, so bye
            return true;
          }
        }
#ifdef CSS_DEBUG
        for( int i = 0; i < num; i++ ) {
          kdDebug(6080) << "fnd[" << i << "] "<< fnd[i] << endl;
        }
#endif
        propIndex++;
      }
    }
  }
  return foundAnything;
}

bool StyleBaseImpl::parseBackground(const QChar *curP, const QChar *endP)
{
  // ### implement background position
  bool last = false;
  bool fnd[5];
  for( int i = 0; i < 5; i++ )
    fnd[i] = false;
  while(!last) {
    bool found = false;
    const QChar *nextP = getNext( curP, endP, last );
    if(!last && !fnd[0]) {
      bool l;
      const QChar *second = getNext( nextP+1, endP, l );
      if(!fnd[0]) {
        found = parseValue(curP, second, CSS_PROP_BACKGROUND_POSITION);
        if( found ) {
          fnd[0] = true;
          nextP = second;
        }
      }
    }
#ifdef CSS_DEBUG
    kdDebug(6080) << "parsing \"" << QString(curP, nextP - curP) << "\"" << endl;
#endif
    if(!found && !fnd[2]) {
      found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_COLOR);
      if( found ) {
        //kdDebug() << "color!!!" << endl;
        fnd[2] = true;
      }
    }
    if( !found ) {
      int id = -1;
      QString value(curP, nextP - curP);
      const struct css_value *cssval = findValue(value.latin1(), value.length());
      if (cssval)
        id = cssval->id;
      int prop = -1;
      switch(id) {
      case CSS_VAL_REPEAT:
      case CSS_VAL_REPEAT_X:
      case CSS_VAL_REPEAT_Y:
      case CSS_VAL_NO_REPEAT:
        prop = CSS_PROP_BACKGROUND_REPEAT;
        fnd[3] = true;
        found = true;
        break;
      case CSS_VAL_SCROLL:
      case CSS_VAL_FIXED:
        prop = CSS_PROP_BACKGROUND_ATTACHMENT;
        fnd[4] = true;
        found = true;
      case CSS_VAL_CENTER:
      case CSS_VAL_TOP:
      case CSS_VAL_BOTTOM:
      case CSS_VAL_LEFT:
      case CSS_VAL_RIGHT:
        // #### remove this, do background position correctly
        found = true;
        break;
      default:
        break;
      }
      if( id != -1 )
        found = parseValue(curP, nextP, prop);
    }
    if(!found && !fnd[1]) {
      found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_IMAGE);
      if( found ) {
        kdDebug( 6080 ) << "image!!!" << endl;
        fnd[1] = true;
      }
    }
    if(!found && !fnd[0]) {
      found = parseValue(curP, nextP, CSS_PROP_BACKGROUND_POSITION);
      if( found )
        fnd[0] = true;
    }
    if(!found) {
#ifdef CSS_DEBUG
      kdDebug(6080) << "invalid property" << endl;
#endif
      return false;
    }
    curP = nextP+1;
    if(curP >= endP) break;
  }
  return true;
}

QList<QChar> StyleBaseImpl::splitShorthandProperties(const QChar *curP, const QChar *endP)
{
    bool last = false;
    QList<QChar> list;
    while(!last) {
        const QChar *nextP = curP;
        while(!(nextP->isSpace())) {
            nextP++;
            if(nextP >= endP) {
                last = true;
                break;
            }
        }
        list.append(curP);
        list.append(nextP);
        while(nextP->isSpace()) { // skip over WS between tokens
            nextP++;
            curP = nextP;
            if(curP >= endP) {
                last = true;
                break;
            }
        }
    }
    return list;
}

#ifdef CSS_AURAL
// used for shorthand properties xxx{1,2}
bool StyleBaseImpl::parse2Values( const QChar *curP, const QChar *endP, const int *properties)
{
    QList<QChar> list = splitShorthandProperties(curP, endP);
    switch(list.count())
    {
    case 2:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        setParsedValue(properties[1], m_propList->last()->value());
        return true;
    case 4:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1])) return false;
        return true;
    default:
        return false;
    }
}
#endif

// used for shorthand properties xxx{1,4}
bool StyleBaseImpl::parse4Values( const QChar *curP, const QChar *endP, const int *properties)
{
    /* From the CSS 2 specs, 8.3
     * If there is only one value, it applies to all sides. If there are two values, the top and
     * bottom margins are set to the first value and the right and left margins are set to the second.
     * If there are three values, the top is set to the first value, the left and right are set to the
     * second, and the bottom is set to the third. If there are four values, they apply to the top,
     * right, bottom, and left, respectively.
     */

    QList<QChar> list = splitShorthandProperties(curP, endP);
    switch(list.count())
    {
    case 2:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        setParsedValue(properties[1], m_propList->last()->value());
        setParsedValue(properties[2], m_propList->last()->value());
        setParsedValue(properties[3], m_propList->last()->value());
        return true;
    case 4:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        setParsedValue(properties[2], m_propList->last()->value());
        if(!parseValue(list.at(2), list.at(3), properties[1])) return false;
        setParsedValue(properties[3], m_propList->last()->value());
        return true;
    case 6:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1])) return false;
        setParsedValue(properties[3], m_propList->last()->value());
        if(!parseValue(list.at(4), list.at(5), properties[2])) return false;
        return true;
    case 8:
        if(!parseValue(list.at(0), list.at(1), properties[0])) return false;
        if(!parseValue(list.at(2), list.at(3), properties[1])) return false;
        if(!parseValue(list.at(4), list.at(5), properties[2])) return false;
        if(!parseValue(list.at(6), list.at(7), properties[3])) return false;
        return true;
    default:
        return false;
    }
}

CSSValueImpl *
StyleBaseImpl::parseUnit(const QChar * curP, const QChar *endP, int allowedUnits)
{
    endP--;
    while(*endP == ' ' && endP > curP) endP--;
    const QChar *split = endP;
    // splt up number and unit
    while( (*split < '0' || *split > '9') && *split != '.' && split > curP)
        split--;
    split++;

    QString s(curP, split-curP);

    bool isInt = false;
    if(s.find('.') == -1) isInt = true;

    bool ok;
    float value = s.toFloat(&ok);
    if(!ok) return 0;

    if(split > endP) // no unit
    {
        if(allowedUnits & NUMBER)
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_NUMBER);

        if(allowedUnits & INTEGER && isInt) // ### DOM CSS doesn't seem to define something for integer
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_NUMBER);

        // ### according to the css specs only 0 is allowed without unit.
        // there are however too many web pages out there using CSS without units
        // cause ie and ns allow them. We do so if the document is not using a strict dtd
        if(allowedUnits & LENGTH  && (value == 0 || !strictParsing ))
            return new CSSPrimitiveValueImpl(value, CSSPrimitiveValue::CSS_PX);

        return 0;
    }

    CSSPrimitiveValue::UnitTypes type = CSSPrimitiveValue::CSS_UNKNOWN;
    int unit = 0;

    switch(split->lower().latin1())
    {
    case '%':
        type = CSSPrimitiveValue::CSS_PERCENTAGE;
        unit = PERCENT;
        break;
    case 'e':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_EMS;
            unit = LENGTH;
        }
        else if(split->latin1() == 'x' || split->latin1() == 'X')
        {
            type = CSSPrimitiveValue::CSS_EXS;
            unit = LENGTH;
        }
        break;
    case 'p':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'x' || split->latin1() == 'X')
        {
            type = CSSPrimitiveValue::CSS_PX;
            unit = LENGTH;
        }
        else if(split->latin1() == 't' || split->latin1() == 'T')
        {
            type = CSSPrimitiveValue::CSS_PT;
            unit = LENGTH;
        }
        else if(split->latin1() == 'c' || split->latin1() == 'C')
        {
            type = CSSPrimitiveValue::CSS_PC;
            unit = LENGTH;
        }
        break;
    case 'c':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_CM;
            unit = LENGTH;
        }
        break;
    case 'm':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'm' || split->latin1() == 'M')
        {
            type = CSSPrimitiveValue::CSS_MM;
            unit = LENGTH;
        }
        else if(split->latin1() == 's' || split->latin1() == 'S')
        {
            type = CSSPrimitiveValue::CSS_MS;
            unit = TIME;
        }
        break;
    case 'i':
        split++;
        if(split > endP) break;
        if(split->latin1() == 'n' || split->latin1() == 'N')
        {
            type = CSSPrimitiveValue::CSS_IN;
            unit = LENGTH;
        }
        break;
    case 'd':
        type = CSSPrimitiveValue::CSS_DEG;
        unit = ANGLE;
        break;
    case 'r':
        type = CSSPrimitiveValue::CSS_RAD;
        unit = ANGLE;
        break;
    case 'g':
        type = CSSPrimitiveValue::CSS_GRAD;
        unit = ANGLE;
        break;
    case 's':
        type = CSSPrimitiveValue::CSS_S;
        unit = TIME;
        break;
    case 'h':
        type = CSSPrimitiveValue::CSS_HZ;
        unit = FREQUENCY;
        break;
    case 'k':
        type = CSSPrimitiveValue::CSS_KHZ;
        unit = FREQUENCY;
        break;
    }

    if(unit & allowedUnits)
    {
        //kdDebug( 6080 ) << "found allowed number " << value << ", unit " << type << endl;
        return new CSSPrimitiveValueImpl(value, type);
    }

    return 0;
}

CSSStyleRuleImpl *
StyleBaseImpl::parseStyleRule(const QChar *&curP, const QChar *endP)
{
    //kdDebug( 6080 ) << "style rule is \'" << QString(curP, endP-curP) << "\'" << endl;

    const QChar *startP;
    QList<CSSSelector> *slist;
    QList<CSSProperty> *plist;

    startP = curP;
    curP = parseToChar(startP, endP, '{', false);
    if (!curP)
        return(0);
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "selector is \'" << QString(startP, curP-startP) << "\'" << endl;
#endif

    slist = parseSelector(startP, curP );

    curP++; // need to get past the '{' from above

    startP = curP;
    curP = parseToChar(startP, endP, '}', false);

    if (!curP)
    {
        delete slist;
        return(0);
    }
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "rules are \'" << QString(startP, curP-startP) << "\'" << endl;
#endif

    plist = parseProperties(startP, curP );

    curP++; // need to get past the '}' from above

    if (!plist || !slist)
    {
        // Useless rule
        delete slist;
        delete plist;
#ifdef CSS_DEBUG
        kdDebug( 6080 ) << "bad style rule" << endl;
#endif
        return 0;
    }

    // return the newly created rule
    CSSStyleRuleImpl *rule = new CSSStyleRuleImpl(this);
    CSSStyleDeclarationImpl *decl = new CSSStyleDeclarationImpl(rule, plist);

    rule->setSelector(slist);
    rule->setDeclaration(decl);
    // ### set selector and value
    return rule;
}

CSSRuleImpl *
StyleBaseImpl::parseRule(const QChar *&curP, const QChar *endP)
{
    const char *comment = "<!--";
    const QChar *startP;
    int count = 0;

    curP = parseSpace( curP, endP );
    startP = curP;

    // The code below ignores any occurances of
    // the beginning and/or the end of a html
    // comment tag
    while (startP && (startP < endP))
    {
       if(*startP == comment[count])
         count++;
       else
         break;
       if(count == 4)
       {
          curP = ++startP;
          break;
       }
       ++startP;
    }

    comment = "-->";
    while (startP && (startP < endP))
    {
       if(*startP == comment[count])
         count++;
       else
         break;
       if(count == 3)
       {
          curP = ++startP;
          break;
       }
       ++startP;
    }

    CSSRuleImpl *rule = 0;

    if(!curP) return 0;
#ifdef CSS_DEBUG
    kdDebug( 6080 ) << "parse rule: current = " << curP->latin1() << endl;
#endif

    if (*curP == '@' )
    {
        rule = parseAtRule(curP, endP);
    }
    else
    {
        rule = parseStyleRule(curP, endP);
        if( rule )
            hasInlinedDecl = true;  // set flag to true iff we have a valid inlined decl.
    }

    if(curP) curP++;
    return rule;
}

// remove comments, replace character escapes and simplify spacing
QString StyleBaseImpl::preprocess(const QString &str)
{
    QString processed;

    bool sq = false;
    bool dq = false;
    bool comment = false;
    bool firstChar = false;

    hasInlinedDecl = false; // reset the inilned decl. flag

    const QChar *ch = str.unicode();
    const QChar *last = str.unicode()+str.length();
    while(ch < last) {
        if ( !comment && !sq && *ch == '"' ) {
            dq = !dq;
            processed += *ch;
        } else if ( !comment && !dq && *ch == '\'' ) {
            dq = !dq;
            processed += *ch;
        } else if ( comment ) {
            if ( firstChar && *ch == '/' ) {
                comment = false;
                firstChar = false;
            } else if ( *ch == '*' )
                firstChar = true;
            else
                firstChar = false;
        } else if ( !sq && !dq ) {
            // check for comment
            if ( firstChar ) {
                if ( *ch == '*' ) {
                    comment = true;
                } else {
                    processed += '/';
                    processed += *ch;
                }
                firstChar = false;
            } else if ( *ch == '/' )
                firstChar = true;
            else if ( *ch == '}' ) {
                processed += *ch;
                processed += QChar(' ');
            } else
                processed += *ch;
        }
        else
            processed += *ch;
        ++ch;
    }

    processed += ' ';
    return processed;
}

// ------------------------------------------------------------------------------

StyleListImpl::~StyleListImpl()
{
    StyleBaseImpl *n;

    if(!m_lstChildren) return;

    for( n = m_lstChildren->first(); n != 0; n = m_lstChildren->next() )
    {
        n->setParent(0);
        if(n->deleteMe()) delete n;
    }
    delete m_lstChildren;
}

// --------------------------------------------------------------------------------

CSSSelector::CSSSelector(void)
: tag(0), tagHistory(0)
{
    attr = 0;
    match = None;
    relation = Descendant;
    nonCSSHint = false;
}

CSSSelector::~CSSSelector(void)
{
    if (tagHistory)
    {
        delete tagHistory;
    }
}

void CSSSelector::print(void)
{
    kdDebug( 6080 ) << "[Selector: tag = " <<       tag << ", attr = \"" << attr << "\", value = \"" << value.string().latin1() << "\" relation = " << (int)relation << endl;
    if ( tagHistory )
        tagHistory->print();
}

int CSSSelector::specificity()
{
    if ( nonCSSHint )
        return 0;

    int s = 0;
    if(tag != -1) s = 1;
    switch(match)
    {
    case Exact:
        if(attr == ATTR_ID)
        {
            s += 10000;
            break;
        }
    case Set:
    case List:
    case Hyphen:
    case Pseudo:
        s += 100;
    case None:
        break;
    }
    if(tagHistory)
        s += tagHistory->specificity();
    return s;
}
// ----------------------------------------------------------------------------

CSSProperty::~CSSProperty()
{
    if(m_value) m_value->deref();
}

void CSSProperty::setValue(CSSValueImpl *val)
{
    if(m_value) m_value->deref();
    m_value = val;
    if(m_value) m_value->ref();
}

CSSValueImpl *CSSProperty::value()
{
    return m_value;
}
