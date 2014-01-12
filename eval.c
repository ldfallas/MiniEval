#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "eval.h"


/* forward declarations */
int peekToken(TokenStreamWithLookAhead* tokStream,
              Token* resultToken);



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

void printExpr(Expr* expr, OutStream* out)
{
  
   switch(expr->id)
   {
      case ADDITION_NODE: 
	 printToOutStream(out,2, "<");
	 printExpr(getLeftExprFromBin(expr), out);
         printToOutStream(out, 4, " + ");
	 printExpr(getRightExprFromBin(expr), out);
         printToOutStream(out, 5, ">");
         break;
      case NUM_LITERAL_NODE:
	 printToOutStream(out, 10, "%g", expr->innerValue);
         break;
      default:
	 printToOutStream(out, 10, "?? %d\n",expr->id);
   }
}


int parse(FILE* inputFile, Expr** expr)
{
  /* read_tok(aFile, &t)*/
  /* SINGLEEXPR = lit | '(' MULTEXPR ')' */
  /* SUMEXPR = SINGLEEXPR ('-'|'+' SUMEXPR)? */
  /* MULTEXPR = SUMEXPR ('*'|'/' MULTEXPR)? */
  return -1;
}

int parseSingleExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken;
   int readResult;
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
  return -1;
}
int parseSumExprExpression(
       TokenStreamWithLookAhead* stream, 
       Expr** expr) {
  return -1;
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


OutStream createFileOutStream(FILE* outStream) 
{
  OutStream result;
  result.kind = FILE_OUT_STREAM;
  result.innerOutStream = outStream;
  return result;
}

OutStream createStringOutStream(int initialCapacity) {
  OutStream result;
  if (initialCapacity  > 0) {
    result.kind = STRING_OUT_STREAM;
    result.buffer = (char*)malloc(sizeof(char) * initialCapacity + 1);

    if (result.buffer != NULL) {
      result.buffer[0] = '\0';
      result.capacity = initialCapacity + 1;
      result.size = 0;
    }
  }
  return result;
}
char* getStringFromStringOutStream(const OutStream* stream) { 
  if (stream->kind == STRING_OUT_STREAM) {
    return stream->buffer;
  } else {
    return NULL;
  }
}
void destroyOutStream(const OutStream* stream) {
  if (stream->kind == FILE_OUT_STREAM) {
    fclose(stream->innerOutStream);
  } else if (stream->kind == STRING_OUT_STREAM) {
    free(stream->buffer);
  }
}


inline int _max(int x, int y) {
  return x > y ? x : y;
}


void printToOutStream( OutStream* stream,int maxSize, const char* format, ...) {
  va_list arglist;
  int actualCapacity, newCapacity, printedSize;
  char* tmpBuffer;

  va_start( arglist, format);
  if (stream->kind == FILE_OUT_STREAM) {
    vfprintf(stream->innerOutStream, format, arglist);
  } else if (stream->kind == STRING_OUT_STREAM) {
    actualCapacity = stream->capacity - stream->size - 1;
    if (maxSize > actualCapacity) {
      newCapacity = _max((stream->capacity * 2) + 1,
			 stream->capacity + maxSize + 1);
      tmpBuffer = (char*)malloc(newCapacity);
      strncpy(tmpBuffer, stream->buffer, stream->size);
      /*TODO what happens in malloc fails????...*/
      printedSize = vsnprintf(tmpBuffer + stream->size,
			      maxSize, 
			      format, 
			      arglist);
      free(stream->buffer);
      stream->buffer = tmpBuffer;
      stream->size = stream->size + printedSize;
      stream->capacity = newCapacity;
    } else {
      printedSize = vsnprintf(stream->buffer + stream->size,
			      maxSize, 
			      format, 
			      arglist);
      stream->size = stream->size + printedSize;
      stream->buffer[stream->size] = '\0';
    }
  }
  va_end( arglist );
}

