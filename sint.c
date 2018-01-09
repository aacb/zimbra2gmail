#include "includes.h"

/*
Regular grammar suited for a top-down predictive parser of the Zimbra filter sintax:

zimbra2gmail ::= CERCA NAME ID require { filters }

require ::= ZIMBRAMAILSIEVESCRIPT DOISPONTOS REQUIRE ABRECOLCHETE requires-rep { VIRGULA requires-rep } FECHACOLCHETE PONTOEVIRGULA

requires-rep ::= ASPAS ( FILEINTO | REJECT | TAG | FLAG ) ASPAS

filters ::= CERCA IDEXTENDED conditions action

conditions ::= ( IF | DISABLED_IF ) ( ANYOF | ALLOF ) ABREPARENTESIS expr FECHAPARENTESIS

expr ::= expr-rep { VIRGULA expr-rep }

expr-rep ::= [ NOT ] ( expr-header | expr-address | expr-body | expr-date | expr-bulk | expr-distribution-list | expr-to-me )

expr-header ::= HEADER DOISPONTOS ( IS | CONTAINS | MATCHES ) ABRECOLCHETE header { VIRGULA header } FECHACOLCHETE ASPAS IDEXTENDED ASPAS

expr-address ::= ADDRESS DOISPONTOS ( ALL | DOMAIN ) DOISPONTOS ( IS | CONTAINS | MATCHES ) DOISPONTOS COMPARATOR ASPAS ID PONTOEVIRGULA ASCII_CASEMAP ASPAS ABRECOLCHETE header { VIRGULA header } FECHACOLCHETE ASPAS IDEXTENDED ASPAS

expr-body ::= BODY DOISPONTOS CONTAINS ASPAS IDEXTENDED ASPAS 

expr-date ::= DATE DOISPONTOS ( AFTER | BEFORE ) ASPAS IDEXTENDED ASPAS 

expr-to-me ::= ME DOISPONTOS IN ASPAS to-me ASPAS 

expr-distribution-list ::= LIST

expr-bulk ::= BULK

to-me ::= ( TO [ VIRGULA CC ] | CC )

header ::= ASPAS ( TO | CC | FROM | SUBJECT | HEADER | ID ) ASPAS

action ::= ABRECHAVES { move | tag | flag | discard | keep | redirect }1 [ STOP PONTOEVIRGULA ]  FECHACHAVES

move ::= FILEINTO ASPAS IDEXTENDED ASPAS PONTOEVIRGULA

tag ::= TAG ASPAS IDEXTENDED ASPAS PONTOEVIRGULA

flag ::= FLAG ASPAS ( READ ) ASPAS PONTOEVIRGULA

discard ::= DISCARD PONTOEVIRGULA

keep ::= KEEP PONTOEVIRGULA

redirect ::= REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA { REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA } [ keep ]

*/

void require(), filters(), requiresRep(), conditions(), action(), expr(), exprRep(), exprHeader(), exprAddress(), exprBody(), exprDate(), exprDistributionList(), exprBulk(), exprToMe(), toMe(), header(), move(), flag(), tag(), discard(), keep(), redirect();

void cleanLabel(char *);

/*
 ************************************************************
 * Things to consider for the semantic actions that will be *
 * added to the parser                                      *
 ************************************************************
 // gmail gam command syntax for filters:
 gam user <username>|group <groupname>|ou <ouname>|all users filter
  from <email>|to <email>|subject <words>|haswords <words>|nowords <words>|musthaveattachment
  label <label name>|markread|archive|star|forward <email address>|trash|neverspam|important|notimportant

*/

