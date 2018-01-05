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

expr-header ::= HEADER DOISPONTOS ( IS | CONTAINS | MATCHES ) ABRECOLCHETE ASPAS header ASPAS { VIRGULA ASPAS header ASPAS } FECHACOLCHETE ASPAS IDEXTENDED ASPAS

expr-address ::= ADDRESS DOISPONTOS ( ALL | DOMAIN ) DOISPONTOS ( IS | CONTAINS | MATCHES ) DOISPONTOS COMPARATOR ASPAS ID PONTOEVIRGULA ASCII_CASEMAP ASPAS ABRECOLCHETE ASPAS header ASPAS { VIRGULA ASPAS header ASPAS } FECHACOLCHETE ASPAS IDEXTENDED ASPAS

expr-body ::= BODY DOISPONTOS CONTAINS ASPAS IDEXTENDED ASPAS 

expr-date ::= DATE DOISPONTOS ( AFTER | BEFORE ) ASPAS IDEXTENDED ASPAS 

expr-to-me ::= ME DOISPONTOS IN ASPAS to-me ASPAS 

expr-distribution-list ::= LIST

expr-bulk ::= BULK

to-me ::= ( TO [ VIRGULA CC ] | CC )

header ::= ( TO | CC | FROM | SUBJECT | HEADER | ID )

action ::= ABRECHAVES { move | tag | flag | discard | keep | redirect }1 [ STOP PONTOEVIRGULA ]  FECHACHAVES

move ::= FILEINTO ASPAS IDEXTENDED ASPAS PONTOEVIRGULA

tag ::= TAG ASPAS IDEXTENDED ASPAS PONTOEVIRGULA

flag ::= FLAG ASPAS ( READ ) ASPAS PONTOEVIRGULA

discard ::= DISCARD PONTOEVIRGULA

keep ::= KEEP PONTOEVIRGULA

redirect ::= REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA { REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA } [ keep ]

*/

void require(), filters(), requiresRep(), conditions(), action(), expr(), exprRep(), exprHeader(), exprAddress(), exprBody(), exprDate(), exprDistributionList(), exprBulk(), exprToMe(), toMe(), header(), move(), flag(), tag(), discard(), keep(), redirect();

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
  printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", spaces, lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexico();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void matchTakingNextAllExceptAspas (int tokenExpected) {
  printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", spacesCall, lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexicoAllExceptAspas();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void matchNextFilterName (int tokenExpected) {
  printf("%slexeme: %s\t\ttoken: %d\t\texpected token: %d\n", spacesCall, lexeme, token, tokenExpected);
  if (token == tokenExpected) {
    token = lexicoFilterName();
  } else {
    msgLexError(1, tokenExpected);
  }
}

void adjustIndent(char *sSpaces) {
  strcpy(spaces, sSpaces);
  strcat(spaces, "  ");
  sprintf(spacesCall, "%s  ", sSpaces);
}

void zimbra2gmail(char *sSpaces) {
  // zimbra2gmail ::= CERCA NAME ID require { filters }
  adjustIndent(sSpaces);
  match(CERCA);
  match(NAME);
  match(ID);
  adjustIndent(sSpaces);
  require(spacesCall);
  adjustIndent(sSpaces);
  while (token == CERCA) {
    adjustIndent(sSpaces);
    filters(spacesCall);
  }
}

void require(char *sSpaces) {
  // require ::= ZIMBRAMAILSIEVESCRIPT DOISPONTOS REQUIRE ABRECOLCHETE requires-rep { VIRGULA requires-rep } FECHACOLCHETE PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(ZIMBRAMAILSIEVESCRIPT);
  match(DOISPONTOS);
  match(REQUIRE);
  match(ABRECOLCHETE);
  requiresRep(spacesCall);
  while ( token == VIRGULA ) {
    match(VIRGULA);
    adjustIndent(sSpaces);
    requiresRep(spacesCall);
  }
  match(FECHACOLCHETE);
  match(PONTOEVIRGULA);
}

