#include "includes.h"

char msg[200], lexeme[200], *symbols="{}[]()#:;,\"", spaces[200], spacesCall[200], msgIdent[200];
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
  spaces[0] = '\0';
  spacesCall[0] = '\0';
  openSourceFile(arquivo);
  readNextChar(); // read the first char of the file
  boo(">>>>> zimbra2gmail() is about to start");
  token = lexico();
  zimbra2gmail("");
  boo(">>>>> zimbra2gmail() ended");
  fclose(fp);
  boo(lexeme);

  exit(0);
}
