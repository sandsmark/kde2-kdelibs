/* ANSI-C code produced by gperf version 2.7 */
/* Command-line: gperf -a -L ANSI-C -C -G -c -o -t -k * -NfindEntity -D -s 2 kentities.gperf  */
/*   This file is part of the KDE libraries
  
     Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
  
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
  
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
  
     You should have received a copy of the GNU Library General Public License
     along with this library; see the file COPYING.LIB.  If not, write to
     the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
     Boston, MA 02111-1307, USA.
  
  ----------------------------------------------------------------------------
  
    khtmlentities.gperf: input file to generate a hash table for entities
    khtmlentities.c: DO NOT EDIT! generated by the command
    "gperf -a -L "ANSI-C" -C -G -c -o -t -k '*' -NfindEntity -D -s 2 khtmlentities.gperf > khtmlentities.c"   
    from khtmlentities.gperf 

    $Id$     
*/  
struct entity {
    const char *name;
    int code;
};

#define TOTAL_KEYWORDS 255
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 2
#define MAX_HASH_VALUE 1331
/* maximum key range = 1330, duplicates = 1 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,    0,
        10,   15,   20, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332,  240,   20,   15,    0,  115,
      1332,    0,    0,  100, 1332,   10,    5,    0,   30,  110,
         0,    0,   10,    0,    0,   60, 1332, 1332,   15,   35,
        25, 1332, 1332, 1332, 1332, 1332, 1332,    0,  125,  125,
       270,    5,    0,  205,  235,   15,    0,    0,   70,   25,
       315,   25,   65,  365,    0,  455,   60,   10,   70,    5,
         5,   40,   15, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332, 1332,
      1332, 1332, 1332, 1332, 1332, 1332
    };
  register int hval = len;

  switch (hval)
    {
      default:
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

static const struct entity wordlist[] =
  {
    {"GT", 62},
    {"rarr", 0x2192},
    {"LT", 60},
    {"Mu", 0x039c},
    {"Tau", 0x03a4},
    {"uarr", 0x2191},
    {"Pi", 0x03a0},
    {"xi", 0x03be},
    {"zwj", 0x200d},
    {"or", 0x22a6},
    {"Xi", 0x039e},
    {"mu", 0x03bc},
    {"Nu", 0x039d},
    {"Prime", 0x2033},
    {"Gamma", 0x0393},
    {"eta", 0x03b7},
    {"para", 0x00b6},
    {"tau", 0x03c4},
    {"larr", 0x2190},
    {"le", 0x2264},
    {"real", 0x211c},
    {"pi", 0x03c0},
    {"zeta", 0x03b6},
    {"Beta", 0x0392},
    {"amp", 38},
    {"Zeta", 0x0396},
    {"lrm", 0x200e},
    {"rlm", 0x200f},
    {"weierp", 0x2118},
    {"iota", 0x03b9},
    {"uml", 0x00a8},
    {"auml", 0x00e4},
    {"loz", 0x25ca},
    {"euml", 0x00eb},
    {"prime", 0x2032},
    {"ETH", 0x00d0},
    {"uuml", 0x00fc},
    {"iuml", 0x00ef},
    {"rfloor", 0x230b},
    {"part", 0x2202},
    {"crarr", 0x21b5},
    {"lt", 60},
    {"ouml", 0x00f6},
    {"kappa", 0x03ba},
    {"perp", 0x22a5},
    {"Delta", 0x0394},
    {"frac12", 0x00bd},
    {"Yuml", 0x0178},
    {"Kappa", 0x039a},
    {"yuml", 0x00ff},
    {"frac14", 0x00bc},
    {"piv", 0x03d6},
    {"macr", 0x00af},
    {"THORN", 0x00de},
    {"prop", 0x221d},
    {"frac34", 0x00be},
    {"Uuml", 0x00dc},
    {"forall", 0x2200},
    {"QUOT", 34},
    {"Eta", 0x0397},
    {"permil", 0x2030},
    {"Iota", 0x0399},
    {"cap", 0x2229},
    {"beta", 0x03b2},
    {"micro", 0x00b5},
    {"lfloor", 0x230a},
    {"empty", 0x2205},
    {"cup", 0x222a},
    {"acute", 0x00b4},
    {"aacute", 0x00e1},
    {"Iuml", 0x00cf},
    {"eacute", 0x00e9},
    {"ge", 0x2265},
    {"reg", 0x00ae},
    {"uacute", 0x00fa},
    {"Ouml", 0x00d6},
    {"rceil", 0x2309},
    {"iacute", 0x00ed},
    {"Euml", 0x00cb},
    {"iexcl", 0x00a1},
    {"oacute", 0x00f3},
    {"harr", 0x2194},
    {"Yacute", 0x00dd},
    {"AMP", 38},
    {"rArr", 0x21d2},
    {"yacute", 0x00fd},
    {"Sigma", 0x03a3},
    {"Phi", 0x03a6},
    {"uArr", 0x21d1},
    {"image", 0x2111},
    {"copy", 0x00a9},
    {"gamma", 0x03b3},
    {"rho", 0x03c1},
    {"omega", 0x03c9},
    {"Uacute", 0x00da},
    {"gt", 62},
    {"Chi", 0x03a7},
    {"circ", 0x02c6},
    {"acirc", 0x00e2},
    {"Rho", 0x03a1},
    {"darr", 0x2193},
    {"ecirc", 0x00ea},
    {"bull", 0x2022},
    {"ucirc", 0x00fb},
    {"icirc", 0x00ee},
    {"agrave", 0x00e0},
    {"lceil", 0x2308},
    {"egrave", 0x00e8},
    {"ocirc", 0x00f4},
    {"ugrave", 0x00f9},
    {"ordf", 0x00aa},
    {"aelig", 0x00e6},
    {"igrave", 0x00ec},
    {"eth", 0x00f0},
    {"Theta", 0x0398},
    {"Iacute", 0x00cd},
    {"ograve", 0x00f2},
    {"lArr", 0x21d0},
    {"Oacute", 0x00d3},
    {"phi", 0x03c6},
    {"Eacute", 0x00c9},
    {"ne", 0x2260},
    {"ordm", 0x00ba},
    {"oelig", 0x0153},
    {"brvbar", 0x00a6},
    {"nu", 0x03bd},
    {"Ucirc", 0x00db},
    {"there4", 0x2234},
    {"ni", 0x220b},
    {"zwnj", 0x200c},
    {"trade", 0x2122},
    {"fnof", 0x0192},
    {"Ugrave", 0x00d9},
    {"Auml", 0x00c4},
    {"Omega", 0x03a9},
    {"yen", 0x00a5},
    {"prod", 0x220f},
    {"theta", 0x03b8},
    {"Icirc", 0x00ce},
    {"alpha", 0x03b1},
    {"chi", 0x03c7},
    {"Ocirc", 0x00d4},
    {"Ecirc", 0x00ca},
    {"Igrave", 0x00cc},
    {"int", 0x222b},
    {"Ograve", 0x00d2},
    {"Egrave", 0x00c8},
    {"not", 0x00ac},
    {"raquo", 0x00bb},
    {"delta", 0x03b4},
    {"radic", 0x221a},
    {"Dagger", 0x2021},
    {"tilde", 0x02dc},
    {"atilde", 0x00e3},
    {"Lambda", 0x039b},
    {"oline", 0x203e},
    {"Aacute", 0x00c1},
    {"otilde", 0x00f5},
    {"Ntilde", 0x00d1},
    {"curren", 0x00a4},
    {"quot", 34},
    {"hellip", 0x2026},
    {"equiv", 0x2261},
    {"Scaron", 0x0160},
    {"Psi", 0x03a8},
    {"laquo", 0x00ab},
    {"hArr", 0x21d4},
    {"deg", 0x00b0},
    {"cedil", 0x00b8},
    {"sum", 0x2211},
    {"lambda", 0x03bb},
    {"sim", 0x223c},
    {"Ccedil", 0x00c7},
    {"cent", 0x00a2},
    {"Acirc", 0x00c2},
    {"dArr", 0x21d3},
    {"nabla", 0x2207},
    {"OElig", 0x0152},
    {"ang", 0x2220},
    {"rang", 0x232a},
    {"Agrave", 0x00c0},
    {"frasl", 0x2044},
    {"sup", 0x2283},
    {"sup1", 0x00b9},
    {"Otilde", 0x00d5},
    {"omicron", 0x03bf},
    {"psi", 0x03c8},
    {"supe", 0x2287},
    {"aring", 0x00e5},
    {"sup2", 0x00b2},
    {"exist", 0x2203},
    {"sup3", 0x00b3},
    {"emsp", 0x2003},
    {"times", 0x00d7},
    {"and", 0x22a5},
    {"asymp", 0x2248},
    {"otimes", 0x2297},
    {"sub", 0x2282},
    {"lang", 0x2329},
    {"sube", 0x2286},
    {"alefsym", 0x2135},
    {"Alpha", 0x0391},
    {"ccedil", 0x00e7},
    {"lowast", 0x2217},
    {"Omicron", 0x039f},
    {"oplus", 0x2295},
    {"thorn", 0x00fe},
    {"sect", 0x00a7},
    {"AElig", 0x00c6},
    {"divide", 0x00f7},
    {"infin", 0x221e},
    {"Atilde", 0x00c3},
    {"middot", 0x00b7},
    {"cong", 0x2245},
    {"rdquo", 0x201d},
    {"pound", 0x00a3},
    {"dagger", 0x2020},
    {"sigma", 0x03c3},
    {"sigmaf", 0x03c2},
    {"shy", 0x00ad},
    {"notin", 0x2209},
    {"ntilde", 0x00f1},
    {"ldquo", 0x201c},
    {"hearts", 0x2665},
    {"szlig", 0x00df},
    {"diams", 0x2666},
    {"Aring", 0x00c5},
    {"upsih", 0x03d2},
    {"clubs", 0x2663},
    {"bdquo", 0x201e},
    {"isin", 0x2208},
    {"sdot", 0x22c5},
    {"minus", 0x2212},
    {"ensp", 0x2002},
    {"rsquo", 0x2019},
    {"rsaquo", 0x203a},
    {"thetasym", 0x03d1},
    {"nsub", 0x2284},
    {"iquest", 0x00bf},
    {"scaron", 0x0161},
    {"lsquo", 0x2018},
    {"lsaquo", 0x2039},
    {"plusmn", 0x00b1},
    {"epsilon", 0x03b5},
    {"upsilon", 0x03c5},
    {"nbsp", 0x00a0},
    {"sbquo", 0x201a},
    {"mdash", 0x2014},
    {"Upsilon", 0x03a5},
    {"Epsilon", 0x0395},
    {"thinsp", 0x2009},
    {"oslash", 0x00f8},
    {"spades", 0x2660},
    {"ndash", 0x2013},
    {"Oslash", 0x00d8}
  };

static const short lookup[] =
  {
      -1,   -1,    0,   -1,    1,   -1,   -1,    2,
      -1,   -1,   -1,   -1,    3,    4,    5,   -1,
      -1,    6,   -1,   -1,   -1,   -1,    7,    8,
      -1,   -1,   -1,    9,   -1,   -1,   -1,   -1,
      10,   -1,   -1,   -1,   -1,   11,   -1,   -1,
      -1,   -1,   12,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   13,   -1,   -1,   -1,   -1,   14,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   15,   16,   -1,   -1,
      -1,   17,   18,   -1,   -1,   19,   -1,   20,
      -1,   -1,   21,   -1,   22,   -1,   -1,   -1,
      -1,   23,   -1,   -1,   -1,   24,   25,   -1,
      -1,   -1, -355, -229,   -2,   28,   -1,   -1,
      29,   -1,   -1,   -1,   30,   31,   -1,   -1,
      -1,   32,   33,   34,   -1,   -1,   35,   36,
      -1,   -1,   -1,   -1,   37,   -1,   38,   -1,
      -1,   39,   40,   -1,   41,   -1,   42,   43,
      -1,   -1,   -1,   44,   45,   46,   -1,   -1,
      47,   48,   -1,   -1,   -1,   49,   -1,   50,
      -1,   51,   52,   53,   -1,   -1,   -1,   54,
      -1,   -1,   -1,   -1,   -1,   -1,   55,   -1,
      -1,   56,   -1,   57,   -1,   -1,   58,   -1,
      -1,   -1,   59,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   60,   -1,   -1,   61,   -1,   -1,
      -1,   62,   63,   64,   65,   -1,   -1,   -1,
      66,   -1,   -1,   67,   -1,   68,   69,   -1,
      -1,   70,   -1,   71,   72,   73,   -1,   -1,
      74,   -1,   -1,   75,   76,   77,   -1,   -1,
      78,   79,   -1,   -1,   -1,   -1,   -1,   80,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   81,
      -1,   82,   -1,   83,   84,   -1,   85,   -1,
      -1,   -1,   86,   -1,   -1,   87,   88,   89,
      -1,   -1,   -1,   90,   91,   -1,   -1,   92,
      -1,   93,   94,   95,   96,   97,   98,   -1,
      -1,   99,  100,  101,   -1,   -1,   -1,  102,
     103,   -1,   -1,   -1,   -1,  104,  105,   -1,
      -1,   -1,  106,  107,   -1,   -1,   -1,  108,
     109,   -1,   -1,  110,  111,  112,   -1,  113,
      -1,  114,  115,   -1,   -1,   -1,   -1,  116,
      -1,   -1,  117,   -1,  118,   -1,  119,   -1,
      -1,  120,  121,   -1,  122,  123,  124,  125,
      -1,   -1,  126,  127,  128,   -1,   -1,   -1,
      -1,   -1,   -1,  129,  130,   -1,   -1,   -1,
     131,   -1,  132,   -1,   -1,  133,  134,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  135,  136,  137,   -1,   -1,
      -1,   -1,  138,   -1,   -1,   -1,   -1,  139,
      -1,   -1,  140,   -1,  141,   -1,   -1,   -1,
      -1,  142,  143,   -1,   -1,   -1,   -1,   -1,
      -1,  144,   -1,   -1,  145,   -1,   -1,   -1,
      -1,  146,   -1,  147,   -1,  148,   -1,   -1,
      -1,   -1,  149,   -1,   -1,   -1,   -1,  150,
      -1,   -1,   -1,   -1,   -1,  151,   -1,   -1,
      -1,  152,  153,   -1,   -1,   -1,   -1,  154,
      -1,   -1,   -1,  155,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  156,   -1,
      -1,   -1,   -1,  157,   -1,   -1,   -1,   -1,
     158,   -1,   -1,   -1,   -1,  159,   -1,   -1,
     160,   -1,  161,   -1,   -1,   -1,  162,  163,
      -1,  164,   -1,  165,   -1,   -1,   -1,  166,
      -1,   -1,   -1,  167,   -1,   -1,   -1,   -1,
      -1,   -1,  168,   -1,   -1,  169,   -1,   -1,
     170,   -1,  171,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  172,   -1,   -1,  173,  174,   -1,
      -1,   -1,  175,  176,   -1,   -1,   -1,   -1,
     177,   -1,   -1,  178,  179,   -1,  180,   -1,
      -1,   -1,  181,   -1,   -1,  182,  183,   -1,
     184,  185,  186,  187,  188,   -1,   -1,   -1,
     189,  190,   -1,   -1,   -1,  191,   -1,   -1,
      -1,   -1,  192,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  193,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  194,   -1,  195,  196,
      -1,  197,  198,   -1,   -1,   -1,   -1,  199,
      -1,   -1,  200,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  201,
     202,   -1,   -1,   -1,   -1,  203,  204,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  205,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     206,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  207,  208,  209,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  210,  211,   -1,   -1,   -1,   -1,  212,
      -1,   -1,  213,  214,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  215,  216,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  217,  218,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  219,   -1,  220,
      -1,   -1,   -1,   -1,   -1,  221,   -1,   -1,
      -1,  222,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  223,   -1,   -1,   -1,  224,   -1,   -1,
      -1,   -1,  225,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  226,   -1,   -1,   -1,
      -1,  227,   -1,   -1,   -1,   -1,  228,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     229,   -1,   -1,   -1,  230,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  231,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  232,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  233,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  234,  235,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     236,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  237,   -1,   -1,
      -1,   -1,   -1,   -1,  238,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  239,   -1,
      -1,   -1,  240,  241,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  242,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  243,   -1,   -1,
      -1,   -1,  244,   -1,  245,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  246,   -1,   -1,   -1,   -1,  247,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  248,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  249,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  250,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  251,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     252,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     253,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  254
  };

#ifdef __GNUC__
__inline
#endif
const struct entity *
findEntity (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1))
                return &wordlist[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct entity *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register const struct entity *wordendptr = wordptr + -lookup[offset + 1];

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

