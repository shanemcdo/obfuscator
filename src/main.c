#include<stdio.h>

int main(int argc, char **argv){
	if(argc > 1) {
		if(freopen(argv[1], "r", stdin) == NULL) {
			fprintf(stderr, "Fatal Error: Couldn't open file '%s'\n", argv[1]);
			return 1;
		}
	}
    return 0;
}
