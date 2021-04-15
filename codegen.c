#include "hilfcc.h"

static char *argreg[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static int count() {
	static int i = 0;
	return i++;
}

void gen_lval(Node *node) {
	if (node->kind != ND_LVAR)
		error("代入の左辺値が変数ではありません");
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}

void gen(Node *node) {
	switch (node->kind) {
		case ND_BLOCK:
			for (Node *cur = node->body; cur; cur = cur->next) {
				gen(cur);
				printf("	pop rax\n");
			}
			return;
		case ND_RETURN:
			gen(node->lhs);
			printf("	pop rax\n");
			printf("	mov rsp, rbp\n");
			printf("	pop rbp\n");
			printf("	ret\n");
			return;
		case ND_IF: {
			gen(node->cond);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n"); // condの評価結果がfalseの場合Lelseにジャンプ
			int index = count();
			printf("	je .L.else.%d\n", index);
			gen(node->then);
			printf("	jmp .L.end.%d\n", index);
			printf(".L.else.%d:\n", index);
			if (node->els)
				gen(node->els);
			printf(".L.end.%d:\n", index);
			return;
		}
		case ND_WHILE: {
			int index = count();
			printf(".L.begin.%d:\n", index);
			gen(node->cond);
			printf("	pop rax\n");
			printf("	cmp rax, 0\n");
			printf("	je .L.end.%d\n", index);
			gen(node->then);
			printf("	jmp .L.begin.%d\n", index);
			printf(".L.end.%d:\n", index);
			return;
		}
		case ND_FOR: {
			int index = count();
			if (node->init)
				gen(node->init);
			printf(".L.begin.%d:\n", index);
			if (node->cond) {
				gen(node->cond);
				printf("	pop rax\n");
				printf("	cmp rax, 0\n");
				printf("	je .L.end.%d\n", index);
			}
			gen(node->then);
			if (node->inc)
				gen(node->inc);
			printf("	jmp .L.begin.%d\n", index);
			printf(".L.end.%d:\n", index);
			return;
		}
		case ND_NUM:
			printf("	push %d\n", node->val);
			return;
		case ND_LVAR:
			// 変数をスタック先頭アドレスに保存されている数値に置き換える操作
			gen_lval(node);
			printf("	pop rax\n");
			printf("	mov rax, [rax]\n");
			printf("	push rax\n");
			return;
		case ND_ASSIGN:
			// 右辺の評価結果を左辺の変数に対応するメモリアドレスに保存
			gen_lval(node->lhs);
			gen(node->rhs);

			printf("	pop rdi\n");
			printf("	pop rax\n");
			printf("	mov [rax], rdi\n");
			printf("	push rdi\n");
			return;
		case ND_FUNCALL: {
			// 引数を処理して各レジスタに格納
			int narg = 0;
			for (Node *arg = node->args; arg && narg < 6; arg = arg->next) {
				gen(arg);
				narg++;
			}
			for (int i = narg - 1; i >= 0; i--) {
				printf("	pop %s\n", argreg[i]);
			}

			// スタックポインタが16の倍数となるよう調整
			int index = count();
			printf("	mov r10, rdi\n");
			printf("	mov r11, rdx\n");
			printf("	mov rdi, rsp\n");
			printf("	push rdi\n");
			printf("	mov rax, 16\n");
			printf("	cqo\n");
			printf("	idiv rdi\n");
			printf("	cmp rdx, 0\n");
			printf("	jne .L.dest.%d\n", index);
			printf("	push rdi\n");
			printf(".L.dest.%d:\n", index);

			printf("	mov rdi, r10\n");
			printf("	mov rdx, r11\n");
			printf("	call %s\n", node->funcname);

			// 調整したスタックポインタを戻す
			printf("	pop rdi\n");
			printf("	mov rsp, rdi\n");
			
			printf("	push rax\n");
			return;
		}
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("	pop rdi\n");
	printf("	pop rax\n");

	switch (node->kind) {
	case ND_ADD:
		printf("	add rax, rdi\n");
		break;
	case ND_SUB:
		printf("	sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("	imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("	cqo\n");
		printf("	idiv rdi\n");
		break;
	case ND_EQ:
		printf("	cmp rax, rdi\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_NE:
		printf("	cmp rax, rdi\n");
		printf("	setne al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_LT:
		printf("	cmp rax, rdi\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
		break;
	case ND_LE:
		printf("	cmp rax, rdi\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
		break;
	}

	printf("	push rax\n");
}

void codegen() {
	printf(".intel_syntax noprefix\n");
	printf(".globl main\n");

	for (Function *cur = function; cur; cur = cur->next) {
		printf("%s:\n", cur->name);
		printf("	push rbp\n");
		printf("	mov rbp, rsp\n");
		printf("	sub rsp, 208\n");
		gen(cur->body);
		printf("	pop rax\n");
	}
	
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	printf("	ret\n");
}