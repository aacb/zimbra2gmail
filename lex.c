#include "includes.h"

FILE *fp;

void teste(void) {
  printf( "hello, world\n" );
}

int openSourceFile(char *fileName) {
  fp = fopen(fileName, "r");
  //strcat(msg, "abriu o src: ");
  //strcat(msg, fileName);
  //boo(msg);
}


void readNextChar(void) {
  //fwscanf(fp, "%c", &ch);
  chLast = ch;
  ch = fgetwc(fp);
}

void voidSpaces(void) {
  while ( iswspace(ch) && !feof(fp) ) {
    //fwscanf(fp, "%c", &ch);
    ch = fgetwc(fp);
  }
}

int lexico(void) {
  int position = 0, symbolToken = 0;
  //lexeme[0] = 'Y'; lexeme[1] = 'Z'; lexeme[2] = '\0';
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
  do {
    lexeme[position] = ch;
    position++;
    readNextChar();
  } while ( (!strchr(symbols, ch)) && (!iswspace(ch)) && !feof(fp) );
  lexeme[position] = '\0';
  return ID;
/*
  do {
    //fwscanf(fp, "%c", &ch);
    ch = fgetwc(fp);
    printf("%c", ch);
  } while (!feof(fp));
*/
  
}


