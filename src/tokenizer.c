#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"

#define MAX_TOKEN_SIZE 100

typedef enum {
	NEWLINE,
	START,
	IDENT,
	NUMBER_OP,
	INT,
	FLOAT,
	STRING,
	STRING_ESCAPE,
	COMMENT,
	IGNORE_LINE,
} State;

// return 0 if success
#define SUCCESS 0
// all other return codes are fail
// 1 - one or more inputs NULL
#define ERR_NUL 1
// 2 - memory allocation error
#define ERR_MEM 2
// 3 - token exceeds max size
#define ERR_TOK 3
// 4 - parsing error
#define ERR_PAR 4
// 5 - ungetc error
#define ERR_UGC 5
#define CHECK_UNGETC(ch) ({ if(ungetc(ch, stdin) == EOF) return ERR_UGC; })
int get_tokens(int *out_len, char ***out_array) {
	if(out_array == NULL || out_len == NULL) return ERR_NUL;
	if(*out_array == NULL) {
		*out_len = 20;
		*out_array = calloc(sizeof(*out_array), *out_len);
		if(*out_array == NULL) return ERR_MEM;
	}
	State state = NEWLINE;
	int prev;
	char token[MAX_TOKEN_SIZE + 1];
	bzero(token, sizeof(token) / sizeof(*token));
	int token_index = 0;
	int array_index = 0;
	while(1) {
		if(token_index > MAX_TOKEN_SIZE) return ERR_TOK;
		int ch = getchar();
		switch(state){
		case NEWLINE:
			switch(ch){
			case '/':
				state = COMMENT;
				break;
			case '#':
				state = IGNORE_LINE;
				break;
			case ' ':
			case '\t':
			case '\r':
			case '\n':
			case EOF:
				break;
			default:
				CHECK_UNGETC(ch);
				state = START;
				break;
			}
			break;
		case START:
			switch(ch){
			case '~':
			case '|':
			case '}':
			case '{':
			case ']':
			case '[':
			case '/':
			case ',':
			case '(':
			case ')':
			case '.':
				token[token_index] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				token[token_index++] = '\0';
				break;
			case 'a'...'z':
			case 'A'...'Z':
			case '_':
				token[token_index++] = ch;
				state = IDENT;
				break;
			case '"':
			case '\'':
				token[token_index++] = ch;
				state = STRING;
				break;
				break;
			case '-':
			case '+':
				token[token_index++] = ch;
				state = NUMBER_OP;
				break;
			case '0'...'9':
				token[token_index++] = ch;
				state = INT;
				break;
			case '\r':
			case '\n':
				state = NEWLINE;
				break;
			case '$':
				return ERR_PAR;
			}
			break;
		case IDENT:
			switch(ch) {
			case 'a'...'z':
			case 'A'...'Z':
			case '_':
			case '0'...'9':
				token[token_index++] = ch;
				break;
			case EOF:
				break;
			default:
				CHECK_UNGETC(ch);
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
				break;
			}
			break;
		case NUMBER_OP:
			if(ch >= '0' && ch <= '9') {
				token[token_index++] = ch;
				break;
			}
			if(
				(token[0] == '+' && ch == '+') ||
				(token[0] == '-' && (
					ch == '>' ||
					ch == '-'
				))
			){
				token[token_index++] = ch;
			}else if(ch != EOF){
				CHECK_UNGETC(ch);
			}
			(*out_array)[array_index] = strdup(token);
			if((*out_array)[array_index++] == NULL) return ERR_MEM;
			bzero(token, sizeof(token) / sizeof(*token));
			token_index = 0;
			state = START;
			break;
		case INT:
			switch(ch){
			case '.':
				state = FLOAT;
			case '0'...'9':
				token[token_index++] = ch;
				break;
			case 'a'...'z':
			case 'A'...'Z':
			case '_':
				return ERR_PAR;
			case EOF:
				break;
			default:
				CHECK_UNGETC(ch);
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
				break;
			}
			break;
		case FLOAT:
			break;
		case STRING:
			break;
		case STRING_ESCAPE:
			break;
		case COMMENT:
			break;
		case IGNORE_LINE:
			break;
		}
		prev = ch;
		if(ch == EOF) break;
		if(array_index == *out_len) {
			*out_len *= 2;
			*out_array = realloc(*out_array, sizeof(*out_array) * *out_len);
			if(*out_array == NULL) return ERR_MEM;
		}
	}
	*out_len = array_index;
	*out_array = realloc(*out_array, sizeof(*out_array) * *out_len);
	if(*out_array == NULL) return ERR_MEM;
	return SUCCESS;
}
