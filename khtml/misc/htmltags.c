/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -L ANSI-C -D -E -C -l -o -t -k '*' -NfindTag -Hhash_tag -Wwordlist_tag htmltags.gperf  */
/* This file is automatically generated from htmltags.in by maketags, do not edit */
/* Copyright 1999 Lars Knoll */
#include "htmltags.h"
struct tags {
    const char *name;
    int id;
};
/* maximum key range = 360, duplicates = 1 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_tag (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361,  65,
       70,  80,  30,  55,  60, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361,   0,  42, 110,
       35,   0,  65,   0,  20,  30,   0,  35,  10,  30,
        5,   0, 127,  20,   5,  75,   0,  85,   0, 361,
       25,  35, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361, 361, 361, 361, 361,
      361, 361, 361, 361, 361, 361
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
      TOTAL_KEYWORDS = 100,
      MIN_WORD_LENGTH = 1,
      MAX_WORD_LENGTH = 10,
      MIN_HASH_VALUE = 1,
      MAX_HASH_VALUE = 360
    };

  static const unsigned char lengthtable[] =
    {
       1,  2,  2,  3,  4,  2,  1,  2,  2,  1,  2,  4,  2,  2,
       8,  2,  1,  5,  2,  3,  2,  2,  5,  6,  5,  4,  5,  7,
       3,  4,  5,  5,  3,  5,  2,  3,  4,  3,  1,  2,  3,  2,
       4,  1,  2,  6,  2,  4,  2,  2,  4,  5,  3,  5,  3,  3,
       4,  5,  7,  4,  3,  4,  5,  6,  1,  5,  3,  6,  6,  4,
       6,  4,  6,  7,  6,  3,  5,  6,  5,  8,  3,  8,  7,  8,
       6,  3,  7,  4,  8,  7,  4,  5,  6,  7,  3, 10,  8,  8,
       6,  8
    };
  static const struct tags wordlist_tag[] =
    {
      {"a", ID_A},
      {"tt", ID_TT},
      {"tr", ID_TR},
      {"var", ID_VAR},
      {"area", ID_AREA},
      {"ol", ID_OL},
      {"q", ID_Q},
      {"th", ID_TH},
      {"hr", ID_HR},
      {"i", ID_I},
      {"em", ID_EM},
      {"meta", ID_META},
      {"dt", ID_DT},
      {"td", ID_TD},
      {"textarea", ID_TEXTAREA},
      {"li", ID_LI},
      {"b", ID_B},
      {"title", ID_TITLE},
      {"dl", ID_DL},
      {"del", ID_DEL},
      {"br", ID_BR},
      {"h4", ID_H4},
      {"layer", ID_LAYER},
      {"legend", ID_LEGEND},
      {"table", ID_TABLE},
      {"head", ID_HEAD},
      {"thead", ID_THEAD},
      {"nolayer", ID_NOLAYER},
      {"img", ID_IMG},
      {"html", ID_HTML},
      {"image", ID_IMG},
      {"label", ID_LABEL},
      {"div", ID_DIV},
      {"tfoot", ID_TFOOT},
      {"dd", ID_DD},
      {"dir", ID_DIR},
      {"font", ID_FONT},
      {"big", ID_BIG},
      {"s", ID_S},
      {"h5", ID_H5},
      {"bdo", ID_BDO},
      {"h6", ID_H6},
      {"link", ID_LINK},
      {"u", ID_U},
      {"h1", ID_H1},
      {"strong", ID_STRONG},
      {"h2", ID_H2},
      {"abbr", ID_ABBR},
      {"ul", ID_UL},
      {"h3", ID_H3},
      {"form", ID_FORM},
      {"frame", ID_FRAME},
      {"dfn", ID_DFN},
      {"embed", ID_EMBED},
      {"ins", ID_INS},
      {"kbd", ID_KBD},
      {"body", ID_BODY},
      {"tbody", ID_TBODY},
      {"noembed", ID_NOEMBED},
      {"base", ID_BASE},
      {"col", ID_COL},
      {"menu", ID_MENU},
      {"style", ID_STYLE},
      {"center", ID_CENTER},
      {"p", ID_P},
      {"small", ID_SMALL},
      {"pre", ID_PRE},
      {"iframe", ID_IFRAME},
      {"button", ID_BUTTON},
      {"cite", ID_CITE},
      {"anchor", ID_A},
      {"code", ID_CODE},
      {"strike", ID_STRIKE},
      {"listing", ID_LISTING},
      {"object", ID_OBJECT},
      {"map", ID_MAP},
      {"param", ID_PARAM},
      {"option", ID_OPTION},
      {"plain", ID_PLAIN},
      {"frameset", ID_FRAMESET},
      {"xmp", ID_PRE},
      {"noframes", ID_NOFRAMES},
      {"acronym", ID_ACRONYM},
      {"basefont", ID_BASEFONT},
      {"select", ID_SELECT},
      {"sub", ID_SUB},
      {"isindex", ID_ISINDEX},
      {"span", ID_SPAN},
      {"fieldset", ID_FIELDSET},
      {"address", ID_ADDRESS},
      {"samp", ID_SAMP},
      {"input", ID_INPUT},
      {"applet", ID_APPLET},
      {"caption", ID_CAPTION},
      {"sup", ID_SUP},
      {"blockquote", ID_BLOCKQUOTE},
      {"colgroup", ID_COLGROUP},
      {"optgroup", ID_OPTGROUP},
      {"script", ID_SCRIPT},
      {"noscript", ID_NOSCRIPT}
    };

  static const short lookup[] =
    {
        -1,    0,    1,   -1,   -1,   -1,   -1,    2,
         3,    4,   -1,   -1,    5,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,    6,    7,   -1,
        -1,   -1,   -1,    8,   -1,   -1,   -1,    9,
        10,   -1,   11,   -1,   -1, -140,   14,  -88,
        -2,   -1,   15,   16,   -1,   17,   -1,   18,
        19,   20,   -1,   -1,   21,   -1,   -1,   22,
        23,   24,   -1,   25,   26,   -1,   27,   28,
        29,   30,   -1,   31,   32,   -1,   33,   -1,
        34,   35,   36,   37,   38,   39,   -1,   -1,
        40,   -1,   41,   -1,   42,   -1,   43,   44,
        -1,   -1,   -1,   45,   46,   47,   -1,   -1,
        -1,   48,   -1,   -1,   -1,   -1,   49,   -1,
        50,   51,   -1,   -1,   52,   -1,   -1,   -1,
        53,   54,   -1,   55,   56,   57,   -1,   58,
        -1,   59,   -1,   60,   61,   62,   63,   -1,
        64,   -1,   65,   -1,   -1,   -1,   -1,   66,
        67,   -1,   68,   -1,   -1,   -1,   -1,   -1,
        69,   -1,   70,   -1,   -1,   71,   -1,   72,
        -1,   -1,   -1,   -1,   -1,   73,   74,   -1,
        75,   -1,   -1,   -1,   -1,   -1,   -1,   76,
        77,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   78,   -1,   -1,   -1,   -1,   -1,   79,
        -1,   80,   -1,   -1,   81,   -1,   -1,   -1,
        82,   -1,   -1,   83,   -1,   -1,   -1,   -1,
        -1,   84,   -1,   -1,   -1,   85,   -1,   86,
        -1,   -1,   -1,   87,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   88,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        89,   -1,   -1,   -1,   90,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   91,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   92,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   93,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   94,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        95,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   96,   -1,   -1,   -1,   -1,   -1,   -1,
        97,   98,   -1,   -1,   -1,   -1,   -1,   -1,
        99
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_tag (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              if (len == lengthtable[index])
                {
                  register const char *s = wordlist_tag[index].name;

                  if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
                    return &wordlist_tag[index];
                }
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const unsigned char *lengthptr = &lengthtable[TOTAL_KEYWORDS + lookup[offset]];
              register const struct tags *wordptr = &wordlist_tag[TOTAL_KEYWORDS + lookup[offset]];
              register const struct tags *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  if (len == *lengthptr)
                    {
                      register const char *s = wordptr->name;

                      if (*str == *s && !memcmp (str + 1, s + 1, len - 1))
                        return wordptr;
                    }
                  lengthptr++;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}


static const char * tagList[] = {
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
    "EMBED",
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
    "LAYER",
    "LEGEND",
    "LI",
    "LINK",
    "LISTING",
    "MAP",
    "MENU",
    "META",
    "NOEMBED",
    "NOFRAMES",
    "NOSCRIPT",
    "NOLAYER",
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
    "/EMBED",
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
    "/LAYER",
    "/LEGEND",
    "/LI",
    "/LINK",
    "/LISTING",
    "/MAP",
    "/MENU",
    "/META",
    "/NOEMBED",
    "/NOFRAMES",
    "/NOSCRIPT",
    "/NOLAYER",
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
DOMString getTagName(unsigned short id)
{
    if(id > ID_CLOSE_TAG*2) id = ID_CLOSE_TAG+1;
    return DOMString(tagList[id]);
};
