#include "hilfcc.h"

Token *token;

void error_at(char *loc, char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);

	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, " ");
	fprintf(stderr, "^ ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

bool startswith(char *p, char *q) {
	return memcmp(p, q, strlen(q)) == 0;
}

bool is_ident_head(char p) {
	return ('a' <= p && p <= 'z') || ('A' <= p && p <= 'Z') || p == '_';
}

bool is_ident_char(char p) {
	return is_ident_head(p) || ('0' <= p && p <= '9');
}

bool is_keyword(char *p, char *keyword) {
	int len = strlen(keyword);
	return strncmp(p, keyword, len) == 0 && !is_ident_char(p[len]);
}

Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
	Token *tok = calloc(1, sizeof(Token));
	tok->kind = kind;
	tok->str = str;
	tok->len = len;
	cur->next = tok;
	return tok;
}

void tokenize(char *user_input) {
	char *p = user_input;
	Token head;
	head.next = NULL;
	Token *cur = &head;
	while (*p) {
		if (isspace(*p)) {
			p++;
			continue;
		}
		if (startswith(p, "==") || startswith(p, "<=") || startswith(p, ">=") || startswith(p, "!=")) {
			cur = new_token(TK_RESERVED, cur, p, 2);
			p += 2;
			continue;
		}
		if (strchr("+-*/()<>=;{},&", *p)) {
			cur = new_token(TK_RESERVED, cur, p++, 1);
			continue;
		}
		if (is_keyword(p, "return")) {
			cur = new_token(TK_RETURN, cur, p, 6);
			p += 6;
			continue;
		}
		if (is_keyword(p, "if")) {
			cur = new_token(TK_IF, cur, p, 2);
			p += 2;
			continue;
		}
		if (is_keyword(p, "else")) {
			cur = new_token(TK_ELSE, cur, p, 4);
			p += 4;
			continue;
		}
		if (is_keyword(p, "while")) {
			cur = new_token(TK_WHILE, cur, p, 5);
			p += 5;
			continue;
		}
		if (is_keyword(p, "for")) {
			cur = new_token(TK_FOR, cur, p, 3);
			p += 3;
			continue;
		}
		if (is_ident_head(*p)) {
			char *start = p;
			int lvar_len = 0;
			do {
				p++; lvar_len++;
			} while(is_ident_char(*p));
			cur = new_token(TK_IDENT, cur, start, lvar_len);
			continue;
		}
		if (isdigit(*p)) {
			cur = new_token(TK_NUM, cur, p, 0) ;
			char *q = p;
			cur->val = strtol(p, &p, 10);
			cur->len = p - q;
			continue;
		}

		error_at(p, "トークン化できません");
	}
	new_token(TK_EOF, cur, p, 0);
	token = head.next;
}