void cleanLabel(char *myString) {
  char *tmp, chTmp;
  int tam;

  if (myString[0] == '/') {
    tmp = myString + 1;
    strcpy(myString,tmp);
  }
  chTmp = myString[5];
  myString[5] = '\0';
  if ( !strcmp(myString, "Inbox") ) {
    myString[5] = chTmp;
    tmp = myString + 5;
    strcpy(myString,tmp);
    if (myString[0] == '/') {
      tmp = myString + 1;
      strcpy(myString,tmp);
    }
  } else {
    myString[5] = chTmp;
  }
  if (strlen(myString) == 0) {
    strcpy(myString, "inbox");
  }
}

void msgLexError( int num, ...) {
  va_list valist;
  int i;
  char cTokens[400] = "";
  /* initialize valist for num number of arguments */
  va_start(valist, num);

  /* access all the arguments assigned to valist */
  sprintf(cTokens, "%d", va_arg(valist, int));
  for (i = 1; i < num; i++) {
    sprintf(cTokens, "%s or %d", cTokens, va_arg(valist, int));
  }
  
  /* clean memory reserved for valist */
  va_end(valist);

  /* print error message */
  printf("\nERROR ... lexeme: %s\n  expected token (or tokens): %s\n  but received this token from the source file: %d\n", lexeme, cTokens, token);
  exit(1);
  
}

void match (int tokenExpected) {
  //printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", spaces, lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexico();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void matchTakingNextAllExceptAspas (int tokenExpected) {
  //printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", , lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexicoAllExceptAspas();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void matchNextFilterName (int tokenExpected) {
  //printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", , lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexicoFilterName();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void zimbra2gmail() {
  // zimbra2gmail ::= CERCA NAME ID require { filters }
  match(CERCA);
  match(NAME);
  strcpy(emailAddr, lexeme);
  sprintf(msg, "##### %s", emailAddr);
  boo(msg);
  match(ID);
  require();
  while (token == CERCA) {
    sprintf(gamUser, "gam user %s filter ", emailAddr);
    filters();
  }
  strcpy(msg, "# deleting the temporary label zLabelToStopActions");
  boo(msg);
  strcpy(gamCommand, "gam user ");
  strcat(gamCommand, emailAddr);
  strcat(gamCommand, " delete label zLabelToStopActions");
  boo(gamCommand);
  boo("");
}

void require() {
  // require ::= ZIMBRAMAILSIEVESCRIPT DOISPONTOS REQUIRE ABRECOLCHETE requires-rep { VIRGULA requires-rep } FECHACOLCHETE PONTOEVIRGULA
  match(ZIMBRAMAILSIEVESCRIPT);
  match(DOISPONTOS);
  match(REQUIRE);
  match(ABRECOLCHETE);
  requiresRep();
  while ( token == VIRGULA ) {
    match(VIRGULA);
    requiresRep();
  }
  match(FECHACOLCHETE);
  match(PONTOEVIRGULA);
}

void requiresRep() {
  // requires-rep ::= ASPAS ( FILEINTO | REJECT | TAG | FLAG ) ASPAS
  match(ASPAS);
  switch (token) {
    case FILEINTO:
      match(FILEINTO);
      break;
    case REJECT:
      match(REJECT);
      break;
    case TAG:
      match(TAG);
      break;
    case FLAG:
      match(FLAG);
      break;
    default:
      msgLexError(4, FILEINTO, FLAG, REJECT, TAG);
  }
  match(ASPAS);
}

void filters() {
  // filters ::= CERCA IDEXTENDED conditions action
  matchNextFilterName(CERCA);
  strcpy(thisFilterName, lexeme);
  strcpy(msg, "# ");
  strcat(msg, thisFilterName);
  boo(msg);
  match(IDFILTERNAME);
  conditions();
  action();
  if (strlen(gamAction) > 0) {
    strcpy(gamCommand, gamUser);
    strcat(gamCommand, gamCondition);
    strcat(gamCommand, gamAction);
    boo(gamCommand);
  }
  if (!strcmp(hasStop, "y")) {
    strcpy(gamCommand, gamUser);
    strcat(gamCommand, gamCondition);
    strcat(gamCommand, "label zLabelToStopActions");
    boo(gamCommand);
  }
  boo("");
}


