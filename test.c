#include <stdio.h>
#include <ctype.h>
#include "eval.h"


void test1()
{
   Expr* expr;
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
   printExpr(expr);
   printf("\n");
   deepReleaseExpr(expr);

}


int main(int argc, char* argv[]) {

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
            printExpr(expr2);
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
      printExpr(expr);
      printf("\n");
      deepReleaseExpr(expr);

   /*}*/
   return result;


}


