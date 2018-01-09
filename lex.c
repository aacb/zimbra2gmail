#include "includes.h"

FILE *fp;
// reserved words
//char reservedWord[][70] = { [0 ... 49] = "" };
char reservedWord[][70] = { 
  "address", "after", "all", "allof", "anyof", "ascii-casemap",
  "before", "body", "bulk", "cc", "comparator", "contains", "date", "disabled_if", "discard",
  "domain", "fileinto", "flag", "from", "header",
  "if", "in", "is", "keep", "list", "matches", "me", "name", "not",
  "read", "redirect", "reject", "require", "stop", "subject",
  "tag", "to", "zimbraMailSieveScript"
};
/*
 in bulk list me 
*/
int tokens[] = { 
  ADDRESS, AFTER, ALL, ALLOF, ANYOF,
  ASCII_CASEMAP, BEFORE, BODY, BULK, CC,
  COMPARATOR, CONTAINS, DATE, DISABLED_IF, DISCARD,
  DOMAIN, FILEINTO, FLAG, FROM, HEADER,
  IF, IN, IS, KEEP, LIST,
  MATCHES, ME, NAME, NOT, READ,
  REDIRECT, REJECT, REQUIRE, STOP, SUBJECT,
  TAG, TO, ZIMBRAMAILSIEVESCRIPT
};

int reservedWordLen = 38;

void teste(void) {
  printf( "hello, world\n" );
}

int openSourceFile(char *fileName) {
  if ( access( fileName, R_OK ) != -1 )
    fp = fopen(fileName, "r");
  else {
    printf("\nERROR ... could not open this file:\n%s\n", fileName);
    exit(1);
  }
}


void readNextChar(void) {
  chLast = ch;
  ch = fgetwc(fp);
  if (ch == '\\') {
    // for now, it was chosen to bypass these chars
    ch = fgetwc(fp);
    ch = fgetwc(fp);
  }
}

void voidSpaces(void) {
  while ( iswspace(ch) && !feof(fp) ) {
    ch = fgetwc(fp);
  }
}

int findReservedWord(char *rWord, int fIndex, int lIndex) {
  // search the string rWord in the already sorted array reservedWord,
  // from the first index fIndex up to the last index lIndex
  int ind = (fIndex + lIndex)/2;
  if ( strcmp(rWord, reservedWord[fIndex]) == 0 ) {
    msg[0]='\0';
    return tokens[fIndex];
  }
  if ( strcmp(rWord, reservedWord[lIndex]) == 0 ) {
    msg[0]='\0';
    return tokens[lIndex];
  }
  if (fIndex == ind) {
    return -1;
  }
  if ( strcmp(rWord, reservedWord[fIndex]) < 0 ) {
    return -1;
  }
  if ( strcmp(rWord, reservedWord[lIndex]) < 0 ) {
    if ( strcmp(rWord, reservedWord[ind]) < 0 ) {
      findReservedWord(rWord, fIndex, ind);
    } else {
      findReservedWord(rWord, ind, lIndex);
    }
  } else {
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
  IDtoken = findReservedWord(lexeme, 0, reservedWordLen-1);
  if (IDtoken != -1)
    return IDtoken;
  else
    return ID;
}

int lexicoAllExceptAspas(void) {
  int position = 0, symbolToken = 0, IDtoken;
  voidSpaces();
  lexeme[0] = ch; lexeme[1] = '\0';
  if (feof(fp)) {
    lexeme[0] = '\0';
    return ENDOFFILE;
  }

  do {
    lexeme[position] = ch;
    position++;
    readNextChar();
    if ( position > 198 ) {
      boo("ERROR ... ID string too long");
      exit(1);
    }
  } while ( (ch != '"') && !feof(fp) );
  lexeme[position] = '\0';
  return IDEXTENDED;
}


int lexicoFilterName(void) {
  int position = 0, symbolToken = 0, IDtoken;
  voidSpaces();
  lexeme[0] = ch; lexeme[1] = '\0';
  if (feof(fp)) {
    lexeme[0] = '\0';
    return ENDOFFILE;
  }

  do {
    lexeme[position] = ch;
    position++;
    readNextChar();
    if ( position > 198 ) {
      boo("ERROR ... ID string too long");
      exit(1);
    }
  } while ( (ch != '\n') && !feof(fp) );
  lexeme[position] = '\0';
  return IDFILTERNAME;
}


