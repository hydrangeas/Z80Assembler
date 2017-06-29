/*
** lex.l - Z80 lexer
**
** See Copyright Notice in LICENSE file.
*/

%{
#include <string.h>
#include <ctype.h>

//#define YY_DECL    int yylex(YYSTYPE *lval, parser_state *p)
//
//static int
//lex_return(parser_state *p, int c)
//{
//  p->lineno = p->tline;
//  p->tline = yylineno;
//  return c;
//}
//
//#define LEX_RETURN(c) return lex_return(p, c)
#define LEX_RETURN(c)		{ fprintf(stdout, "lex:%s\n", c); }
#define LEX_RETURN_N(c)	{ fprintf(stdout, "lex:%lf\n",c); }
#define LEX_RETURN_F(c)	{ fprintf(stdout, "lex:%f\n", c); }

#define op_plus		"+"
#define op_minus	"-"
#define op_mult		"*"
#define op_div		"/"
#define op_mod		"%"
#define op_eq			"=="
#define op_neq		"!="
#define op_lt			"<"
#define op_le			"<="
#define op_gt			">"
#define op_ge			">="
#define op_and		"&&"
#define op_or			"||"
#define op_amper	"&"	
#define op_bar		"|"	

%}

%option noyywrap
%option yylineno


TRAIL  ([\t ]|"#"[^\n]*)*
CHAR   [a-zA-Z_]|[\302-\337][\200-\277]|[\340-\357][\200-\277][\200-\277]|[\360-\367][\200-\277][\200-\277]|[\370-\373][\200-\277][\200-\277][\200-\277][\200-\277]|[\374-\375][\200-\277][\200-\277][\200-\277][\200-\277][\200-\277]
CHNUM  ({CHAR}|[0-9])
WORD   {CHAR}{CHNUM}*

%%
"+"{TRAIL}  LEX_RETURN(op_plus);
"-"{TRAIL}  LEX_RETURN(op_minus);
"*"{TRAIL}  LEX_RETURN(op_mult);
"/"{TRAIL}  LEX_RETURN(op_div);
"%"{TRAIL}  LEX_RETURN(op_mod);
"=="{TRAIL} LEX_RETURN(op_eq);
"!="{TRAIL} LEX_RETURN(op_neq);
"<"{TRAIL}  LEX_RETURN(op_lt);
"<="{TRAIL} LEX_RETURN(op_le);
">"{TRAIL}  LEX_RETURN(op_gt);
">="{TRAIL} LEX_RETURN(op_ge);
"&&"{TRAIL} LEX_RETURN(op_and);
"||"{TRAIL} LEX_RETURN(op_or);
"&"{TRAIL}  LEX_RETURN(op_amper);
"="{TRAIL}  LEX_RETURN('=');

{WORD} {
  //lval->id = node_str_new(yytext, yyleng);
  //LEX_RETURN(identifier);
  LEX_RETURN(yytext);
};

{WORD}: {
  yytext[yyleng-1]='\0';
  //lval->id = node_str_new(yytext, yyleng-1);
  //LEX_RETURN(label);
  LEX_RETURN(yytext);
};

{TRAIL}"|"{TRAIL}	LEX_RETURN(op_bar);
{TRAIL}\.{TRAIL}	LEX_RETURN('.');
[(\[{,;:]{TRAIL}	LEX_RETURN(yytext[0]);
[)\]}@]						LEX_RETURN(yytext[0]);
"\n"							LEX_RETURN('\n');
"#"[^\n]*"\n"			LEX_RETURN('\n');

(([1-9][0-9]*)|0) {
  unsigned long temp = atol(yytext);
  //lval->nd = node_int_new(atol(yytext));
  //LEX_RETURN(lit_number);
  LEX_RETURN_N(temp);
};

0x[0-9a-fA-F]+ {
  unsigned long temp;
  sscanf(yytext+2, "%lx", &temp);
  //lval->nd = node_int_new(temp);
  //LEX_RETURN(lit_number);
  LEX_RETURN_N(temp);
};

0o[0-7]+ {
  unsigned long temp;
  sscanf(yytext+2, "%lo", &temp);
  //lval->nd = node_int_new(temp);
  //LEX_RETURN(lit_number);
  LEX_RETURN_N(temp);
};



^[ \t]+LD {
	fprintf(stdout, "lex(op):LD\n");
};

[ \t] ;

. {
    char c = yytext[0];

//    fprintf(stderr, "%s:%d:lexical error", p->fname, yylineno);
    if ((c & 0x80) || !isprint(c)) {
      fprintf(stderr, "('\\%03o').\n", c);
    }
    else if (c == '\\') {
      fprintf(stderr, "('\\\\').\n");
    }
    else {
      fprintf(stderr, "('%c').\n", c);
    }
    exit(1); 
};


%%

int main(int argc, char** argv)
{
	if(argc > 1)
		yyin = fopen(argv[1], "r");
	return yylex();
}
