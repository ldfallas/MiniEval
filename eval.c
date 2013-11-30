#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "eval.h"


int isletter(int c) {
   return isupper(c) || islower(c);
}

int isoperator(int c) {
   return c == '+' || c == '-' || c == '*' || c == '/'; 
}

int isparenthesis(int c) {
   return c == '(' || c == ')';
}


int read_tok(FILE* file, Token* tok) {

   int theChar;
   int c;
   int state;
   int result;
   
   result = 0;
   
   char* buffer; 
   unsigned int buffpos;
   int stop;

   buffer = tok->buffer;
   buffpos = 0;
   state = BLANK_STATE; 
   stop = 0;


   while(!stop) {
      theChar = fgetc(file);
      c = theChar; 
      switch(state) {
         case BLANK_STATE:
            if (isdigit(c)) {
               buffer[buffpos++] = (char)c;
               state = NUMBER_STATE;
               tok->id = 1;
            } else if (isalpha(c)) {

               buffer[buffpos++] = (char)c;
               state = ID_STATE;
               tok->id = 2;
            } else if (isoperator(c)) {
               buffer[buffpos++] = (char)c;
               buffer[buffpos++] = '\0';
               stop = 1;
               tok->id = 3;
            } else if (isparenthesis(c)) {
               buffer[buffpos++] = (char)c;
               buffer[buffpos++] = '\0';
               stop = 1;
               tok->id = 4;
            } else if (isspace(c) && theChar != EOF) {
               continue;
            }
          break;
         case NUMBER_STATE:
            if (isdigit(c) && (buffpos < MAX_BUFF))
            {
               buffer[buffpos++] = (char)c;
            } else if ((c == '.') && (buffpos < MAX_BUFF)) {
               buffer[buffpos++] = (char)c;
            } else {
               buffer[buffpos++] = '\0';
               ungetc(c, file);
               stop = 1;
            }
            break;
         case ID_STATE:
            if ((isletter(c) || isdigit(c)) && (buffpos < MAX_BUFF))
            {
               buffer[buffpos++] = c;
            } else  {
               buffer[buffpos++] = '\0';
               ungetc(c, file);
               stop = 1;
            }
            break;

      }
      if (theChar == EOF) {
         stop = 1;
      }
   }
   result = (theChar == EOF && state == BLANK_STATE) ? -1 : result;
   return result;
}




int isAddition(Expr* expr) {
   return expr->id == ADDITION_NODE;
}

Expr* createAddition(Expr* expr1,Expr* expr2) {
   Expr* result;
   BinExpr* inner;

   
   result = malloc(sizeof(Expr));
   result->id = ADDITION_NODE;
   inner= malloc(sizeof(BinExpr));
   result->payload = inner;
   inner->left = expr1;
   inner->right = expr2;

   return result;
}

double getValueFromLiteral(Expr* expr)
{
   return expr->innerValue;
}

Expr* getLeftExprFromBin(Expr* expr) {
   return ((BinExpr*)expr->payload)->left;
}

Expr* getRightExprFromBin(Expr* expr) {
   return ((BinExpr*)expr->payload)->right;
}

double evaluateExpression(Expr* expr)
{
   double x,y;
   switch(expr->id)
   {
      case ADDITION_NODE: 
         x = evaluateExpression(getLeftExprFromBin(expr));
         y = evaluateExpression(getRightExprFromBin(expr));
         return x + y;
         break;
      case NUM_LITERAL_NODE:
         return getValueFromLiteral(expr);
         break;
      default:
         return (1.0/0.0);
   }
}

Expr* createNumLiteral(double value)
{
   Expr* result;

   result = malloc(sizeof(Expr));
   result->id = NUM_LITERAL_NODE; 
   result->innerValue = value;

   return result;
}

void deepReleaseExpr(Expr* expr)
{
   switch(expr->id)
   {
      case ADDITION_NODE: 
         deepReleaseExpr(getLeftExprFromBin(expr));
         deepReleaseExpr(getRightExprFromBin(expr));
         free(expr->payload);
         free(expr);
         break;
      case NUM_LITERAL_NODE:
         free(expr);
         break;
   }

}

void printExpr(Expr* expr)
{
   switch(expr->id)
   {
      case ADDITION_NODE: 
         printf("<");
         printExpr(getLeftExprFromBin(expr));
         printf(" + ");
         printExpr(getRightExprFromBin(expr));
         printf(">");
         break;
      case NUM_LITERAL_NODE:
         printf("%g",expr->innerValue);
         break;
      default:
         printf("?? %d\n",expr->id);
   }
}


int parse(FILE* inputFile, Expr** expr)
{
  /* read_tok(aFile, &t)*/
  /* SINGLEEXPR = lit | '(' MULTEXPR ')' 
  /* SUMEXPR = SINGLEEXPR ('-'|'+' SUMEXPR)? */
  /* MULTEXPR = SUMEXPR ('*'|'/' MULTEXPR)? */
}

int parseSingleExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken;
   int readResult, innerReadResult;
   Expr* innerExpr;

   readResult = peekToken(stream, &peekedToken);
   if (readResult == 0)
   {
      if (peekedToken.id == 4 
          && strncmp(peekedToken.buffer,"(",2) == 0) {
         readToken(stream, &peekedToken);
         parseSingleExpr(stream,&innerExpr);
         printf("1,");


         if((readResult = readToken(stream, &peekedToken))
            && peekedToken.id == 4
            && strncmp(peekedToken.buffer, ")",2)) {
            *expr = innerExpr;
         printf("2,");
            return 0;
         } else {
         printf("3, -- 00%d ",readResult);
            return -1;
         }
      } else if (peekedToken.id == 1) {
         readResult = readToken(stream, &peekedToken);
         printf("middle read: %d ",readResult);
         *expr = createNumLiteral(atof(peekedToken.buffer));
         return 0;
      } else {
         return -1;
      }

   }
   return readResult;
}

int parseMultExpression(
       TokenStreamWithLookAhead* stream, 
       Expr** expr) {
}
int parseSumExprExpression(
       TokenStreamWithLookAhead* stream, 
       Expr** expr) {
}



TokenStreamWithLookAhead createTokenStreamWithLookAhead(FILE* file)
{
 TokenStreamWithLookAhead result;  
 result.stream = file;
 result.bufferedToken = NULL;
 return result;
}

int readToken(TokenStreamWithLookAhead* tokStream,
              Token* resultToken)
{
   if (tokStream->bufferedToken == NULL) {
      return read_tok(tokStream->stream, resultToken);
   } else {
      *resultToken = *tokStream->bufferedToken;
      free(tokStream->bufferedToken);
      tokStream->bufferedToken = NULL;
      return 1; 
   }
}

int peekToken(TokenStreamWithLookAhead* tokStream,
              Token* resultToken)
{
   int result;
   if (tokStream->bufferedToken == NULL) {
      result = read_tok(tokStream->stream, resultToken);
      tokStream->bufferedToken = (Token*)malloc(sizeof(Token));
      *(tokStream->bufferedToken) =  *resultToken;
      return result;
   } else {
      *resultToken = *tokStream->bufferedToken;
      tokStream->bufferedToken = NULL;
      return 0; 
   }
}

void releaseTokStream(TokenStreamWithLookAhead* tokStream)
{
   if (tokStream->bufferedToken != NULL) {
      free(tokStream->bufferedToken);
   }
}





