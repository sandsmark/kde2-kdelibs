%{

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <qstring.h>

#define AMP_ENTITY "&amp;"
#define YYERROR_VERBOSE

extern int yylex();

// extern QString idl_lexFile;
extern int idl_line_no;

static int dcop_area = 0;

void dcopidlInitFlex( const char *_code );

void yyerror( const char *s )
{
	qDebug( "In line %i : %s", idl_line_no, s );
        exit(1);
	//   theParser->parse_error( idl_lexFile, s, idl_line_no );
}

%}


%union
{
  long   _int;
  QString        *_str;
  ushort          _char;
  double _float;
}

%token <_char> T_CHARACTER_LITERAL
%token <_float> T_DOUBLE_LITERAL
%token <_str> T_IDENTIFIER
%token <_int> T_INTEGER_LITERAL
%token <_str> T_STRING_LITERAL
%token <_str> T_INCLUDE
%token T_CLASS
%token T_STRUCT
%token T_LEFT_CURLY_BRACKET
%token T_LEFT_PARANTHESIS
%token T_RIGHT_CURLY_BRACKET
%token T_RIGHT_PARANTHESIS
%token T_COLON
%token T_SEMICOLON
%token T_PUBLIC
%token T_PROTECTED
%token T_PRIVATE
%token T_VIRTUAL
%token T_CONST
%token T_RETURN
%token T_SIGNAL
%token T_SLOT
%token T_TYPEDEF
%token T_COMMA
%token T_ASTERISK
%token T_TILDE
%token T_LESS
%token T_GREATER
%token T_AMPERSAND
%token T_ACCESS
%token T_ENUM
%token T_UNKNOWN
%token T_TRUE
%token T_FALSE
%token T_STATIC
%token T_EQUAL
%token T_SCOPE
%token T_NULL
%token T_DCOP
%token T_DCOP_AREA
%token T_SIGNED
%token T_UNSIGNED

%type <_str> body
%type <_str> class_header
%type <_str> super_classes
%type <_str> super_class
%type <_str> super_class_name
%type <_str> typedef
%type <_str> typedef_params
%type <_str> function
%type <_str> param
%type <_str> params
%type <_str> return
%type <_str> return_params
%type <_str> qualifier
%type <_str> prequalifier
%type <_str> Identifier

%%

/*1*/
main
	: includes rest
	  {
	     dcop_area = 0; // reset
	  }
	;
	
includes
	: T_INCLUDE includes
          {
		printf("<INCLUDE file=\"%s\"/>\n", $1->latin1() );
	  }
        | /* empty */
          {
          }
        ;

rest
	: T_CLASS Identifier class_header T_DCOP body T_SEMICOLON rest
	  {
		printf("<CLASS name=\"%s\">\n%s%s</CLASS>\n", $2->latin1(), $3->latin1(), $5->latin1() );
	  }
	| T_CLASS Identifier class_header body T_SEMICOLON rest
	  {
	  }
	| T_CLASS Identifier T_SEMICOLON main
	  {
	  }
	| T_STRUCT Identifier T_SEMICOLON main
	  {
	  }
	| {}
	;

bool_value: T_TRUE | T_FALSE;

nodcop_area: T_PRIVATE | T_PROTECTED | T_PUBLIC ;

sigslot: T_SIGNAL | T_SLOT | ;

nodcop_area_begin
	: nodcop_area sigslot T_COLON
        {
	  dcop_area = 0;
	}

dcop_area_begin
	: T_DCOP_AREA T_COLON
	{
	  dcop_area = 1;
	}

Identifier
	: T_IDENTIFIER {
	  $$ = $1;
	}
	| T_IDENTIFIER T_SCOPE Identifier {
	   $$ = new QString( *($1) + *($3) );
	}
	;
		
super_class_name
	: Identifier
	  {
		QString* tmp = new QString( "<SUPER name=\"%1\"/>\n" );
		*tmp = tmp->arg( *($1) );
		$$ = tmp;
	  }
	;


super_class
	: T_VIRTUAL T_PUBLIC super_class_name
	  {
		$$ = $3;
	  }
	| T_PUBLIC super_class_name
	  {
		$$ = $2;
	  }
	| super_class_name
	  {
		$$ = $1;
	  }
	;

super_classes
	: super_class T_LEFT_CURLY_BRACKET
	  {
		$$ = $1;
	  }
	| super_class T_COMMA super_classes
	  {
		/* $$ = $1; */
		$$ = new QString( *($1) + *($3) );
	  }
	;

class_header
	: T_COLON super_classes
	  {
		$$ = $2;
	  }
	| T_LEFT_CURLY_BRACKET
	  {
		$$ = new QString( "" );
	  }
	;

body
	: T_RIGHT_CURLY_BRACKET
	  {
		$$ = new QString( "" );
	  }
	| typedef body
	  {
		$$ = new QString( *($1) + *($2) );
	  }
	| function body
	  {
		$$ = new QString( *($1) + *($2) );
	  }
	| dcop_area_begin body
	  {
		$$ = $2;
	  }
	| nodcop_area_begin body
	  {	
	        $$ = $2;
	  }
	| member body {
 	        $$ = $2;
	}
	;

