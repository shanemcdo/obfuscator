#include<stdio.h>
#include"tokenizer.h"

int main(int argc, char **argv){
	if(argc > 1) {
		if(freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "Fatal Error: Couldn't open file '%s'\n", argv[1]);
			return 1;
		}
	}
	char **tokens = NULL;
	int size = 0;
	int result = get_tokens(&size, &tokens);
	if(result != 0) {
		fprintf(stderr, "Fatal Error: Couldn't get tokens. Error code: %d\n", result);
		return result;
	}
	for(int i = 0; i < size; i++) {
		printf("%d: %s\n", i, tokens[i]);
	}

    return 0;
}
