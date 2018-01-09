#ifndef GLOBALS_H_   /* Include guard */
#define GLOBALS_H_

#endif // GLOBALS_H_

// global functions
void boo(char *);
char *strReplace(char *, char *, char *);

// global variables
extern char msg[200], msgIdent[200], lexeme[200], *symbols, spaces[200], spacesCall[200], reservedWord[50][70];
extern wint_t ch, chLast;
extern int token, tokens[50], reservedWordLen;
extern FILE *fp;

// global variables used by the semantic actions
extern char emailAddr[200], thisFilterName[200], conditionType[5], gamCommand[10000], gamUser[10000], gamCondition[10000], gamLabel[10000], gamAction[10000], filterEnabled[2], hasStop[2], *replacedString;

