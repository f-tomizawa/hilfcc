#include "hilfcc.h"

Function *function;
LVar *locals;

bool consume(char *op) {
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
	  return false;
	token = token->next;
	return true;
}

void expect(char *op) {
	if (token->kind != TK_RESERVED ||
		strlen(op) != token->len ||
		memcmp(token->str, op, token->len))
	  error_at(token->str, "'%c'ではありません", op);
	token = token->next;
}

Token *consume_kind(TokenKind kind) {
	if (token->kind != kind)
		return NULL;
	Token *token_tmp = token;
	token = token->next;
	return token_tmp;
}

LVar *find_lvar(Token *tok) {
	for (LVar *var = locals; var; var = var->next) 
		if (var->len == tok->len && !memcmp(var->name, tok->str, var->len))
			return var;
	return NULL;
}

int expect_number() {
	if (token->kind != TK_NUM)
		error_at(token->str, "数ではありません");
	int val = token->val;
	token = token->next;
	return val;
}

bool at_eof() {
	return token->kind == TK_EOF;
}

LVar *new_lvar(Token *tok) {
	LVar *lvar = calloc(1, sizeof(LVar));
	lvar->next = locals;
	lvar->name = tok->str;
	lvar->len = tok->len;
	lvar->offset = locals->offset + 8;
	locals = lvar;
	return lvar;
}

Node *new_node(NodeKind kind) {
	Node *node = calloc(1, sizeof(Node));
	node->kind = kind;
	return node;
}

Node *new_binary(NodeKind kind, Node *lhs, Node *rhs) {
	Node *node = new_node(kind);
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_num(int val) {
	Node *node = new_node(ND_NUM);
	node->val = val;
	return node;
}

void program();
Function *function_declare();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

/** 
 * program = function-declare*
**/

void program() {
	Function head = {};
	Function *cur = &head;
	while (!at_eof()) {
		cur = cur->next = function_declare();
	}
	function = head.next;
	return;
}

/**
 * function-declare = ident "(" (ident ("," ident)*?)? ")" "{" stmt "}"
**/
Function *function_declare() {
	locals = calloc(1, sizeof(LVar));
	Function *function = calloc(1, sizeof(Function));
	Token *tok = consume_kind(TK_IDENT);
	if (!tok) {
		error_at(tok->str,  "関数定義ではありません\n");
	}
	function->name = calloc(tok->len, sizeof(char));
	strncpy(function->name, tok->str, tok->len);
	expect("(");
	expect(")");
	expect("{");
	Node *node = new_node(ND_BLOCK);
	Node head = {};
	Node *cur = &head;
	while(!consume("}"))
		cur = cur->next = stmt();
	node->body = head.next;
	function->body = node;
	function->locals = locals;
	return function;
}

/** 
 * stmt = "return" expr ";"
 *      | "if" "(" expr ")" stmt ("else" stmt)?
 *      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
 *      | "while" "(" expr ")" stmt
 *      | "{" expr "}" 
 *      | expr ";"
**/

Node *stmt() {
	Node *node;

	if (consume("{")) {
		Node head = {};
		Node *cur = &head;
		while(!consume("}"))
			cur = cur->next = stmt();
		node = new_node(ND_BLOCK);
		node->body = head.next;
		return node;
	}

	if (consume_kind(TK_IF)) {
		node = new_node(ND_IF);
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
		if (consume_kind(TK_ELSE)) 
			node->els = stmt();
		else
			node->els = NULL;
		return node;
	}
	if (consume_kind(TK_WHILE)) {
		node = new_node(ND_WHILE);
		expect("(");
		node->cond = expr();
		expect(")");
		node->then = stmt();
		return node;
	}
	if (consume_kind(TK_FOR)) {
		node = new_node(ND_FOR);
		expect("(");
		if (consume(";")) {
			node->init = NULL;
		} else {
			node->init = expr();
			expect(";");
		}
		if (consume(";")) {
			node->cond = NULL;
		} else {
			node->cond = expr();
			expect(";");
		}
		if (consume(")")) {
			node->inc = NULL;
		} else {
			node->inc = expr();
			expect(")");
		}
		node->then = stmt();
		return node;
	}
	if (consume_kind(TK_RETURN)) {
		node = new_node(ND_RETURN);
		node->lhs = expr();
	} else {
		node = expr();
	}
	if (!consume(";"))
		error_at(token->str, "';'ではないトークンです");
	return node;
}

Node *expr() {
	return assign();
}

Node *assign() {
	Node *node = equality();
	for (;;) {
		if (consume("="))
			node = new_binary(ND_ASSIGN, node, assign());
		else
			return node;
	}
}

Node *equality() {
	Node *node = relational();
	for (;;) {
		if (consume("=="))
			node = new_binary(ND_EQ, node, relational());
		else if (consume("!="))
			node = new_binary(ND_NE, node, relational());
		else
			return node;
	}
}

Node *relational() {
	Node *node = add();
	for (;;) {
		if (consume("<"))
			node = new_binary(ND_LT, node, add());
		else if (consume("<="))
			node = new_binary(ND_LE, node, add());
		else if (consume(">"))
			node = new_binary(ND_LT, add(), node);
		else if (consume(">="))
			node = new_binary(ND_LE, add(), node);
		else
			return node;
	}
}

Node *add() {
	Node *node = mul();
	for (;;) {
		if (consume("+"))
			node = new_binary(ND_ADD, node, mul());
		else if (consume("-"))
			node = new_binary(ND_SUB, node, mul());
		else
			return node;
	}
}

Node *mul() {
	Node *node = unary();
	for (;;) {
		if (consume("*"))
			node = new_binary(ND_MUL, node, unary());
		else if (consume("/"))
			node = new_binary(ND_DIV, node, unary());
		else
			return node;
	}
}

Node *unary() {
	if (consume("+"))
		return unary();
	else if (consume("-"))
		return new_binary(ND_SUB, new_num(0), unary());
	return primary();
}

Node *primary() {
	if (consume("(")) {
		Node *node = expr();
		expect(")");
		return node;
	}
	Token *tok = consume_kind(TK_IDENT);
	if (tok) {
		if (consume("(")) {
			// identifier の次に "(" が来たら関数呼び出し
			Node *node = new_node(ND_FUNCALL);
			node->funcname = calloc(tok->len, sizeof(char));
			strncpy(node->funcname, tok->str, tok->len);
			Node head = {};
			Node *cur = &head;
			if (!consume(")")) {
				do {
					cur = cur->next = expr();
				} while (consume(","));
				expect(")");
			}
			cur->next = NULL;
			node->args = head.next;
			return node;
		}
		Node *node = new_node(ND_LVAR);

		LVar *lvar = find_lvar(tok);
		if (!lvar) {
			lvar = new_lvar(tok);
		}
		node->offset = lvar->offset;
		return node;
	}
	return new_num(expect_number());
}

void parse() {
	program();
}