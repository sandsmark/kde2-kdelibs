/* ANSI-C code produced by gperf version 2.7 */
/* Command-line: gperf -a -L ANSI-C -E -C -c -o -t -k * -NfindTag -Hhash_tag -Wwordlist_tag -D -s 2 htmltags.gperf  */
/* This file is automatically generated from htmltags.in by maketags, do not edit */
/* Copyright 1999 Lars Knoll */
#include "htmltags.h"
struct tags {
    const char *name;
    int id;
};
/* maximum key range = 363, duplicates = 1 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash_tag (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364,  70,
       35,  40,  55,  25,  30, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364,   0,  95, 115,
       35,   0,  50,  45,  25,  30,   0,  25,  10,  75,
        5,   0, 105,  25,   5,   5,   0,  75,   0, 364,
        5,   5, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364, 364, 364, 364, 364,
      364, 364, 364, 364, 364, 364
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 10:
        hval += asso_values[(unsigned char)str[9]];
      case 9:
        hval += asso_values[(unsigned char)str[8]];
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct tags *
findTag (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 93,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 363
    };

  static const struct tags wordlist_tag[] =
    {
      {"a", ID_A},
      {"tt", ID_TT},
      {"s", ID_S},
      {"tr", ID_TR},
      {"var", ID_VAR},
      {"area", ID_AREA},
      {"ol", ID_OL},
      {"textarea", ID_TEXTAREA},
      {"style", ID_STYLE},
      {"q", ID_Q},
      {"th", ID_TH},
      {"i", ID_I},
      {"hr", ID_HR},
      {"dt", ID_DT},
      {"td", ID_TD},
      {"li", ID_LI},
      {"ins", ID_INS},
      {"title", ID_TITLE},
      {"dl", ID_DL},
      {"del", ID_DEL},
      {"h5", ID_H5},
      {"tfoot", ID_TFOOT},
      {"h6", ID_H6},
      {"font", ID_FONT},
      {"h2", ID_H2},
      {"head", ID_HEAD},
      {"thead", ID_THEAD},
      {"strong", ID_STRONG},
      {"h3", ID_H3},
      {"div", ID_DIV},
      {"strike", ID_STRIKE},
      {"dd", ID_DD},
      {"dir", ID_DIR},
      {"link", ID_LINK},
      {"u", ID_U},
      {"em", ID_EM},
      {"meta", ID_META},
      {"h4", ID_H4},
      {"ul", ID_UL},
      {"address", ID_ADDRESS},
      {"dfn", ID_DFN},
      {"b", ID_B},
      {"h1", ID_H1},
      {"legend", ID_LEGEND},
      {"br", ID_BR},
      {"base", ID_BASE},
      {"small", ID_SMALL},
      {"p", ID_P},
      {"table", ID_TABLE},
      {"pre", ID_PRE},
      {"html", ID_HTML},
      {"isindex", ID_ISINDEX},
      {"span", ID_SPAN},
      {"label", ID_LABEL},
      {"col", ID_COL},
      {"center", ID_CENTER},
      {"listing", ID_LISTING},
      {"bdo", ID_BDO},
      {"form", ID_FORM},
      {"frame", ID_FRAME},
      {"select", ID_SELECT},
      {"fieldset", ID_FIELDSET},
      {"body", ID_BODY},
      {"tbody", ID_TBODY},
      {"frameset", ID_FRAMESET},
      {"option", ID_OPTION},
      {"noframes", ID_NOFRAMES},
      {"cite", ID_CITE},
      {"img", ID_IMG},
      {"code", ID_CODE},
      {"plain", ID_PLAIN},
      {"kbd", ID_KBD},
      {"menu", ID_MENU},
      {"basefont", ID_BASEFONT},
      {"iframe", ID_IFRAME},
      {"big", ID_BIG},
      {"sub", ID_SUB},
      {"button", ID_BUTTON},
      {"map", ID_MAP},
      {"sup", ID_SUP},
      {"samp", ID_SAMP},
      {"param", ID_PARAM},
      {"abbr", ID_ABBR},
      {"acronym", ID_ACRONYM},
      {"object", ID_OBJECT},
      {"input", ID_INPUT},
      {"applet", ID_APPLET},
      {"caption", ID_CAPTION},
      {"script", ID_SCRIPT},
      {"noscript", ID_NOSCRIPT},
      {"optgroup", ID_OPTGROUP},
      {"blockquote", ID_BLOCKQUOTE},
      {"colgroup", ID_COLGROUP}
    };

  static const short lookup[] =
    {
        -1,    0,    1,   -1,   -1,   -1,    2,    3,
         4,    5,   -1,   -1,    6,   -1,   -1,   -1,
        -1,   -1,    7,   -1,   -1,   -1,   -1,   -1,
        -1,    8,    9,   10,   -1,   -1,   -1,   11,
        12,   -1,   -1,   -1,   -1, -132,  -80,   -2,
        -1,   -1,   15,   16,   -1,   17,   -1,   18,
        19,   -1,   -1,   -1,   20,   -1,   -1,   21,
        -1,   22,   -1,   23,   -1,   -1,   24,   -1,
        25,   26,   27,   28,   29,   -1,   -1,   30,
        31,   32,   33,   -1,   34,   35,   -1,   36,
        -1,   -1,   37,   -1,   -1,   -1,   -1,   38,
        -1,   -1,   -1,   -1,   39,   40,   -1,   -1,
        41,   42,   -1,   -1,   -1,   43,   44,   -1,
        45,   46,   47,   -1,   -1,   -1,   48,   -1,
        -1,   49,   50,   -1,   -1,   51,   -1,   52,
        53,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        54,   -1,   -1,   55,   56,   57,   58,   59,
        60,   -1,   61,   62,   63,   -1,   -1,   64,
        -1,   -1,   65,   -1,   66,   67,   -1,   -1,
        -1,   68,   69,   70,   -1,   -1,   71,   72,
        -1,   -1,   -1,   73,   -1,   -1,   74,   -1,
        -1,   -1,   -1,   -1,   -1,   75,   -1,   -1,
        -1,   -1,   76,   -1,   -1,   77,   -1,   78,
        -1,   -1,   -1,   -1,   79,   80,   81,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   82,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   83,   -1,   -1,   -1,
        84,   -1,   -1,   -1,   85,   -1,   -1,   -1,
        -1,   -1,   86,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   87,   -1,
        -1,   -1,   88,   -1,   -1,   -1,   -1,   -1,
        -1,   89,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   90,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   91,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   92
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_tag (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist_tag[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                return &wordlist_tag[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct tags *wordptr = &wordlist_tag[TOTAL_KEYWORDS + lookup[offset]];
              register const struct tags *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->name;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}


static const DOMString tagList[] = {
"",
    "A",
    "ABBR",
    "ACRONYM",
    "ADDRESS",
    "APPLET",
    "AREA",
    "B",
    "BASE",
    "BASEFONT",
    "BDO",
    "BIG",
    "BLOCKQUOTE",
    "BODY",
    "BR",
    "BUTTON",
    "CAPTION",
    "CENTER",
    "CITE",
    "CODE",
    "COL",
    "COLGROUP",
    "DD",
    "DEL",
    "DFN",
    "DIR",
    "DIV",
    "DL",
    "DT",
    "EM",
    "FIELDSET",
    "FONT",
    "FORM",
    "FRAME",
    "FRAMESET",
    "H1",
    "H2",
    "H3",
    "H4",
    "H5",
    "H6",
    "HEAD",
    "HR",
    "HTML",
    "I",
    "IFRAME",
    "IMG",
    "INPUT",
    "INS",
    "ISINDEX",
    "KBD",
    "LABEL",
    "LEGEND",
    "LI",
    "LINK",
    "LISTING",
    "MAP",
    "MENU",
    "META",
    "NOFRAMES",
    "NOSCRIPT",
    "OBJECT",
    "OL",
    "OPTGROUP",
    "OPTION",
    "P",
    "PARAM",
    "PLAIN",
    "PRE",
    "Q",
    "S",
    "SAMP",
    "SCRIPT",
    "SELECT",
    "SMALL",
    "SPAN",
    "STRIKE",
    "STRONG",
    "STYLE",
    "SUB",
    "SUP",
    "TABLE",
    "TBODY",
    "TD",
    "TEXTAREA",
    "TFOOT",
    "TH",
    "THEAD",
    "TITLE",
    "TR",
    "TT",
    "U",
    "UL",
    "VAR",
"TEXT",
"COMMENT",
    "/A",
    "/ABBR",
    "/ACRONYM",
    "/ADDRESS",
    "/APPLET",
    "/AREA",
    "/B",
    "/BASE",
    "/BASEFONT",
    "/BDO",
    "/BIG",
    "/BLOCKQUOTE",
    "/BODY",
    "/BR",
    "/BUTTON",
    "/CAPTION",
    "/CENTER",
    "/CITE",
    "/CODE",
    "/COL",
    "/COLGROUP",
    "/DD",
    "/DEL",
    "/DFN",
    "/DIR",
    "/DIV",
    "/DL",
    "/DT",
    "/EM",
    "/FIELDSET",
    "/FONT",
    "/FORM",
    "/FRAME",
    "/FRAMESET",
    "/H1",
    "/H2",
    "/H3",
    "/H4",
    "/H5",
    "/H6",
    "/HEAD",
    "/HR",
    "/HTML",
    "/I",
    "/IFRAME",
    "/IMG",
    "/INPUT",
    "/INS",
    "/ISINDEX",
    "/KBD",
    "/LABEL",
    "/LEGEND",
    "/LI",
    "/LINK",
    "/LISTING",
    "/MAP",
    "/MENU",
    "/META",
    "/NOFRAMES",
    "/NOSCRIPT",
    "/OBJECT",
    "/OL",
    "/OPTGROUP",
    "/OPTION",
    "/P",
    "/PARAM",
    "/PLAIN",
    "/PRE",
    "/Q",
    "/S",
    "/SAMP",
    "/SCRIPT",
    "/SELECT",
    "/SMALL",
    "/SPAN",
    "/STRIKE",
    "/STRONG",
    "/STYLE",
    "/SUB",
    "/SUP",
    "/TABLE",
    "/TBODY",
    "/TD",
    "/TEXTAREA",
    "/TFOOT",
    "/TH",
    "/THEAD",
    "/TITLE",
    "/TR",
    "/TT",
    "/U",
    "/UL",
    "/VAR",
    0
};
const DOMString &getTagName(unsigned short id)
{
    if(id > ID_CLOSE_TAG*2) return tagList[ID_CLOSE_TAG+1];
    return tagList[id];
};