void conditions() {
  //conditions ::= ( IF | DISABLED_IF ) ( ANYOF | ALLOF ) ABREPARENTESIS expr FECHAPARENTESIS
  strcpy(gamCondition, "haswords \"-label:zLabelToStopActions ");
  switch (token) {
    case IF:
      strcpy(filterEnabled, "y");
      match(IF);
      break;
    case DISABLED_IF:
      strcpy(filterEnabled, "n");
      match(DISABLED_IF);
      break;
    default:
      msgLexError(2, IF, DISABLED_IF);
  }
  
  switch (token) {
    case ANYOF:
      strcpy(conditionType,"anyof");
      match(ANYOF);
      break;
    case ALLOF:
      strcpy(conditionType,"allof");
      match(ALLOF);
      break;
    default:
      msgLexError(2, ALLOF, ANYOF);
  }
  match(ABREPARENTESIS);
  expr();
  match(FECHAPARENTESIS);
}

void expr() {
  // expr ::= expr-rep { VIRGULA expr-rep }
  strcat(gamCondition, "(");
  exprRep();
  while (token == VIRGULA) {
    if ( !strcmp(conditionType, "anyof") )
      strcat(gamCondition, "OR ");
    match(VIRGULA);
    exprRep();
  }
  strcat(gamCondition, ")\" ");
}

void exprRep() {
  // expr-rep ::= [ NOT ] ( expr-header | expr-address | expr-body | expr-date | expr-bulk | expr-distribution-list | expr-me )
  if (token == NOT) {
    strcat(gamCondition, "-");
    match(NOT);
  }
  switch (token) {
    case HEADER:
      exprHeader();
      break;
    case ADDRESS:
      exprAddress();
      break;
    case BODY:
      exprBody();
      break;
    case DATE:
      exprDate();
      break;
    case BULK:
      exprBulk();
      break;
    case LIST:
      exprDistributionList();
      break;
    case ME:
      exprToMe();
      break;
    default:
      msgLexError(7, ADDRESS, BODY, BULK, DATE, HEADER, LIST, ME);
  }
}

