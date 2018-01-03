#include "includes.h"

char msg[200], lexeme[200], *symbols="{}[]()#:;,\"", spaces[200], msgIdent[200];
wint_t ch='X', chLast;
int token, i;

int main(int argc, char *argv[]) {
  char arquivo[200];

  setlocale(LC_ALL, "en_US.utf8");
  for (i=0; i<reservedWordLen; i++) {
    printf("%i  _%s_ %d\n", i, reservedWord[i], tokens[i]);
  }
  strcpy(arquivo, argv[1]);
  //boo(arquivo);
  openSourceFile(arquivo);
  readNextChar(); // read the first char of the file
  boo(">>>>> lexico() is about to start");
  do {
    token = lexico();
    switch (token) {
      case UNKOWN:
        printf("\nERROR. This character is not defined: %c\n", chLast);
        exit (1);
        break;
      default:
        printf("lexeme: %s\t\ttoken: %d\n", lexeme, token);
    }
  } while (!feof(fp));
  boo(">>>>> lexico() ended");
  fclose(fp);
  boo(lexeme);

  strcpy(msg,"2: ");
  strcat(msg,reservedWord[2]);
  strcat(msg,"  18: ");
  strcat(msg,reservedWord[18]);
  boo(msg);

  exit(0);
}
