#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"
#include"id_gens.h"

int find(char *target, char **array, int size) {
	for(int i = 0; i < size && array[i] != NULL; i++) {
		if(strcmp(target, array[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int main(int argc, char **argv){
	if(argc > 1) {
		if(freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "Fatal Error: Couldn't open file '%s'\n", argv[1]);
			return 1;
		}
	}
	IDGen get_id = id_gen_e; // TODO: add args to make this swapable

	int size = 0;
	char **tokens = NULL;
	char *macros = NULL;
	int result = get_tokens(&size, &tokens, &macros);
	if(result != 0) {
		fprintf(stderr, "Fatal Error: Couldn't get tokens. Error code: %d\n", result);
		return result;
	}
	fputs(macros, stdout);

	char **unique = calloc(size, sizeof(*unique));
	if(unique == NULL) return 1;
	char **new_ids = calloc(size, sizeof(*new_ids));
	if(new_ids == NULL) return 1;
	int new_ids_index = 0;

	for(int i = 0; i < size; i++) {
		// printf("%d: '%s'\n", i, tokens[i]);
		int index = find(tokens[i], unique, size);
		if(index == -1) {
			unique[new_ids_index] = tokens[i];
			char *new_id = NULL;
			do {
				new_id = get_id();
			} while(find(new_id, tokens, size) != -1);
			new_ids[new_ids_index] = strdup(new_id);
			if(new_ids[new_ids_index] == NULL) return 1;
			index = new_ids_index++;
		}
	}

	for(int i = 0; i < size && unique[i] != NULL; i++) {
		printf("#define %s %s\n", new_ids[i], unique[i]);
	}

	int line_length = 0;
	for(int i = 0; i < size; i++) {
		int index = find(tokens[i], unique, size);
		if(index < 0) return 3;
		line_length += printf("%s%s", line_length == 0 ? "" : " ", new_ids[index]);
		if(line_length >= 40) {
			line_length = 0;
			putchar('\n');
		}
	}

	for(int i = 0; i < size; i++) {
		free(tokens[i]);
	}
	free(tokens);
	free(macros);
	free(unique);
	for(int i = 0; i < size && new_ids[i] != NULL; i++) {
		free(new_ids[i]);
	}
	free(new_ids);
	return 0;
}
