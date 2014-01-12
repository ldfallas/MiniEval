#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "eval.h"

void tassert(int result, const char* testName, char* formatString, ...) {

  va_list args;
  va_start(args, formatString);
  if (!result) {
    printf("ASSERT FAILED in test \"%s\": ", testName);
    vprintf(formatString, args);
    printf("\n");
  }
  va_end(args);
}

void tassert_equal_ints(int expectedValue,int actualValue, const char* testName) {
  int comparisonResult;
  comparisonResult = expectedValue == actualValue;
  tassert(comparisonResult,
	  testName,
	  "expected: %d, actual %d", 
	  expectedValue, 
	  actualValue);
}

void tassert_equal_strings(char* expectedValue,char* actualValue, const char* testName) {
  int comparisonResult;
  comparisonResult = strcmp(expectedValue, actualValue) == 0;
  tassert(comparisonResult,
	  testName,
	  "expected: \"%s\", actual \"%s\"", 
	  expectedValue, 
	  actualValue);
}


void testStrBuffer1() { 
  OutStream buff;
  buff = createStringOutStream(5);
  printToOutStream( &buff, 3, "--");
  tassert(strcmp(getStringFromStringOutStream(&buff), "--") == 0,
	  __FUNCTION__, "String printed");
  tassert_equal_ints(2, buff.size,__FUNCTION__);
  destroyOutStream(&buff);
} 

void testStrBuffer2() { 
  OutStream buff;
  buff = createStringOutStream(5);
  printToOutStream( &buff, 3, "12");
  tassert_equal_strings( "12",
			 getStringFromStringOutStream(&buff),
			__FUNCTION__);
  tassert_equal_ints(2, buff.size,__FUNCTION__);
  printToOutStream( &buff, 3, "34");
  printToOutStream( &buff, 3, "56");
  tassert_equal_strings("123456",
			getStringFromStringOutStream(&buff),
			__FUNCTION__);
  tassert_equal_ints(6, buff.size,__FUNCTION__);
  destroyOutStream(&buff);
} 

void testStrBuffer3() { 
  OutStream buff;
  buff = createStringOutStream(5);
  printToOutStream( &buff, 3, "--");
  tassert_equal_strings( "--",
			 getStringFromStringOutStream(&buff),
			__FUNCTION__);
  tassert_equal_ints(2, buff.size,__FUNCTION__);
  printToOutStream( &buff, 30, "Number: %d",2000);
  tassert_equal_strings("--Number: 2000",
			getStringFromStringOutStream(&buff),
			__FUNCTION__);
  
  tassert_equal_ints(14, buff.size,__FUNCTION__);
  destroyOutStream(&buff);
} 


void test1() {
   Expr* expr;
   char txt1[5];
   expr =
      createAddition(
            createAddition(
               createNumLiteral(10.2),
               createNumLiteral(-13)),
            createNumLiteral(13));
   snprintf(
         txt1, 5,
         "%g",
         evaluateExpression(
            expr
            )
         ); 
   tassert(strncmp("10.2", txt1, 5) == 0,
	   __FUNCTION__,
           "Expression evaluation failed");

   deepReleaseExpr(expr);
   
}

void testTreeFormation1() {
   Expr* expr;
   OutStream stringOut;
   
   stringOut = createStringOutStream(15);
   expr =
      createAddition(
            createAddition(
               createNumLiteral(10.2),
               createNumLiteral(-13)),
            createNumLiteral(13));

   printExpr(expr, &stringOut);

   tassert_equal_strings("<<10.2 + -13> + 13>",
			getStringFromStringOutStream(&stringOut),
			__FUNCTION__);
   
   deepReleaseExpr(expr);
   destroyOutStream(&stringOut);
}

int main(int argc, char* argv[]) {
  printf("Running tests\n");
  testStrBuffer1();
  testStrBuffer2();
  testStrBuffer3();
  test1();
  testTreeFormation1();
  return 0;
}

int ___main(int argc, char* argv[]) {

   FILE* aFile;
   Token t;
   int result;
   Expr* expr;
   int parseResult;
   Expr* expr2;
   TokenStreamWithLookAhead tokstream;
   result = 0;
   if (argc == 2) {
      aFile = fopen(argv[1],"r");
      if (aFile != NULL) {
         while(read_tok(aFile, &t) != -1)
         {
            printf("------");
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
         }
         fclose(aFile);
         printf("*******************\n");
         aFile = fopen(argv[1],"r");
         tokstream = createTokenStreamWithLookAhead(aFile);
         parseResult = parseSingleExpr(&tokstream, &expr2);
         printf("!--parse result %d\n", parseResult);
         if(!parseResult) { 
            printf("!parse result %d\n", parseResult);
            /*printExpr(expr2);*/
         }

         /*
         readToken(&tokstream, &t);
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
         readToken(&tokstream, &t);
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
         peekToken(&tokstream, &t);
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
         readToken(&tokstream, &t);
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
         readToken(&tokstream, &t);
            printf("token id=%d\n", t.id);
            printf("token buffer='%s'\n", t.buffer);
            */
         releaseTokStream(&tokstream);
         fclose(aFile);
      } else {
         printf("could not open file");
         result = 1;
      }
   } else {
      result = 1;
      printf("wrong number of arguments");
   }


   /*while(1){*/
      expr =
         createAddition(
               createAddition(
                  createNumLiteral(10.2),
                  createNumLiteral(-13)),
               createNumLiteral(13));
      printf(
            "\n%g\n",
            evaluateExpression(
               expr
               )
            ); 
      /*      printExpr(expr);
	      printf("\n");*/
      deepReleaseExpr(expr);

   /*}*/
   return result;


}


