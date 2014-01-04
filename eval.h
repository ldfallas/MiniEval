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

#define ADDITION_NODE 1001
#define NUM_LITERAL_NODE  1002


typedef struct _Tok {
   char buffer[MAX_BUFF + 1]; 
   int id;
} Token;

typedef struct {
   FILE* stream;
   Token* bufferedToken; 
} TokenStreamWithLookAhead;


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


TokenStreamWithLookAhead createTokenStreamWithLookAhead(FILE* file);
int parse(FILE* inputFile, Expr** expr);



Expr* createAddition(Expr* expr1,Expr* expr2);
Expr* createNumLiteral(double value);


#endif
