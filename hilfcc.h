#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
	TK_RESERVED,
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_INT,
	TK_IDENT,
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

extern Token *token;
void error_at(char *loc, char *fmt, ...);
void error(char *fmt, ...);
void tokenize(char *user_input);

typedef struct LVar LVar;
struct LVar {
	LVar *next;
	char *name;
	int len;
	int offset;
};

typedef enum {
	ND_ADD,
	ND_SUB,
	ND_MUL,
	ND_DIV,
	ND_EQ,
	ND_NE,
	ND_LE,
	ND_LT,
	ND_ADDR,
	ND_DEREF,
	ND_ASSIGN,
	ND_RETURN,
	ND_IF,
	ND_WHILE,
	ND_FOR,
	ND_BLOCK,
	ND_FUNCALL,
	ND_VARDECL,
	ND_LVAR,
	ND_NUM,
} NodeKind;

typedef struct Node Node;
struct Node {
	NodeKind kind;
	Node *lhs;
	Node *rhs;
	Node *next;

	// if, for文用
	Node *cond;
	Node *then;
	Node *els;
	Node *init;
	Node *inc;

	// Block用
	Node *body;

	// Function Call用
	char *funcname;
	Node *args;

	// Num 用
	int val;

	// LVar 用
	LVar *var;
};

typedef struct Function Function;
struct Function {
	Function *next;
	char *name;
	int nargs;
	LVar *args;
	int stack_size;
	LVar *locals;	
	Node *body;
};

extern Function *function;
void parse();

void codegen(); 

extern char *user_input;