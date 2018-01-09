#include "includes.h"

char msg[200], lexeme[200], *symbols="{}[]()#:;,\"", spaces[200], spacesCall[200], msgIdent[200];
char emailAddr[200], thisFilterName[200], conditionType[5]="allof", filterEnabled[2], hasStop[2];
char gamCommand[10000], gamUser[10000], gamCondition[10000], gamLabel[10000], gamAction[10000];
char *replacedString;
wint_t ch='X', chLast;
int token, i;

int main(int argc, char *argv[]) {
  char arquivo[200];

  setlocale(LC_ALL, "en_US.utf8");
  //for (i=0; i<reservedWordLen; i++) {
  //  printf("%i  _%s_ %d\n", i, reservedWord[i], tokens[i]);
  //}
  strcpy(arquivo, argv[1]);
  //boo(arquivo);
  spaces[0] = '\0';
  spacesCall[0] = '\0';
  openSourceFile(arquivo);
  readNextChar(); // read the first char of the file
  sprintf(msg, "##### zimbra2gmail() is about to start");
  boo(msg);
  token = lexico();
  zimbra2gmail("");
  sprintf(msg, "##### zimbra2gmail() ended for %s", emailAddr);
  boo(msg);
  fclose(fp);
  // boo(lexeme);

  exit(0);
}
