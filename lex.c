#include "includes.h"

FILE *fp;
// reserved words
//char reservedWord[][70] = { [0 ... 49] = "" };
char reservedWord[][70] = { 
  "address", "all", "allof", "anyof", "ascii-casemap",
  "cc", "comparator", "contains", "disabled_if", "fileinto",
  "flag", "from", "header", "if", "is",
  "name", "require", "stop", "subject", "tag",
  "to", "zimbraMailSieveScript"
};
/*
*/
int tokens[] = { 
  ADDRESS, ALL, ALLOF, ANYOF, ASCII_CASEMAP,
  CC, COMPARATOR, CONTAINS, DISABLED_IF, FILEINTO,
  FLAG, FROM, HEADER, IF, IS,
  NAME, REQUIRE, STOP, SUBJECT, TAG,
  TO, ZIMBRAMAILSIEVESCRIPT
};

int reservedWordLen = 22;

void teste(void) {
  printf( "hello, world\n" );
}

int openSourceFile(char *fileName) {
  fp = fopen(fileName, "r");
}


void readNextChar(void) {
  chLast = ch;
  ch = fgetwc(fp);
}

void voidSpaces(void) {
  while ( iswspace(ch) && !feof(fp) ) {
    ch = fgetwc(fp);
  }
}

int findReservedWord(char *rWord, int fIndex, int lIndex) {
  // search the string rWord in the already sorted array reservedWord,
  // from the first index fIndex up to the last index lIndex
  strcat(spaces, "  ");
  int ind = (fIndex + lIndex)/2;
  if ( strcmp(rWord, reservedWord[fIndex]) == 0 ) {
    msg[0]='\0';
    sprintf(msgIdent, "%sTOKEN _%d_", spaces, tokens[fIndex]);
    boo(msgIdent);
    return tokens[fIndex];
  }
  if ( strcmp(rWord, reservedWord[lIndex]) == 0 ) {
    msg[0]='\0';
    sprintf(msgIdent, "%sTOKEN _%d_", spaces, tokens[lIndex]);
    boo(msgIdent);
    return tokens[lIndex];
  }
  if (fIndex == ind) {
    return -1;
  }
  if ( strcmp(rWord, reservedWord[fIndex]) < 0 ) {
    sprintf(msg, "fIndex _%d_ ; lIndex _%d_ ; ind _%d_ ; lexeme _%s_ ; é menor que fIndex", fIndex, lIndex, ind, rWord);
    sprintf(msgIdent, "%s%s", spaces, msg);
    boo(msgIdent);
    return -1;
  }
  if ( strcmp(rWord, reservedWord[lIndex]) < 0 ) {
    if ( strcmp(rWord, reservedWord[ind]) < 0 ) {
      sprintf(msg, "fIndex _%d_ ; lIndex _%d_ ; ind _%d_ ; lexeme _%s_ ; lIndex = ind", fIndex, lIndex, ind, rWord);
      sprintf(msgIdent, "%s%s", spaces, msg);
      boo(msgIdent);
      findReservedWord(rWord, fIndex, ind);
    } else {
      sprintf(msg, "fIndex _%d_ ; lIndex _%d_ ; ind _%d_ ; lexeme _%s_ ; fIndex = ind", fIndex, lIndex, ind, rWord);
      sprintf(msgIdent, "%s%s", spaces, msg);
      boo(msgIdent);
      findReservedWord(rWord, ind, lIndex);
    }
  } else {
      sprintf(msg, "fIndex _%d_ ; lIndex _%d_ ; ind _%d_ ; lexeme _%s_ ;word _%s_ ; não é reservada", fIndex, lIndex, ind, rWord, reservedWord[lIndex]);
      sprintf(msgIdent, "%s%s", spaces, msg);
      boo(msgIdent);
      return -1;
  }
}

int lexico(void) {
  int position = 0, symbolToken = 0, IDtoken;
  voidSpaces();
  lexeme[0] = ch; lexeme[1] = '\0';
  if (feof(fp)) {
    lexeme[0] = '\0';
    return ENDOFFILE;
  }
  switch (ch) {
    case '{':
      symbolToken = ABRECHAVES;
      break;
    case '[':
      symbolToken = ABRECOLCHETE;
      break;
    case '(':
      symbolToken = ABREPARENTESIS;
      break;
    case '"':
      symbolToken = ASPAS;
      break;
    case '#':
      symbolToken = CERCA;
      break;
    case ':':
      symbolToken = DOISPONTOS;
      break;
    case '}':
      symbolToken = FECHACHAVES;
      break;
    case ']':
      symbolToken = FECHACOLCHETE;
      break;
    case ')':
      symbolToken = FECHAPARENTESIS;
      break;
    case ';':
      symbolToken = PONTOEVIRGULA;
      break;
    case ',':
      symbolToken = VIRGULA;
      break;
    default:
      symbolToken = 0;
  }
  if ( symbolToken != 0) {
    readNextChar();
    return symbolToken;
  }

  // symbolToken == 0, so the character is not a language symbol
  // and it will be considered an ID, or a language reserved word
  do {
    lexeme[position] = ch;
    position++;
    readNextChar();
    if ( position > 198 ) {
      boo("ERROR ... ID string too long");
      exit(1);
    }
  } while ( (!strchr(symbols, ch)) && (!iswspace(ch)) && !feof(fp) );
  lexeme[position] = '\0';
  spaces[0] = '\0';
  IDtoken = findReservedWord(lexeme, 0, reservedWordLen-1);
  if (IDtoken != -1)
    return IDtoken;
  else
    return ID;
/*
some tests
*/
  
}


