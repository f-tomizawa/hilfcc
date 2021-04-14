#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
	TK_RESERVED,
	TK_NUM,
	TK_EOF,
} TokenKind;

typedef struct Token Token;
struct Token {
	TokenKind kind;
	Token *next;
	int val;
	char *str;
	int len;
};

void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);

Token *tokenize(char *user_input);


typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_LE,
	ND_LT,
	ND_NUM,
} NodeKind;

typedef struct Node Node;
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	int val;
};


Node *parse(Token *tok);


void codegen(Node *node); 


extern char *user_input;