/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -L ANSI-C -E -C -c -o -t -k '*' -NfindAttr -Hhash_attr -Wwordlist_attr -D -s 2 htmlattrs.gperf  */
/* This file is automatically generated from
#htmlattrs.in by makeattrs, do not edit */
/* Copyright 1999 Lars Knoll */
#include "htmlattrs.h"
struct attrs {
    const char *name;
    int id;
};
/* maximum key range = 618, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash_attr (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627,   0, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627,  55,  25,   0,
      155,   0,  10, 110,   5,   0,   0,   5,   0, 145,
        0,   0,  20,   0,  40,  95,  25,   5, 100,  80,
        0,   0,  10, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627, 627, 627, 627, 627,
      627, 627, 627, 627, 627, 627
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 14:
        hval += asso_values[(unsigned char)str[13]];
      case 13:
        hval += asso_values[(unsigned char)str[12]];
      case 12:
        hval += asso_values[(unsigned char)str[11]];
      case 11:
        hval += asso_values[(unsigned char)str[10]];
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
const struct attrs *
findAttr (register const char *str, register unsigned int len)
{
  enum
    {
      TOTAL_KEYWORDS = 129,
      MIN_WORD_LENGTH = 2,
      MAX_WORD_LENGTH = 14,
      MIN_HASH_VALUE = 9,
      MAX_HASH_VALUE = 626
    };

  static const struct attrs wordlist_attr[] =
    {
      {"link", ATTR_LINK},
      {"onclick", ATTR_ONCLICK},
      {"cite", ATTR_CITE},
      {"onkeyup", ATTR_ONKEYUP},
      {"rel", ATTR_REL},
      {"color", ATTR_COLOR},
      {"type", ATTR_TYPE},
      {"enctype", ATTR_ENCTYPE},
      {"for", ATTR_FOR},
      {"text", ATTR_TEXT},
      {"title", ATTR_TITLE},
      {"object", ATTR_OBJECT},
      {"content", ATTR_CONTENT},
      {"href", ATTR_HREF},
      {"nohref", ATTR_NOHREF},
      {"alink", ATTR_ALINK},
      {"face", ATTR_FACE},
      {"onblur", ATTR_ONBLUR},
      {"profile", ATTR_PROFILE},
      {"plain", ATTR_PLAIN},
      {"alt", ATTR_ALT},
      {"label", ATTR_LABEL},
      {"action", ATTR_ACTION},
      {"unknown", ATTR_UNKNOWN},
      {"cols", ATTR_COLS},
      {"clear", ATTR_CLEAR},
      {"char", ATTR_CHAR},
      {"accept", ATTR_ACCEPT},
      {"size", ATTR_SIZE},
      {"vlink", ATTR_VLINK},
      {"onfocus", ATTR_ONFOCUS},
      {"scope", ATTR_SCOPE},
      {"style", ATTR_STYLE},
      {"charoff", ATTR_CHAROFF},
      {"onselect", ATTR_ONSELECT},
      {"src", ATTR_SRC},
      {"rev", ATTR_REV},
      {"rules", ATTR_RULES},
      {"abbr", ATTR_ABBR},
      {"height", ATTR_HEIGHT},
      {"noresize", ATTR_NORESIZE},
      {"axis", ATTR_AXIS},
      {"id", ATTR_ID},
      {"code", ATTR_CODE},
      {"value", ATTR_VALUE},
      {"onreset", ATTR_ONRESET},
      {"lang", ATTR_LANG},
      {"align", ATTR_ALIGN},
      {"checked", ATTR_CHECKED},
      {"span", ATTR_SPAN},
      {"colspan", ATTR_COLSPAN},
      {"onchange", ATTR_ONCHANGE},
      {"html", ATTR_HTML},
      {"shape", ATTR_SHAPE},
      {"hspace", ATTR_HSPACE},
      {"bgcolor", ATTR_BGCOLOR},
      {"pluginurl", ATTR_PLUGINURL},
      {"http-equiv", ATTR_HTTP_EQUIV},
      {"ondblclick", ATTR_ONDBLCLICK},
      {"dir", ATTR_DIR},
      {"wrap", ATTR_WRAP},
      {"nowrap", ATTR_NOWRAP},
      {"multiple", ATTR_MULTIPLE},
      {"name", ATTR_NAME},
      {"archive", ATTR_ARCHIVE},
      {"codetype", ATTR_CODETYPE},
      {"defer", ATTR_DEFER},
      {"valuetype", ATTR_VALUETYPE},
      {"onload", ATTR_ONLOAD},
      {"rows", ATTR_ROWS},
      {"onunload", ATTR_ONUNLOAD},
      {"charset", ATTR_CHARSET},
      {"hreflang", ATTR_HREFLANG},
      {"version", ATTR_VERSION},
      {"start", ATTR_START},
      {"onkeydown", ATTR_ONKEYDOWN},
      {"class", ATTR_CLASS},
      {"scheme", ATTR_SCHEME},
      {"compact", ATTR_COMPACT},
      {"scrolling", ATTR_SCROLLING},
      {"frame", ATTR_FRAME},
      {"prompt", ATTR_PROMPT},
      {"declare", ATTR_DECLARE},
      {"readonly", ATTR_READONLY},
      {"accesskey", ATTR_ACCESSKEY},
      {"target", ATTR_TARGET},
      {"onkeypress", ATTR_ONKEYPRESS},
      {"border", ATTR_BORDER},
      {"tabindex", ATTR_TABINDEX},
      {"width", ATTR_WIDTH},
      {"valign", ATTR_VALIGN},
      {"vspace", ATTR_VSPACE},
      {"onmouseup", ATTR_ONMOUSEUP},
      {"oversrc", ATTR_OVERSRC},
      {"selected", ATTR_SELECTED},
      {"onmouseout", ATTR_ONMOUSEOUT},
      {"cellspacing", ATTR_CELLSPACING},
      {"data", ATTR_DATA},
      {"coords", ATTR_COORDS},
      {"rowspan", ATTR_ROWSPAN},
      {"onsubmit", ATTR_ONSUBMIT},
      {"bordercolor", ATTR_BORDERCOLOR},
      {"noshade", ATTR_NOSHADE},
      {"ismap", ATTR_ISMAP},
      {"hidden", ATTR_HIDDEN},
      {"usemap", ATTR_USEMAP},
      {"pluginpage", ATTR_PLUGINPAGE},
      {"accept-charset", ATTR_ACCEPT_CHARSET},
      {"method", ATTR_METHOD},
      {"codebase", ATTR_CODEBASE},
      {"language", ATTR_LANGUAGE},
      {"maxlength", ATTR_MAXLENGTH},
      {"headers", ATTR_HEADERS},
      {"media", ATTR_MEDIA},
      {"standby", ATTR_STANDBY},
      {"longdesc", ATTR_LONGDESC},
      {"onmouseover", ATTR_ONMOUSEOVER},
      {"background", ATTR_BACKGROUND},
      {"classid", ATTR_CLASSID},
      {"datetime", ATTR_DATETIME},
      {"pluginspage", ATTR_PLUGINSPAGE},
      {"onmousedown", ATTR_ONMOUSEDOWN},
      {"summary", ATTR_SUMMARY},
      {"disabled", ATTR_DISABLED},
      {"onmousemove", ATTR_ONMOUSEMOVE},
      {"cellpadding", ATTR_CELLPADDING},
      {"marginheight", ATTR_MARGINHEIGHT},
      {"frameborder", ATTR_FRAMEBORDER},
      {"marginwidth", ATTR_MARGINWIDTH}
    };

  static const short lookup[] =
    {
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   0,
       -1,  -1,   1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,   2,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,   3,  -1,  -1,
       -1,  -1,  -1,   4,  -1,   5,  -1,  -1,  -1,   6,
       -1,  -1,   7,   8,   9,  10,  11,  12,  -1,  13,
       -1,  14,  -1,  -1,  -1,  15,  -1,  -1,  -1,  16,
       -1,  -1,  -1,  -1,  -1,  -1,  17,  18,  -1,  -1,
       19,  -1,  -1,  20,  -1,  21,  22,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  23,  -1,  24,
       25,  -1,  -1,  -1,  26,  -1,  27,  -1,  -1,  28,
       29,  -1,  -1,  -1,  -1,  -1,  -1,  30,  -1,  -1,
       31,  -1,  -1,  -1,  -1,  32,  -1,  33,  34,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  35,  -1,
       -1,  -1,  -1,  36,  -1,  37,  -1,  -1,  -1,  38,
       -1,  39,  -1,  40,  41,  -1,  -1,  42,  -1,  43,
       -1,  -1,  -1,  -1,  -1,  44,  -1,  45,  -1,  46,
       47,  -1,  48,  -1,  49,  -1,  -1,  50,  51,  52,
       53,  54,  55,  -1,  -1,  -1,  -1,  -1,  -1,  56,
       57,  -1,  -1,  -1,  -1,  58,  -1,  -1,  59,  60,
       -1,  61,  -1,  62,  63,  -1,  -1,  64,  65,  -1,
       66,  -1,  -1,  -1,  67,  -1,  68,  -1,  -1,  69,
       -1,  -1,  -1,  70,  -1,  -1,  -1,  71,  72,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  73,  -1,  -1,  74,  -1,  -1,  -1,  75,
       76,  77,  78,  -1,  79,  80,  81,  82,  83,  84,
       -1,  85,  -1,  -1,  -1,  86,  87,  -1,  88,  -1,
       89,  90,  -1,  -1,  -1,  -1,  91,  -1,  -1,  92,
       -1,  -1,  93,  94,  -1,  95,  -1,  -1,  -1,  -1,
       -1,  96,  -1,  -1,  97,  -1,  98,  99,  -1,  -1,
       -1,  -1,  -1, 100,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 101,  -1,  -1,  -1,  -1,  -1, 102,  -1,  -1,
      103, 104,  -1,  -1,  -1,  -1, 105,  -1,  -1,  -1,
      106,  -1,  -1,  -1, 107,  -1, 108,  -1, 109,  -1,
       -1,  -1,  -1, 110,  -1,  -1,  -1,  -1,  -1, 111,
       -1,  -1,  -1,  -1,  -1,  -1,  -1, 112,  -1,  -1,
      113,  -1, 114,  -1,  -1,  -1,  -1,  -1, 115,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 116,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1, 117,  -1, 118,  -1,  -1,
       -1,  -1,  -1, 119,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 120,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 121, 122, 123,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 124,  -1,  -1,  -1,  -1, 125, 126,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1, 127,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
       -1,  -1,  -1,  -1,  -1,  -1, 128
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash_attr (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist_attr[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist_attr[index];
            }
        }
    }
  return 0;
}


static const char *attrList[] = {
    "ABBR",
    "ACCEPT-CHARSET",
    "ACCEPT",
    "ACCESSKEY",
    "ACTION",
    "ALIGN",
    "ALINK",
    "ALT",
    "ARCHIVE",
    "AXIS",
    "BACKGROUND",
    "BGCOLOR",
    "BORDER",
    "BORDERCOLOR",
    "CELLPADDING",
    "CELLSPACING",
    "CHAR",
    "CHAROFF",
    "CHARSET",
    "CHECKED",
    "CITE",
    "CLASS",
    "CLASSID",
    "CLEAR",
    "CODE",
    "CODEBASE",
    "CODETYPE",
    "COLOR",
    "COLS",
    "COLSPAN",
    "COMPACT",
    "CONTENT",
    "COORDS",
    "DATA",
    "DATETIME",
    "DECLARE",
    "DEFER",
    "DIR",
    "DISABLED",
    "ENCTYPE",
    "FACE",
    "FOR",
    "FRAME",
    "FRAMEBORDER",
    "HEADERS",
    "HEIGHT",
    "HIDDEN",
    "HREF",
    "HREFLANG",
    "HSPACE",
    "HTML",
    "HTTP-EQUIV",
    "ID",
    "ISMAP",
    "LABEL",
    "LANG",
    "LANGUAGE",
    "LINK",
    "LONGDESC",
    "MARGINHEIGHT",
    "MARGINWIDTH",
    "MAXLENGTH",
    "MEDIA",
    "METHOD",
    "MULTIPLE",
    "NAME",
    "NOHREF",
    "NORESIZE",
    "NOSHADE",
    "NOWRAP",
    "OBJECT",
    "ONBLUR",
    "ONCHANGE",
    "ONCLICK",
    "ONDBLCLICK",
    "ONFOCUS",
    "ONKEYDOWN",
    "ONKEYPRESS",
    "ONKEYUP",
    "ONLOAD",
    "ONMOUSEDOWN",
    "ONMOUSEMOVE",
    "ONMOUSEOUT",
    "ONMOUSEOVER",
    "ONMOUSEUP",
    "ONRESET",
    "ONSELECT",
    "ONSUBMIT",
    "ONUNLOAD",
    "OVERSRC",
    "PLAIN",
    "PLUGINPAGE",
    "PLUGINSPAGE",
    "PLUGINURL",
    "PROFILE",
    "PROMPT",
    "READONLY",
    "REL",
    "REV",
    "ROWS",
    "ROWSPAN",
    "RULES",
    "SCHEME",
    "SCOPE",
    "SCROLLING",
    "SELECTED",
    "SHAPE",
    "SIZE",
    "SPAN",
    "SRC",
    "STANDBY",
    "START",
    "STYLE",
    "SUMMARY",
    "TABINDEX",
    "TARGET",
    "TEXT",
    "TITLE",
    "TYPE",
    "UNKNOWN",
    "USEMAP",
    "VALIGN",
    "VALUE",
    "VALUETYPE",
    "VERSION",
    "VLINK",
    "VSPACE",
    "WIDTH",
    "WRAP",
    0
};
DOMString getAttrName(unsigned short id)
{
    return attrList[id-1];
};