void exprHeader() {
  // expr-header ::= HEADER DOISPONTOS ( IS | CONTAINS | MATCHES ) ABRECOLCHETE header { VIRGULA header } FECHACOLCHETE ASPAS IDEXTENDED ASPAS
  match(HEADER);
  match(DOISPONTOS);
  switch (token) {
    case IS:
      match(IS);
      break;
    case CONTAINS:
      match(CONTAINS);
      break;
    case MATCHES:
      match(MATCHES);
      break;
    default:
      msgLexError(3, CONTAINS, IS, MATCHES);
  }
  match(ABRECOLCHETE);
  header();
  while (token == VIRGULA) {
    match(VIRGULA);
    header();
  }
  match(FECHACOLCHETE);
  matchTakingNextAllExceptAspas(ASPAS);
  // replace(gamCondition, "xYz", lexeme);
  replacedString = strReplace(gamCondition, "xYz", lexeme);
  strcpy(gamCondition, replacedString);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprAddress() {
  // expr-address ::= ADDRESS DOISPONTOS ( ALL | DOMAIN ) DOISPONTOS ( IS | CONTAINS | MATCHES ) DOISPONTOS COMPARATOR ASPAS ID PONTOEVIRGULA ASCII_CASEMAP ASPAS ABRECOLCHETE header { VIRGULA header } FECHACOLCHETE ASPAS IDEXTENDED ASPAS
  match(ADDRESS);
  match(DOISPONTOS);
  switch (token) {
    case ALL:
      match(ALL);
      break;
    case DOMAIN:
      match(DOMAIN);
      break;
    default:
      msgLexError(2, ALL, DOMAIN);
  }
  match(DOISPONTOS);
  switch (token) {
    case IS:
      match(IS);
      break;
    case CONTAINS:
      match(CONTAINS);
      break;
    case MATCHES:
      match(MATCHES);
      break;
    default:
      msgLexError(3, CONTAINS, IS, MATCHES);
  }
  match(DOISPONTOS);
  match(COMPARATOR);
  match(ASPAS);
  match(ID);
  match(PONTOEVIRGULA);
  match(ASCII_CASEMAP);
  match(ASPAS);
  match(ABRECOLCHETE);
  strcat(gamCondition, "(");
  header();
  while (token == VIRGULA) {
    strcat(gamCondition, "OR ");
    match(VIRGULA);
    header();
  }
  strcat(gamCondition, ") ");
  match(FECHACOLCHETE);
  matchTakingNextAllExceptAspas(ASPAS);
  // replace(gamCondition, "xYz", lexeme);
  replacedString = strReplace(gamCondition, "xYz", lexeme);
  strcpy(gamCondition, replacedString);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprBody() {
  // expr-body ::= BODY DOISPONTOS CONTAINS ASPAS IDEXTENDED ASPAS 
  match(BODY);
  strcat(gamCondition, "'");
  match(DOISPONTOS);
  match(CONTAINS);
  matchTakingNextAllExceptAspas(ASPAS);
  strcat(gamCondition, lexeme);
  strcat(gamCondition, "' ");
  match(IDEXTENDED);
  match(ASPAS);
}

void exprDate() {
  // expr-date ::= DATE DOISPONTOS ( AFTER | BEFORE ) ASPAS IDEXTENDED ASPAS 
  match(DATE);
  match(DOISPONTOS);
  switch (token) {
    case AFTER:
      match(AFTER);
      break;
    case BEFORE:
      match(BEFORE);
      break;
    default:
      msgLexError(2, AFTER, BEFORE);
  }
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprToMe() {
  // expr-to-me ::= ME DOISPONTOS IN ASPAS to-me ASPAS 
  match(ME);
  match(DOISPONTOS);
  match(IN);
  match(ASPAS);
  toMe();
  match(ASPAS);
}

void exprDistributionList() {
  // expr-distribution-list ::= LIST
  match(LIST);
}

void exprBulk() {
  // expr-bulk ::= BULK
  match(BULK);
}

void toMe() {
  // to-me ::= ( TO [ VIRGULA CC ] | CC )
  strcat(gamCondition, "(");
  switch (token) {
    case TO:
      strcat(gamCondition, "to:'");
      strcat(gamCondition, emailAddr);
      strcat(gamCondition, "' "); match(TO);
      if (token == VIRGULA) {
        strcat(gamCondition, "OR cc:'");
        strcat(gamCondition, emailAddr);
        strcat(gamCondition, "' ");
        match(VIRGULA);
        match(CC);
      }
      break;
    case CC:
      strcat(gamCondition, "cc:'");
      strcat(gamCondition, emailAddr);
      strcat(gamCondition, "' ");
      break;
    default:
      msgLexError(2, CC, TO);
  }
  strcat(gamCondition, ") ");
  // replace(gamCondition, "xYz", emailAddr)
  replacedString = strReplace(gamCondition, "xYz", emailAddr);
  strcpy(gamCondition, replacedString);
}

void header() {
  // header ::= ASPAS ( TO | CC | FROM | SUBJECT | HEADER | ID ) ASPAS
  match(ASPAS);
  switch (token) {
    case TO:
      strcat(gamCondition, "to:'");
      match(TO);
      break;
    case CC:
      strcat(gamCondition, "cc:'");
      match(CC);
      break;
    case FROM:
      strcat(gamCondition, "from:'");
      match(FROM);
      break;
    case SUBJECT:
      strcat(gamCondition, "subject:'");
      match(SUBJECT);
      break;
    case HEADER:
      strcat(gamCondition, "header:'");
      match(HEADER);
      break;
    case ID:
      strcat(gamCondition, lexeme);
      strcat(gamCondition, ":'");
      match(ID);
      break;
    default:
      msgLexError(5, CC, HEADER, FROM, ID, SUBJECT, TO);
  }
  match(ASPAS);
  strcat(gamCondition, "xYz' ");
}

void action() {
  // action ::= ABRECHAVES { move | tag | flag | discard | keep | redirect }1 [ STOP PONTOEVIRGULA ]  FECHACHAVES
  strcpy(gamAction, "");
  match(ABRECHAVES);
  while (token == FILEINTO || token == TAG || token == FLAG || token == DISCARD || token == KEEP || token == REDIRECT ) {
    switch (token) {
      case FILEINTO:
        move();
        break;
      case TAG:
        tag();
        break;
      case FLAG:
        flag();
        break;
      case DISCARD:
        discard();
        break;
      case KEEP:
        keep();
        break;
      case REDIRECT:
        redirect();
        break;
      default:
        msgLexError(6, DISCARD, FILEINTO, FLAG, KEEP, REDIRECT, TAG);
    }
  }
  strcpy(hasStop, "n");
  if (token == STOP) {
    strcpy(hasStop, "y");
    match(STOP);
    match(PONTOEVIRGULA);
  }
  match(FECHACHAVES);
}

void move() {
  // move ::= FILEINTO ASPAS IDEXTENDED ASPAS PONTOEVIRGULA
  match(FILEINTO);
  matchTakingNextAllExceptAspas(ASPAS);
  strcpy(gamLabel, "label '");
  cleanLabel(lexeme);
  strcat(gamLabel, lexeme);
  strcat(gamLabel, "' archive ");
  strcpy(gamCommand, gamUser);
  strcat(gamCommand, gamCondition);
  strcat(gamCommand, gamLabel);
  boo(gamCommand);
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void tag() {
  // tag ::= TAG ASPAS IDEXTENDED ASPAS PONTOEVIRGULA
  match(TAG);
  matchTakingNextAllExceptAspas(ASPAS);
  strcpy(gamLabel, "label myTaggedMessages/'");
  cleanLabel(lexeme);
  strcat(gamLabel, lexeme);
  strcat(gamLabel, "' ");
  strcpy(gamCommand, gamUser);
  strcat(gamCommand, gamCondition);
  strcat(gamCommand, gamLabel);
  boo(gamCommand);
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void flag() {
  // flag ::= FLAG ASPAS ( READ ) ASPAS PONTOEVIRGULA
  match(FLAG);
  match(ASPAS);
  switch (token) {
    case READ:
      strcat(gamAction, "markread ");
      match(READ);
      break;
    default:
      msgLexError(1, READ);
  }
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void discard() {
  // discard ::= DISCARD PONTOEVIRGULA
  strcat(gamAction, "trash ");
  match(DISCARD);
  match(PONTOEVIRGULA);
}

void keep() {
  // keep ::= KEEP PONTOEVIRGULA
  strcpy(gamLabel, "label inbox ");
  strcpy(gamCommand, gamUser);
  strcat(gamCommand, gamCondition);
  strcat(gamCommand, gamLabel);
  boo(gamCommand);
  match(KEEP);
  match(PONTOEVIRGULA);
}

void redirect() {
  // redirect ::= REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA { REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA } [ keep ]
  match(REDIRECT);
  matchTakingNextAllExceptAspas(ASPAS);
  strcat(gamAction, "forward '");
  strcat(gamAction, lexeme);
  strcat(gamAction, "' ");
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
  while (token == REDIRECT) {
    match(REDIRECT);
    matchTakingNextAllExceptAspas(ASPAS);
    strcat(gamAction, "forward '");
    strcat(gamAction, lexeme);
    strcat(gamAction, "' ");
    match(IDEXTENDED);
    match(ASPAS);
    match(PONTOEVIRGULA);
  }
  if (token == KEEP) {
    keep();
  } else {
    strcat(gamAction, "trash ");
  }
}



