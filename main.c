#include "includes.h"

char msg[200], lexeme[200], *symbols="{}[]()#:;,\"";
wint_t ch='X', chLast;
int token;

int main(int argc, char *argv[]) {
  char arquivo[200];

  setlocale(LC_ALL, "en_US.utf8");
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
  exit(0);
}