typedef
	: T_TYPEDEF Identifier T_LESS typedef_params T_GREATER Identifier T_SEMICOLON
	  {
		if (dcop_area) {
 		  QString* tmp = new QString("<TYPEDEF name=\"%1\" template=\"%2\">%3</TYPEDEF>\n");
		  *tmp = tmp->arg( *($6) ).arg( *($2) ).arg( *($4) );
		  $$ = tmp;
		} else {
		  $$ = new QString("");
		}
	  }
	;

typedef_params
	: Identifier
	  {
		QString* tmp = new QString("<PARAM type=\"%1\"/>");
		*tmp = tmp->arg( *($1) );
		$$ = tmp;
	  }
	| Identifier T_ASTERISK
	  {
		if (dcop_area)
		  yyerror("pointers are not allowed in dcop area!");
	  }
	| Identifier T_COMMA typedef_params
	  {
		QString* tmp = new QString("<PARAM type=\"%1\"/>%2");
		*tmp = tmp->arg( *($1) ).arg( *($3) );
		$$ = tmp;
	  }
	| Identifier T_ASTERISK T_COMMA typedef_params
	  {
		if (dcop_area)
		  yyerror("pointers are not allowed in dcop area!");
	  }
	;

qualifier
	: /* empty */
	  {
		$$ = new QString( "" );
	  }
	| T_CONST
	  {
		$$ = new QString( "const" );
	  }
	;

return_params
	: Identifier
	  {
		$$ = $1;
	  }
	| Identifier T_COMMA return_params
	  {
		$$ = new QString( *($1) + "," + *($3) );
	  }
	;

prequalifier
	: T_SIGNED { $$ = new QString("signed"); }
	| T_UNSIGNED { $$ = new QString("unsigned"); }

return
	: Identifier
	  {
		QString* tmp = new QString("<RET type=\"%1\"/>");
		*tmp = tmp->arg( *($1) );
		$$ = tmp;		
	  }
	| prequalifier Identifier
	  {
		QString* tmp = new QString("<RET type=\"%1%2\"/>");
		*tmp = tmp->arg( *($1) ).arg( *($2) );
		$$ = tmp;		
	  }
	| T_CONST Identifier T_AMPERSAND
	  {
		QString* tmp = new QString("<RET type=\"%1\" qleft=\"const\" qright=\"" AMP_ENTITY "\"/>");
		*tmp = tmp->arg( *($2) );
		$$ = tmp;		
	  }
	| Identifier T_LESS return_params T_GREATER
	  {
		QString* tmp = new QString("<RET type=\"%1<%2>\"/>");
		*tmp = tmp->arg( *($1) ).arg( *($3) );
		$$ = tmp;		
	  }
	| T_CONST Identifier T_LESS return_params T_GREATER T_AMPERSAND
	  {
		QString* tmp = new QString("<RET type=\"%1<%2>\" qleft=\"const\" qright=\"" AMP_ENTITY "\"/>");
		*tmp = tmp->arg( *($2) ).arg( *($4) );
		$$ = tmp;		
	  }
	| pointer_type
	  {
	 	if (dcop_area)
	           yyerror("pointers are not allowed in kdcop areas!");
	  }
	;

pointer_type
	: T_CONST Identifier T_LESS return_params T_GREATER T_ASTERISK {}
	| Identifier T_LESS return_params T_GREATER T_ASTERISK {}
	| Identifier T_ASTERISK {}
	| T_CONST Identifier T_ASTERISK {}
	| Identifier T_LESS return_params T_ASTERISK T_GREATER T_ASTERISK {}
	| Identifier T_LESS return_params T_ASTERISK T_GREATER {}
	;

params
	: /* empty */
	  {
		$$ = new QString( "" );
	  }
	| param
	  {
		$$ = $1;
	  }
	| params T_COMMA param
	  {
		$$ = new QString( *($1) + *($3) );
	  }
	;

param
	: T_CONST Identifier T_AMPERSAND Identifier default
	  {
		QString* tmp = new QString("<ARG name=\"%1\" type=\"%2\" qleft=\"const\" qright=\"" AMP_ENTITY "\"/>");
		*tmp = tmp->arg( *($4) );
		*tmp = tmp->arg( *($2) );
		$$ = tmp;		
	  }
	| Identifier default
	  {
		if (dcop_area) {
		  yyerror("in dcoparea you have to specify paramater names!");
		}
		$$ = 0;
	  }
	| Identifier Identifier default
	  {
		QString* tmp = new QString("<ARG name=\"%1\" type=\"%2\"/>");
		*tmp = tmp->arg( *($2) ).arg( *($1) );
		$$ = tmp;		
	  }
	| Identifier T_LESS return_params T_GREATER Identifier
	  {
		QString* tmp = new QString("<ARG name=\"%1\" type=\"%2<%3>\"/>");
		*tmp = tmp->arg( *($5) ).arg( *($1) ).arg( *($3) );
		$$ = tmp;		
	  }
	| Identifier T_LESS return_params T_GREATER T_AMPERSAND Identifier
	  {
		QString* tmp = new QString("<ARG name=\"%1\" type=\"%2<%3>\" qright=\"" AMP_ENTITY "\"/>");
		*tmp = tmp->arg( *($6) ).arg( *($1) ).arg( *($3) );
		$$ = tmp;		
	  }
	| T_CONST Identifier T_LESS return_params T_GREATER T_AMPERSAND Identifier
	  {
		QString* tmp = new QString("<ARG name=\"%1\" type=\"%1<%2>\" qleft=\"const\" qright=\"" AMP_ENTITY "\"/>");
		*tmp = tmp->arg( *($7) ).arg( *($2) ).arg( *($4) );
		$$ = tmp;		
	  }
	| pointer_type Identifier default {
	       if (dcop_area)
	           yyerror("pointers are not allowed in kdcop areas!");
	       $$ = new QString("");
	}
	;

