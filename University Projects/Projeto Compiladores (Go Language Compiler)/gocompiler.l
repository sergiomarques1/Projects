/* 
	André Cardoso - 2022222265
	Sérgio Marques - 2022222096
*/

%{
    /* place here any C variables and definitions */

	#include "y.tab.h"
	#include "ast.h"

	extern int yyparse(void);

	int caracter;
	int flag_semicolon = 0;
	int flag_erro = 0;
	int coluna_erro = 0;
	int linha_erro = 0;
	int print_tokens = 0;
	int flag_syn = 0;
	int erro_syn = 0;
	int flag_eof = 0;
	int flag_string = 0;
	char buffer[5000];

	// macro YY_USER_ACTION, which updates 'lex_column', is executed before every action
	#define YY_USER_ACTION { syn_line = linha; syn_column = coluna; }
	#define PASS_TOKEN() yylval.lexeme = strdup(yytext);

	#define PASS_STRING() { \
		char buffer2[5000]; \
		strcpy(buffer2, "\""); \
		strcat(buffer2, buffer); \
		strcat(buffer2, "\""); \
		yylval.lexeme = strdup(buffer2); \
	}\

	int syn_line = -1, syn_column = -1;
	int linha = 1, coluna = 1;

	extern struct node *program;

%}


%X COMMENT STRINGG

digit			[0-9]
octaldigit     	[0-7]
hexdigit       	[0-9a-fA-F]
letter         	[a-zA-Z]|"_"
exponent       	[eE][+-]?{digit}+
hexprefix      	0[xX]
octalprefix		0	

identifier 	{letter}({letter}|{digit})*
comment 	"//".*
whitespace 	" "|"\t"

reserved	"--"|"++"|"break"|"case"|"chan"|"const"|"continue"|"default"|"defer"|"fallthrough"|"go"|"goto"|"import"|"interface"|"map"|"range"|"select"|"struct"|"switch"|"type"

%%
<INITIAL><<EOF>>			{ flag_eof = 1; coluna = 1; if(flag_semicolon && print_tokens) {printf("SEMICOLON\n"); flag_semicolon = 0; }; if(flag_semicolon){PASS_TOKEN(); flag_semicolon = 0; return SEMICOLON;}; return 0;}

"/*"				{ linha_erro = linha; coluna_erro = coluna; BEGIN(COMMENT); coluna += yyleng; }
<COMMENT>.			{ coluna++; }
<COMMENT>\n			{ coluna = 1; linha++; }
<COMMENT>"*/"		{ BEGIN(INITIAL); coluna += yyleng; }
<COMMENT><<EOF>> 	{ flag_eof = 1; printf("Line %d, column %d: unterminated comment\n", linha_erro, coluna_erro); return 1; }

{comment}		{ coluna = 1; } 

