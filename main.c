#include "hilfcc.h"

char *user_input;

int main(int argc, char **argv) {
	if (argc != 2) {
		error("引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	Token *token = tokenize(user_input);
	Node *node = parse(token);

	codegen(node);

	return 0;
}