default
	: /* empty */
	  {
	  }
	| T_EQUAL T_STRING_LITERAL
	  {
	  }
	| T_EQUAL T_CHARACTER_LITERAL
	  {
	  }
	| T_EQUAL T_DOUBLE_LITERAL
	  {
	  }
	| T_EQUAL T_INTEGER_LITERAL
	  {
	  }
	| T_EQUAL T_NULL
	  {
	  }
	| T_EQUAL Identifier
	  {
	  }
	| T_EQUAL bool_value
	;

function
	: T_VIRTUAL return Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS qualifier T_EQUAL T_NULL function_body
	  {
	     if (dcop_area) {
		QString* tmp = new QString("<FUNC name=\"%1\" qual=\"%4\">%2%3</FUNC>\n");
		*tmp = tmp->arg( *($3) );
		*tmp = tmp->arg( *($2) );
		*tmp = tmp->arg( *($5) );
		*tmp = tmp->arg( *($7) );
		$$ = tmp;
   	     } else
	        $$ = new QString("");
	  }
	| Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS function_body
	  {
	      /* The constructor */
	      assert(!dcop_area);
              $$ = new QString("");
	  }
	| T_TILDE Identifier T_LEFT_PARANTHESIS T_RIGHT_PARANTHESIS function_body
	  {
	      /* The destructor */
  	      assert(!dcop_area);
              $$ = new QString("");
	  }
	|  return Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS qualifier T_EQUAL T_NULL function_body
	  {
	     if (dcop_area) {
		QString* tmp = new QString("<FUNC name=\"%1\" qual=\"%4\">%2%3</FUNC>\n");
		*tmp = tmp->arg( *($2) ).arg( *($1) ).arg( *($4) ).arg( *($6) );
		$$ = tmp;
	     } else
	        $$ = new QString("");
	  }
	| T_VIRTUAL return Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS qualifier function_body
	  {
	     if (dcop_area) {
		QString* tmp = new QString("<FUNC name=\"%1\" qual=\"%4\">%2%3</FUNC>\n");
		*tmp = tmp->arg( *($3) ).arg( *($2) ).arg( *($5) ).arg( *($7) );
		$$ = tmp;
	     } else
	        $$ = new QString("");
	  }
	| return Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS qualifier function_body
	  {
	     if (dcop_area) {
		QString* tmp = new QString("<FUNC name=\"%1\" qual=\"%4\">%2%3</FUNC>\n");
		*tmp = tmp->arg( *($2) );
		*tmp = tmp->arg( *($1) );
		*tmp = tmp->arg( *($4) );
		*tmp = tmp->arg( *($6) );
		$$ = tmp;
	     } else
	        $$ = new QString("");
	  }
	| T_STATIC return Identifier T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS qualifier function_body
	  {
		if (dcop_area)
		  yyerror("static is not allowed in dcop area!");
		$$ = new QString();
	  }
	;

function_body
	: T_SEMICOLON
	| T_LEFT_CURLY_BRACKET function_lines T_RIGHT_CURLY_BRACKET
	| T_LEFT_CURLY_BRACKET function_lines T_RIGHT_CURLY_BRACKET T_SEMICOLON

function_lines
	: T_RETURN Identifier T_SEMICOLON function_lines {}
	| T_RETURN Identifier T_ACCESS T_IDENTIFIER T_SEMICOLON function_lines {}
	| T_RETURN Identifier T_ACCESS T_IDENTIFIER T_LEFT_PARANTHESIS params T_RIGHT_PARANTHESIS T_SEMICOLON function_lines {}
	| T_IDENTIFIER T_EQUAL T_IDENTIFIER T_SEMICOLON function_lines {}
	| /* empty */ {}
	;

member
	: return T_IDENTIFIER T_SEMICOLON {}
	| T_STATIC return T_IDENTIFIER T_SEMICOLON {}

%%

void dcopidlParse( const char *_code )
{
    printf("<!DOCTYPE DCOP-IDL><DCOP-IDL>\n");
    dcopidlInitFlex( _code );
    yyparse();
    printf("</DCOP-IDL>\n");
}