void requiresRep(char *sSpaces) {
  // requires-rep ::= ASPAS ( FILEINTO | REJECT | TAG | FLAG ) ASPAS
  adjustIndent(sSpaces);
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

void filters(char *sSpaces) {
  // filters ::= CERCA IDEXTENDED conditions action
  adjustIndent(sSpaces);
  matchNextFilterName(CERCA);
  match(IDFILTERNAME);
  adjustIndent(sSpaces);
  conditions(spacesCall);
  adjustIndent(sSpaces);
  action(spacesCall);
}


void conditions(char *sSpaces) {
  //conditions ::= ( IF | DISABLED_IF ) ( ANYOF | ALLOF ) ABREPARENTESIS expr FECHAPARENTESIS
  adjustIndent(sSpaces);
  switch (token) {
    case IF:
      match(IF);
      break;
    case DISABLED_IF:
      match(DISABLED_IF);
      break;
    default:
      msgLexError(2, IF, DISABLED_IF);
  }
  
  switch (token) {
    case ANYOF:
      match(ANYOF);
      break;
    case ALLOF:
      match(ALLOF);
      break;
    default:
      msgLexError(2, ALLOF, ANYOF);
  }
  match(ABREPARENTESIS);
  adjustIndent(sSpaces);
  expr(spacesCall);
  match(FECHAPARENTESIS);
}

void expr(char *sSpaces) {
  // expr ::= expr-rep { VIRGULA expr-rep }
  adjustIndent(sSpaces);
  exprRep(spacesCall);
  while (token == VIRGULA) {
    match(VIRGULA);
    adjustIndent(sSpaces);
    exprRep(spacesCall);
  }
}

void exprRep(char *sSpaces) {
  // expr-rep ::= [ NOT ] ( expr-header | expr-address | expr-body | expr-date | expr-bulk | expr-distribution-list | expr-me )
  if (token == NOT) {
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

void exprAddress(char *sSpaces) {
  // expr-address ::= ADDRESS DOISPONTOS ( ALL | DOMAIN ) DOISPONTOS ( IS | CONTAINS | MATCHES ) DOISPONTOS COMPARATOR ASPAS ID PONTOEVIRGULA ASCII_CASEMAP ASPAS ABRECOLCHETE ASPAS header ASPAS { VIRGULA ASPAS header ASPAS } FECHACOLCHETE ASPAS IDEXTENDED ASPAS
  adjustIndent(sSpaces);
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
  match(ASPAS);
  adjustIndent(sSpaces);
  header(spacesCall);
  match(ASPAS);
  while (token == VIRGULA) {
    match(VIRGULA);
    match(ASPAS);
    adjustIndent(sSpaces);
    header(spacesCall);
    match(ASPAS);
  }
  match(FECHACOLCHETE);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprHeader(char *sSpaces) {
  // expr-header ::= HEADER DOISPONTOS ( IS | CONTAINS | MATCHES ) ABRECOLCHETE ASPAS header ASPAS { VIRGULA ASPAS header ASPAS } FECHACOLCHETE ASPAS IDEXTENDED ASPAS
  adjustIndent(sSpaces);
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
  match(ASPAS);
  adjustIndent(sSpaces);
  header(spacesCall);
  match(ASPAS);
  while (token == VIRGULA) {
    match(VIRGULA);
    match(ASPAS);
    adjustIndent(sSpaces);
    header(spacesCall);
    match(ASPAS);
  }
  match(FECHACOLCHETE);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprBody(char *sSpaces) {
  // expr-body ::= BODY DOISPONTOS CONTAINS ASPAS IDEXTENDED ASPAS 
  adjustIndent(sSpaces);
  match(BODY);
  match(DOISPONTOS);
  match(CONTAINS);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
}

void exprDate(char *sSpaces) {
  // expr-date ::= DATE DOISPONTOS ( AFTER | BEFORE ) ASPAS IDEXTENDED ASPAS 
  adjustIndent(sSpaces);
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

void exprToMe(char *sSpaces) {
  // expr-to-me ::= ME DOISPONTOS IN ASPAS to-me ASPAS 
  adjustIndent(sSpaces);
  match(ME);
  match(DOISPONTOS);
  match(IN);
  match(ASPAS);
  toMe();
  match(ASPAS);
}

void toMe(char *sSpaces) {
  // to-me ::= ( TO [ VIRGULA CC ] | CC )
  adjustIndent(sSpaces);
  switch (token) {
    case TO:
      match(TO);
      if (token == VIRGULA) {
        match(VIRGULA);
        match(CC);
      }
      break;
    case CC:
      match(CC);
      break;
    default:
      msgLexError(2, CC, TO);
  }
}

void exprBulk(char *sSpaces) {
  // expr-bulk ::= BULK
  adjustIndent(sSpaces);
  match(BULK);
}

void exprDistributionList(char *sSpaces) {
  // expr-distribution-list ::= LIST
  adjustIndent(sSpaces);
  match(LIST);
}

void header(char *sSpaces) {
  // header ::= ( TO | CC | FROM | SUBJECT | HEADER | ID )
  adjustIndent(sSpaces);
  switch (token) {
    case TO:
      match(TO);
      break;
    case CC:
      match(CC);
      break;
    case FROM:
      match(FROM);
      break;
    case SUBJECT:
      match(SUBJECT);
      break;
    case HEADER:
      match(HEADER);
      break;
    case ID:
      match(ID);
      break;
    default:
      msgLexError(5, CC, HEADER, FROM, ID, SUBJECT, TO);
  }
}

void action(char *sSpaces) {
  // action ::= ABRECHAVES { move | tag | flag | discard | keep | redirect }1 [ STOP PONTOEVIRGULA ]  FECHACHAVES
  adjustIndent(sSpaces);
  match(ABRECHAVES);
  while (token == FILEINTO || token == TAG || token == FLAG || token == DISCARD || token == KEEP || token == REDIRECT ) {
    switch (token) {
      case FILEINTO:
        adjustIndent(sSpaces);
        move(spacesCall);
        break;
      case TAG:
        adjustIndent(sSpaces);
        tag(spacesCall);
        break;
      case FLAG:
        adjustIndent(sSpaces);
        flag(spacesCall);
        break;
      case DISCARD:
        adjustIndent(sSpaces);
        discard(spacesCall);
        break;
      case KEEP:
        adjustIndent(sSpaces);
        keep(spacesCall);
        break;
      case REDIRECT:
        adjustIndent(sSpaces);
        redirect(spacesCall);
        break;
      default:
        msgLexError(6, DISCARD, FILEINTO, FLAG, KEEP, REDIRECT, TAG);
    }
  }
  if (token == STOP) {
    match(STOP);
    match(PONTOEVIRGULA);
  }
  match(FECHACHAVES);
}

void move(char *sSpaces) {
  // move ::= FILEINTO ASPAS IDEXTENDED ASPAS PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(FILEINTO);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void tag(char *sSpaces) {
  // tag ::= TAG ASPAS IDEXTENDED ASPAS PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(TAG);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void flag(char *sSpaces) {
  // flag ::= FLAG ASPAS ( READ ) ASPAS PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(FLAG);
  match(ASPAS);
  switch (token) {
    case READ:
      match(READ);
      break;
    default:
      msgLexError(1, READ);
  }
  match(ASPAS);
  match(PONTOEVIRGULA);
}

void discard(char *sSpaces) {
  // discard ::= DISCARD PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(DISCARD);
  match(PONTOEVIRGULA);
}

void keep(char *sSpaces) {
  // keep ::= KEEP PONTOEVIRGULA
  adjustIndent(sSpaces);
  match(KEEP);
  match(PONTOEVIRGULA);
}

void redirect(char *sSpaces) {
  // redirect ::= REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA { REDIRECT ASPAS IDEXTENDED ASPAS PONTOEVIRGULA } [ keep ]
  adjustIndent(sSpaces);
  match(REDIRECT);
  matchTakingNextAllExceptAspas(ASPAS);
  match(IDEXTENDED);
  match(ASPAS);
  match(PONTOEVIRGULA);
  while (token == REDIRECT) {
    adjustIndent(sSpaces);
    match(REDIRECT);
    matchTakingNextAllExceptAspas(ASPAS);
    match(IDEXTENDED);
    match(ASPAS);
    match(PONTOEVIRGULA);
  }
  if (token == KEEP) {
    adjustIndent(sSpaces);
    keep(spacesCall);
  }
}



