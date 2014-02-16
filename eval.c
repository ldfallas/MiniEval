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

int getCharFromStream(TokenStreamWithLookAhead* file) {
  switch(file->kind)
  {
    case TOK_STREAM_KIND_FILE:
      return fgetc(file->stream);
    case TOK_STREAM_KIND_STRING:
      if ((file->buffer.position + 1) < file->buffer.maxValue) {
        file->buffer.position++;
        return (int)file->buffer.content[file->buffer.position];
      }
      else {
        return -1;
      } 
    default:
       return -1;
  }
}

int ungetCharFromStream(TokenStreamWithLookAhead* file, int c) {
  switch(file->kind)
  {
    case TOK_STREAM_KIND_FILE:
      return ungetc(c, file->stream);
    case TOK_STREAM_KIND_STRING:
      if ((file->buffer.position - 1) >= 0) {
        file->buffer.position--;
        return 0;
      }
      else {
        return -1;
      } 
    default:
       return -1;
  }
}



int read_tok(TokenStreamWithLookAhead* file, Token* tok) {

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
     theChar = getCharFromStream(file);
      c = theChar; 
      switch(state) {
         case BLANK_STATE:
            if (isdigit(c)) {
               buffer[buffpos++] = (char)c;
               state = NUMBER_STATE;
               tok->id = TokNumeric;
            } else if (isalpha(c)) {

               buffer[buffpos++] = (char)c;
               state = ID_STATE;
               tok->id = TokVariable;
            } else if (isoperator(c)) {
               buffer[buffpos++] = (char)c;
               buffer[buffpos++] = '\0';
               stop = 1;
               tok->id = TokOperator;
            } else if (isparenthesis(c)) {
               buffer[buffpos++] = (char)c;
               buffer[buffpos++] = '\0';
               stop = 1;
               tok->id = TokPar;
            } else if (isspace(c) && theChar != EOF) {
               continue;
            } else {
              result = -1;
              return result;
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
               ungetCharFromStream(file, c);
               stop = 1;
            }
            break;
         case ID_STATE:
            if ((isletter(c) || isdigit(c)) && (buffpos < MAX_BUFF))
            {
               buffer[buffpos++] = c;
            } else  {
               buffer[buffpos++] = '\0';
               ungetCharFromStream(file, c);
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
   return expr->id == ExprAdditionNode;
}



Expr* createBinaryOperation(ExprNodeType kind,Expr* expr1, Expr* expr2) {
   Expr* result;
   BinExpr* inner;

   result = malloc(sizeof(Expr));
   result->id = kind;
   inner = malloc(sizeof(BinExpr));
   result->payload = inner;
   inner->left = expr1;
   inner->right = expr2;

   return result;
}


Expr* createAddition(Expr* expr1, Expr* expr2) {
  return createBinaryOperation(ExprAdditionNode, expr1, expr2);
}

Expr* createDivision(Expr* expr1, Expr* expr2) {
  return createBinaryOperation(ExprDivisionNode ,expr1 ,expr2);
}

Expr* createMultiplication(Expr* expr1, Expr* expr2) {
  return createBinaryOperation(ExprMultiplicationNode ,expr1 ,expr2);
}

Expr* createSubtraction(Expr* expr1, Expr* expr2) {
  return createBinaryOperation(ExprSubtractionNode ,expr1 ,expr2);
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
      case ExprAdditionNode: 
         x = evaluateExpression(getLeftExprFromBin(expr));
         y = evaluateExpression(getRightExprFromBin(expr));
         return x + y;
         break;
      case ExprNumLiteralNode:
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
   result->id = ExprNumLiteralNode; 
   result->innerValue = value;

   return result;
}

ExprNodeType getNodeTypeFromOperator(const char op) {
  switch(op) {
    case '*':
      return ExprMultiplicationNode;
    case '/':
      return ExprDivisionNode;
    case '-':
      return ExprSubtractionNode;
    case '+':
      return ExprAdditionNode;
    default:
      perror("Unexpected operator char");
      return 0;
  }                                             
} 

int getOperatorFromNodeType(const ExprNodeType nodeType, char* op) {
  int result;
  result = 0;
  switch(nodeType) {
    case ExprMultiplicationNode:
      *op =  '*';
      break;
    case ExprDivisionNode:
      *op = '/';
      break;
    case ExprSubtractionNode:
      *op =  '-';
      break;
    case ExprAdditionNode:
      *op = '+';
      break;
    default:
      *op = '?';
      result = 1;
      break;
  }     
  return result;
} 


void deepReleaseExpr(Expr* expr)
{
   switch(expr->id)
   {
      case ExprAdditionNode: 
      case ExprSubtractionNode: 
      case ExprMultiplicationNode: 
      case ExprDivisionNode: 
         deepReleaseExpr(getLeftExprFromBin(expr));
         deepReleaseExpr(getRightExprFromBin(expr));
         free(expr->payload);
         free(expr);
         break;
      case ExprNumLiteralNode:
         free(expr);
         break;
   }

}

void printExpr(Expr* expr, OutStream* out)
{
   char operatorChar;
   switch(expr->id)
   {
      case ExprAdditionNode: 
      case ExprSubtractionNode: 
      case ExprMultiplicationNode: 
      case ExprDivisionNode: 
         printToOutStream(out,2, "<");
         printExpr(getLeftExprFromBin(expr), out);
         getOperatorFromNodeType(expr->id, &operatorChar);
         printToOutStream(out, 4, " %c ", operatorChar);
         printExpr(getRightExprFromBin(expr), out);
         printToOutStream(out, 5, ">");
         break;
      case ExprNumLiteralNode:
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
   Token peekedToken, nextToken;
   int readResult, nextTokenReadResult, parseResult;
   Expr* innerExpr;

   readResult = peekToken(stream, &peekedToken);
   if (readResult == 0)
   {
     if (peekedToken.id == TokNumeric) {
       readResult = readToken(stream, &peekedToken);
       *expr = createNumLiteral(atof(peekedToken.buffer)); 
       parseResult = 0;
     }
   } else {
     parseResult = parseParenExpr(stream, expr);
   }
   return parseResult;
}

int nextTokenIsOperatorWithName(
	TokenStreamWithLookAhead* stream,
	const char* operatorText) {
   Token peekedToken;
   int readResult, result;
   result = 0;
   readResult = peekToken(stream, &peekedToken);
   if (readResult == 0
       && peekedToken.id == TokOperator
       && strncmp(peekedToken.buffer, operatorText, 1) == 0)
   {
     result = 1;
   }
   return result;
}

int parseParenExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken, nextToken;
   int readResult, 
       nextTokenReadResult,
       secondExprResult,
       innerParseResult;
   Expr* innerExpr;

   readResult = peekToken(stream, &peekedToken);
   if (readResult == 0)
   {
      if (peekedToken.id == TokPar 
          && strncmp(peekedToken.buffer,"(",2) == 0) {
         readToken(stream, &peekedToken);
         innerParseResult = parseExpr(stream,&innerExpr);
         if(innerParseResult == 0
	    && (readResult = readToken(stream, &peekedToken))
            && peekedToken.id == TokPar
            && strncmp(peekedToken.buffer, ")",2)) {
           *expr = innerExpr;
            return 0;
         } else {
            return -1;
         }
      }
   }
}
int parseMultiExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken, operatorToken;
   int readResult, 
       nextTokenReadResult,
       secondExprResult,
       parseResult,
       firstParseResult;
   Expr *innerExpr1, *innerExpr2;
   
   parseResult = -1;
   firstParseResult = parseSingleExpr(stream, &innerExpr1);
   if (firstParseResult == 0) {
      if (nextTokenIsOperatorWithName(stream, "*")
            || nextTokenIsOperatorWithName(stream, "/")) {
         readResult = readToken(stream, &operatorToken);
         secondExprResult = parseMultiExpr(stream, &innerExpr2);
         if (secondExprResult == 0) {
             *expr = createBinaryOperation(
                         getNodeTypeFromOperator(operatorToken.buffer[0]), 
                         innerExpr1, 
                         innerExpr2);
             parseResult = 0;

         }
      } else {
         *expr = innerExpr1;
         parseResult = 0;
      }
   }
   return parseResult;
}

int parseExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken, operatorToken;
   int readResult, 
       nextTokenReadResult,
       secondExprResult,
       parseResult,
       firstParseResult;
   Expr *innerExpr1, *innerExpr2;
   
   parseResult = -1;
   firstParseResult = parseMultiExpr(stream, &innerExpr1);
   if (firstParseResult == 0) {
      if (nextTokenIsOperatorWithName(stream, "+")
            || nextTokenIsOperatorWithName(stream, "-")) {
         readResult = readToken(stream, &operatorToken);
         secondExprResult = parseExpr(stream, &innerExpr2);
         if (secondExprResult == 0) {
             *expr = createBinaryOperation(
                         getNodeTypeFromOperator(operatorToken.buffer[0]), 
                         innerExpr1, 
                         innerExpr2);
             parseResult = 0;

         }
      } else {
         *expr = innerExpr1;
         parseResult = 0;
      }
   }
   return parseResult;
}

