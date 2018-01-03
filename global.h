#ifndef GLOBALS_H_   /* Include guard */
#define GLOBALS_H_

#endif // GLOBALS_H_

// global functions
void boo(char *);

// global variables
extern char msg[200], msgIdent[200], lexeme[200], *symbols, spaces[200], reservedWord[50][70];
extern wint_t ch, chLast;
extern int token, tokens[50], reservedWordLen;
extern FILE *fp;

