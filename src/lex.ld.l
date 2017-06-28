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
%}

%option noyywrap
%option yylineno



%%

\tLD {
	fprintf(stdout, "lex:LD");
};

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
