#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "eval.h"

void run_test(char* testname, void (*testfunc)()) {
  printf("Running %s\n", testname);
  testfunc();
}

#define RUN_TEST(testname) run_test(#testname, testname);

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

void testStrBuffer4() { 
  OutStream buff;
  buff = createStringOutStream(5);
  printToOutStream( &buff, 5, "-%d-",12345);
  tassert_equal_strings("-123",
                        getStringFromStringOutStream(&buff),
                        __FUNCTION__);
  tassert_equal_ints(4, buff.size,__FUNCTION__);
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


void testBasicLiteralParsing() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString("10");
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("10",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}

void testBasicLiteralParsing2() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "\t    \t 10 \t \t";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("10",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   destroyOutStream(&stringOut);
   releaseTokStream(&tokstream);
}


void testBasicAdditionParsing1() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3 + 4";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("<3 + 4>",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}

void testMultiAdditionParsing1() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3 + 4 + 5";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("<3 + <4 + 5>>",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}

void testSubtraction1() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3 + 3 * 5 / 3 - 5";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("<3 + <<3 * <5 / 3>> - 5>>",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}


void testParseInvalidExpr1() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3 + (3 * 5 / 3 - 5";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(parseResult == -1, __FUNCTION__, "Parse error");
   if (parseResult != -1) {
     return;
   }
   
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}

void testParseParenExpr1() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3 * (3 - 5 + 3) / 5";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("<3 * <<3 - <5 + 3>> / 5>>",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);

}

void testEvaluation1() {
   Expr* expr;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;
   double evaluationResult;

   stringToParse = "3 + 3 * 5 / 3 - 5";
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }

   evaluationResult = evaluateExpression(expr);
   tassert(3.0 == evaluationResult,
	   __FUNCTION__,
	   "Evaluation result is different %g",
	   evaluationResult);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   
}

void testParsingNumbers() {
   Expr* expr;
   OutStream stringOut;
   int parseResult;
   TokenStreamWithLookAhead tokstream;
   char* stringToParse;

   stringToParse = "3.4 + 0.4";
   stringOut = createStringOutStream(15);
   tokstream = createTokenStreamWithLookAheadFromString(stringToParse);
   parseResult = parseExpr(&tokstream, &expr);
   tassert(!parseResult, __FUNCTION__, "Parse error");
   if (parseResult) {
     return;
   }
   printExpr(expr, &stringOut);
   tassert_equal_strings("<3.4 + 0.4>",
                        getStringFromStringOutStream(&stringOut),
                        __FUNCTION__);
   
   deepReleaseExpr(expr);
   releaseTokStream(&tokstream);
   destroyOutStream(&stringOut);
}


int main(int argc, char* argv[]) {
  
  printf("Running tests\n");

  RUN_TEST(testStrBuffer1);
  RUN_TEST(testStrBuffer2);
  RUN_TEST(testStrBuffer3);
  RUN_TEST(testStrBuffer4);
  RUN_TEST(test1);
  RUN_TEST(testTreeFormation1);
  RUN_TEST(testBasicLiteralParsing);
  RUN_TEST(testBasicLiteralParsing2);
  RUN_TEST(testBasicAdditionParsing1);
  RUN_TEST(testMultiAdditionParsing1);
  RUN_TEST(testSubtraction1);
  RUN_TEST(testParseInvalidExpr1);  
  RUN_TEST(testEvaluation1);
  RUN_TEST(testParsingNumbers);
  RUN_TEST(testParseParenExpr1);
  return 0;
}