int parseExpr__(
        TokenStreamWithLookAhead* stream, 
        Expr** expr) {
   Token peekedToken, nextToken;
   int readResult, nextTokenReadResult,secondExprResult;
   Expr* innerExpr;

   readResult = peekToken(stream, &peekedToken);
   if (readResult == 0)
   {
      if (peekedToken.id == TokPar 
          && strncmp(peekedToken.buffer,"(",2) == 0) {
         readToken(stream, &peekedToken);
         parseExpr(stream,&innerExpr);


         if((readResult = readToken(stream, &peekedToken))
            && peekedToken.id == TokPar
            && strncmp(peekedToken.buffer, ")",2)) {
           *expr = innerExpr;
         printf("2,");
            return 0;
         } else {
         printf("3, -- 00%d ",readResult);
            return -1;
         }
      } else if (peekedToken.id == TokNumeric) {
         readResult = readToken(stream, &peekedToken);
         nextTokenReadResult = peekToken(stream, &nextToken);
         if (nextTokenReadResult == 0
             && nextToken.id == TokOperator
	     && strncmp(nextToken.buffer,"+", 1) == 0)
         {
           *expr = createNumLiteral(atof(peekedToken.buffer));
           readToken(stream, &nextToken);
           secondExprResult = parseExpr(stream, &innerExpr);
           if (secondExprResult == 0) {
             *expr = createBinaryOperation(
                              getNodeTypeFromOperator(nextToken.buffer[0]), *expr, innerExpr);
             return 0;
           } else {
             return 0;
           }
         } else {
           *expr = createNumLiteral(atof(peekedToken.buffer)); 
           return 0;
         }

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
  result.kind = TOK_STREAM_KIND_FILE;
  result.stream = file;
  result.bufferedToken = NULL;
  return result;
}

TokenStreamWithLookAhead createTokenStreamWithLookAheadFromString(char* content)
{
  TokenStreamWithLookAhead result;
  result.kind = TOK_STREAM_KIND_STRING;  
  result.buffer.position = -1;
  result.buffer.maxValue = strlen(content);
  result.buffer.content = content;
  result.bufferedToken = NULL;
  return result;
}

int readToken(TokenStreamWithLookAhead* tokStream,
              Token* resultToken)
{
   if (tokStream->bufferedToken == NULL) {
      return read_tok(tokStream, resultToken);
   } else {
      /**resultToken = *tokStream->bufferedToken;*/
      memcpy(resultToken, tokStream->bufferedToken, sizeof(Token));
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
      result = read_tok(tokStream, resultToken);
      tokStream->bufferedToken = (Token*)malloc(sizeof(Token));
      *(tokStream->bufferedToken) =  *resultToken;
      return result;
   } else {
      /* *resultToken = *tokStream->bufferedToken; */
      memcpy(resultToken, tokStream->bufferedToken, sizeof(Token));
      /*tokStream->bufferedToken = NULL;*/
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
      vsnprintf(tmpBuffer + stream->size,
                maxSize, 
                format, 
                arglist);
      /* workaround for vsnprintf return type behavior*/
      printedSize = strnlen(tmpBuffer + stream->size, maxSize);

      free(stream->buffer);
      stream->buffer = tmpBuffer;
      stream->size = stream->size + printedSize;
      stream->capacity = newCapacity;
    } else {
      vsnprintf(stream->buffer + stream->size,
                maxSize, 
                format, 
                arglist);
      /* workaround for vsnprintf return type behavior*/
      printedSize = strnlen(stream->buffer + stream->size, maxSize);
      stream->size = stream->size + printedSize;
      stream->buffer[stream->size] = '\0';
    }
  }
  va_end( arglist );
}