"\""						{ coluna_erro = coluna; flag_erro = 0; BEGIN(STRINGG); caracter = 0; coluna += yyleng; flag_semicolon = 0; }
<STRINGG>"\n"				{ if(flag_string == 0){ printf("Line %d, column %d: unterminated string literal\n", linha, coluna_erro);}; BEGIN(INITIAL); coluna = 1; linha++; }
<STRINGG><<EOF>>			{ flag_eof = 1; printf("Line %d, column %d: unterminated string literal\n", linha, coluna_erro); return 1; }
<STRINGG>"\\"[fnrt\\\"]		{ buffer[caracter++] = yytext[0]; buffer[caracter++] = yytext[1]; coluna += 2; }
<STRINGG>"\\"[^fnrt\\\"]	{ flag_string = 0; printf("Line %d, column %d: invalid escape sequence (%s)\n", linha, coluna, yytext); flag_erro = 1; coluna += yyleng; } 
<STRINGG>"\""				{ flag_string = 1; buffer[caracter] = '\0'; if((!flag_erro) && print_tokens){ printf("STRLIT(\"%s\")\n", buffer); }; if(!flag_erro){flag_semicolon = 1; BEGIN(INITIAL); coluna++; PASS_STRING(); return STRLIT; } ; BEGIN(INITIAL); coluna++; }
<STRINGG>.					{ buffer[caracter++] = yytext[0]; coluna++; }

{octalprefix}({octaldigit}*[89]+{octaldigit}*)+  { printf("Line %d, column %d: invalid octal constant (%s)\n", linha, coluna, yytext); coluna += yyleng; flag_semicolon = 0; }

{whitespace}	{; coluna += yyleng; }

\n	{ if(flag_semicolon && print_tokens){ printf("SEMICOLON\n"); flag_semicolon = 0; } if(flag_semicolon){PASS_TOKEN(); flag_semicolon = 0; linha++; coluna = 1; return SEMICOLON;}; linha++; coluna = 1; }	

{reserved}		{ if(print_tokens) printf("RESERVED(%s)\n", yytext); coluna += yyleng; flag_semicolon = 0; PASS_TOKEN(); return RESERVED; }

{hexprefix}{hexdigit}+                 { if(print_tokens) printf("NATURAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return NATURAL;}
{octalprefix}{octaldigit}+             { if(print_tokens) printf("NATURAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return NATURAL;}
{digit}+                               { if(print_tokens) printf("NATURAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return NATURAL;}
{digit}+"."({digit}*{exponent})?       { if(print_tokens) printf("DECIMAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return DECIMAL;}
{digit}+"."{digit}+                    { if(print_tokens) printf("DECIMAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return DECIMAL;}
"."{digit}+{exponent}?                 { if(print_tokens) printf("DECIMAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return DECIMAL;}
{digit}+{exponent}                     { if(print_tokens) printf("DECIMAL(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return DECIMAL;}

";"                                    { if(print_tokens) printf("SEMICOLON\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return SEMICOLON;}
","                                    { if(print_tokens) printf("COMMA\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return COMMA;}
"_"                                    { if(print_tokens) printf("BLANKID\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return BLANKID;}
"="                                    { if(print_tokens) printf("ASSIGN\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return ASSIGN;}
"=="                                   { if(print_tokens) printf("EQ\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return EQ;}
"*"                                    { if(print_tokens) printf("STAR\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return STAR;}
"/"                                    { if(print_tokens) printf("DIV\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return DIV;}
"-"                                    { if(print_tokens) printf("MINUS\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return MINUS;}
"+"                                    { if(print_tokens) printf("PLUS\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return PLUS;}
">="                                   { if(print_tokens) printf("GE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return GE;}
">"                                    { if(print_tokens) printf("GT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return GT;}
"<="                                   { if(print_tokens) printf("LE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return LE;}
"<"                                    { if(print_tokens) printf("LT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return LT;}
"!="                                   { if(print_tokens) printf("NE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return NE;}
"%"                                    { if(print_tokens) printf("MOD\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return MOD;}
"!"                                    { if(print_tokens) printf("NOT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return NOT;}
"&&"                                   { if(print_tokens) printf("AND\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return AND;}
"||"                                   { if(print_tokens) printf("OR\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return OR;}
"{"                                    { if(print_tokens) printf("LBRACE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return LBRACE;}
"}"                                    { if(print_tokens) printf("RBRACE\n"); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return RBRACE;}
"("                                    { if(print_tokens) printf("LPAR\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return LPAR;}
")"                                    { if(print_tokens) printf("RPAR\n"); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return RPAR;}
"["                                    { if(print_tokens) printf("LSQ\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return LSQ;}
"]"                                    { if(print_tokens) printf("RSQ\n"); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return RSQ;}
"package"                              { if(print_tokens) printf("PACKAGE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return PACKAGE;}
"return"                               { if(print_tokens) printf("RETURN\n"); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return RETURN;}
"else"                                 { if(print_tokens) printf("ELSE\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return ELSE;}
"for"                                  { if(print_tokens) printf("FOR\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return FOR;}
"if"                                   { if(print_tokens) printf("IF\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return IF;}
"var"                                  { if(print_tokens) printf("VAR\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return VAR;}
"int"                                  { if(print_tokens) printf("INT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return INT;}
"float32"                              { if(print_tokens) printf("FLOAT32\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return FLOAT32;}
"bool"                                 { if(print_tokens) printf("BOOL\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return BOOL;}
"string"                               { if(print_tokens) printf("STRING\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return STRING;}
"fmt.Println"                          { if(print_tokens) printf("PRINT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return PRINT;}
"strconv.Atoi"                         { if(print_tokens) printf("PARSEINT\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return PARSEINT;}
"func"                                 { if(print_tokens) printf("FUNC\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return FUNC;}
"os.Args"                              { if(print_tokens) printf("CMDARGS\n"); flag_semicolon = 0; coluna += yyleng; PASS_TOKEN(); return CMDARGS;}

{identifier}		{ if(print_tokens) printf("IDENTIFIER(%s)\n", yytext); flag_semicolon = 1; coluna += yyleng; PASS_TOKEN(); return IDENTIFIER;}

.	{ printf("Line %d, column %d: illegal character (%s)\n", linha, coluna, yytext) ; coluna += yyleng; } 
%%

void yyerror(char *s) {
		
	erro_syn = 1;
	
	if(flag_eof == 1){
		if(coluna == 1){
			printf("Line %d, column %d: %s: %s\n", linha, coluna, s, yytext);
		}
		else{
			printf("Line %d, column %d: %s: %s\n", linha, coluna - yyleng, s, yytext);
		}
		return;
	}

	if(flag_string == 1){
		char buffer_string[5000]; 
		strcpy(buffer_string, "\""); 
		strcat(buffer_string, buffer); 
		strcat(buffer_string, "\""); 
		printf("Line %d, column %d: %s: %s\n", linha, coluna_erro, s, buffer_string);
		flag_string = 0;
	}
	else{
		printf("Line %d, column %d: %s: %s\n", syn_line, syn_column, s, yytext);
	}
}

extern int yylex();

int main(int argc, char **argv) {

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-l") == 0){
			print_tokens = 1;
			break;
		}
		else if(strcmp(argv[i], "-t") == 0){
			flag_syn = 1;
			break;
		}
	}

	if(flag_syn == 1){
		yyparse();
	}
	
	if(print_tokens == 1){
		yylex();
	}

	if(flag_syn == 1 && erro_syn == 0){
		show(program, 0);
	}

	free_arvore(program);

    return 0;
}

int yywrap() {  /* called on EOF, return 1 to terminate */
    return 1;
}