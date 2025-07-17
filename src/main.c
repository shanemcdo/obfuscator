#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"
#include"id_gens.h"

int main(int argc, char **argv){
	if(argc > 1) {
		if(freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "Fatal Error: Couldn't open file '%s'\n", argv[1]);
			return 1;
		}
	}

	for(int i = 0; i < 10; i++){
		printf("%d: %s\n", i, id_gen_e());
	}

	int size = 0;
	char **tokens = NULL;
	char *macros = NULL;
	int result = get_tokens(&size, &tokens, &macros);
	if(result != 0) {
		fprintf(stderr, "Fatal Error: Couldn't get tokens. Error code: %d\n", result);
		return result;
	}
	printf("macros: %lu\n%s", strlen(macros), macros);
	for(int i = 0; i < size; i++) {
		printf("%d: '%s'\n", i, tokens[i]);
	}
	free(tokens);
	free(macros);
	return 0;
}
