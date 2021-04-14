#include "hilfcc.h"

char *user_input;

int main(int argc, char **argv) {
	if (argc != 2) {
		error("引数の個数が正しくありません\n");
		return 1;
	}

	user_input = argv[1];
	tokenize(user_input);
	parse();
	codegen();

	return 0;
}
