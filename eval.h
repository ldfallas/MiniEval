#ifndef __EVAL_H
#define __EVAL_H

#define MAX_NUM 20
#define MAX_ID 20
#define MAX_BUFF 20

#define BLANK_STATE 100
#define NUMBER_STATE 120
#define ID_STATE 130

#define OP_ADD  100
#define OP_SUB  101
#define OP_MULT 102
#define OP_DIV  103

typedef enum {
  ExprAdditionNode = 1000,
  ExprNumLiteralNode,
} ExprNodeType;


typedef enum {
  TokNumeric,
  TokVariable,
  TokPar,
  TokOperator,
  TokEOF
} TokenKind;

typedef struct _Tok {
   char buffer[MAX_BUFF + 1]; 
   TokenKind id;
} Token;

typedef struct {
  int kind;
  union {
    FILE* stream;
    struct {
      int position;
      int maxValue;
      char* content;
    } buffer;
  };
  Token* bufferedToken; 
} TokenStreamWithLookAhead;

#define TOK_STREAM_KIND_FILE   100
#define TOK_STREAM_KIND_STRING 101

typedef struct {
   int id;
   union {
      void* payload;
      double innerValue;
   };
} Expr;

typedef struct {
   Expr* left;
   Expr* right;
} BinExpr;


#define FILE_OUT_STREAM   1
#define STRING_OUT_STREAM 2
typedef struct {
  int kind;
  union {
    struct {
      char* buffer;
      int capacity;
      int size;
    };
    FILE* innerOutStream;
  };  
} OutStream;

OutStream createFileOutStream(FILE* outStream);
OutStream createStringOutStream(int initialSize);
char* getStringFromStringOutStream(const OutStream* stream);
void destroyOutStream(const OutStream* stream);
void printToOutStream( OutStream* stream, int maxSize, const char* format, ...);

TokenStreamWithLookAhead createTokenStreamWithLookAhead(FILE* file);
TokenStreamWithLookAhead createTokenStreamWithLookAheadFromString(char* content);
int parse(FILE* inputFile, Expr** expr);
double evaluateExpression(Expr* expr);


Expr* createAddition(Expr* expr1,Expr* expr2);
Expr* createNumLiteral(double value);

void releaseTokStream(TokenStreamWithLookAhead* tokStream);
int parseSingleExpr(
        TokenStreamWithLookAhead* stream, 
        Expr** expr);

int readToken(TokenStreamWithLookAhead* tokStream,
              Token* resultToken);

void printExpr(Expr* expr,  OutStream* out);
void deepReleaseExpr(Expr* expr);

int read_tok(TokenStreamWithLookAhead* file, Token* tok) ;
#endif
