#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"tokenizer.h"

#define MAX_TOKEN_SIZE 100

// TODO: KNOWN BUG: Name collisions can occur with already existing names in #define macros
// TODO: KNOWN BUG: Function-like macros will not work because there cannot be a space between name and PAREN

typedef enum {
	NEWLINE,
	START,
	IDENT,
	AND_OP,
	OR_OP,
	LT_OP,
	GT_OP,
	EQ_OP,
	LBS_OP,
	RBS_OP,
	NUMBER_OP,
	INT,
	FLOAT,
	STRING,
	STRING_ESCAPE,
	CHAR,
	CHAR_ESCAPE,
	MACRO,
	MACRO_ESCAPE,
	COMMENT,
	MULTILINE_COMMENT,
	MULTILINE_COMMENT_END,
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
		*out_array = calloc(*out_len, sizeof(*out_array));
		if(*out_array == NULL) return ERR_MEM;
	}
	State state = NEWLINE;
	char token[MAX_TOKEN_SIZE + 1];
	bzero(token, sizeof(token) / sizeof(*token));
	int token_index = 0;
	int array_index = 0;
	while(1) {
		if(token_index > MAX_TOKEN_SIZE) return ERR_TOK;
		int ch = getchar();
#define RET_ERR_PAR(state) ({ \
		fprintf(stderr, "Parsing Error: state " #state " ch=%d(%c) token = \"%s\"\n", ch, ch, token); \
		return ERR_PAR; \
		})
		switch(state){
		case NEWLINE:
			switch(ch){
			case '/': // TODO: might be unneccisary
				token[token_index++] = ch;
				state = COMMENT;
				break;
			case '#':
				token[token_index++] = ch;
				state = MACRO;
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
			case '/':
				token[token_index++] = ch;
				state = COMMENT;
				break;
			case '\r':
			case '\n':
				state = NEWLINE;
			case '~':
			case '}':
			case '{':
			case ']':
			case '[':
			case ',':
			case '(':
			case ')':
			case '.':
			case '?':
			case ':':
			case ';':
			case '*':
				token[token_index] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				token[token_index] = '\0';
				break;
			case 'a'...'z':
			case 'A'...'Z':
			case '_':
				token[token_index++] = ch;
				state = IDENT;
				break;
			case '"':
				token[token_index++] = ch;
				state = STRING;
				break;
			case '\'':
				token[token_index++] = ch;
				state = CHAR;
				break;
			case '&':
				token[token_index++] = ch;
				state = AND_OP;
				break;
			case '|':
				token[token_index++] = ch;
				state = OR_OP;
				break;
			case '!':
			case '=':
				token[token_index++] = ch;
				state = EQ_OP;
				break;
			case '>':
				token[token_index++] = ch;
				state = GT_OP;
				break;
			case '<':
				token[token_index++] = ch;
				state = LT_OP;
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
			case ' ':
			case '\t':
				break;
			case '$':
			default:
				RET_ERR_PAR(START);
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
			case EOF: // TODO: might be unnecessiary
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
		case AND_OP:
			if(ch == '&' || ch == '=') {
				token[token_index++] = ch;
			} else {
				CHECK_UNGETC(ch);
			}
			(*out_array)[array_index] = strdup(token);
			if((*out_array)[array_index++] == NULL) return ERR_MEM;
			bzero(token, sizeof(token) / sizeof(*token));
			token_index = 0;
			state = START;
			break;
		case OR_OP:
			if(ch == '|' || ch == '=') {
				token[token_index++] = ch;
			} else {
				CHECK_UNGETC(ch);
			}
			(*out_array)[array_index] = strdup(token);
			if((*out_array)[array_index++] == NULL) return ERR_MEM;
			bzero(token, sizeof(token) / sizeof(*token));
			token_index = 0;
			state = START;
			break;
		case LT_OP:
			switch(ch) {
			case '<':
				token[token_index++] = ch;
				state = LBS_OP;
				break;
			case '=':
				token[token_index++] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
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
		case GT_OP:
			switch(ch) {
			case '>':
				token[token_index++] = ch;
				state = RBS_OP;
				break;
			case '=':
				token[token_index++] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
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
		case EQ_OP:
		case RBS_OP:
		case LBS_OP:
			if(ch == '=') {
				token[token_index++] = ch;
			} else {
				CHECK_UNGETC(ch);
			}
			(*out_array)[array_index] = strdup(token);
			if((*out_array)[array_index++] == NULL) return ERR_MEM;
			bzero(token, sizeof(token) / sizeof(*token));
			token_index = 0;
			state = START;
			break;
		case NUMBER_OP:
			if(ch >= '0' && ch <= '9') {
				token[token_index++] = ch;
				state = INT;
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
				RET_ERR_PAR(INT);
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
			switch(ch){
			case '0'...'9':
				token[token_index++] = ch;
				break;
			case '.':
			case 'a'...'z':
			case 'A'...'Z':
			case '_':
				RET_ERR_PAR(FLOAT);
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
		case STRING:
			switch(ch){
			case '\r':
			case '\n':
			case EOF:
				RET_ERR_PAR(STRING);
			case '"':
				token[token_index++] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
				break;
			case '\\':
				state = STRING_ESCAPE;
			default:
				token[token_index++] = ch;
				break;
			}
			break;
		case STRING_ESCAPE:
			switch(ch){
			case EOF:
				RET_ERR_PAR(STRING_ESCAPE);
			default:
				token[token_index++] = ch;
				state = STRING;
				break;
			}
			break;
		case CHAR:
			switch(ch){
			case '\r':
			case '\n':
			case EOF:
				RET_ERR_PAR(CHAR);
			case '\'':
				token[token_index++] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				state = START;
				break;
			case '\\':
				state = CHAR_ESCAPE;
			default:
				token[token_index++] = ch;
				break;
			}
			break;
		case CHAR_ESCAPE:
			switch(ch){
			case EOF:
				RET_ERR_PAR(CHARE_ESCAPE);
			default:
				token[token_index++] = ch;
				state = CHAR;
				break;
			}
			break;
		case MACRO:
			switch(ch){
			case EOF:
				break;
			case '\n':
			case '\r':
				state = NEWLINE;
				token[token_index++] = ch;
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				bzero(token, sizeof(token) / sizeof(*token));
				token_index = 0;
				break;
			case '\\':
				state = MACRO_ESCAPE;
			default:
				token[token_index++] = ch;
				break;
			}
			break;
		case MACRO_ESCAPE:
			switch(ch){
			case EOF:
				RET_ERR_PAR(MACRO_ESCAPE);
			default:
				token[token_index++] = ch;
				state = MACRO;
				break;
			}
			break;
		case COMMENT:
			switch(ch){
			case EOF:
				RET_ERR_PAR(COMMENT);
			case '/':
				state = IGNORE_LINE;
				break;
			case '*':
				state = MULTILINE_COMMENT;
				break;
			default:
				CHECK_UNGETC(ch);
				(*out_array)[array_index] = strdup(token);
				if((*out_array)[array_index++] == NULL) return ERR_MEM;
				state = START;
				break;
			}
			bzero(token, sizeof(token) / sizeof(*token));
			token_index = 0;
			break;
		case MULTILINE_COMMENT:
			switch(ch){
			case EOF:
				RET_ERR_PAR(MULTILINE_COMMENT);
			case '*':
				state = MULTILINE_COMMENT_END;
				break;
			}
			break;
		case MULTILINE_COMMENT_END:
			switch(ch){
			case EOF:
				RET_ERR_PAR(MULTILINE_COMMENT_END);
			case '/':
				state = START;
				break;
			default:
				state = MULTILINE_COMMENT;
				break;
			}
			break;
		case IGNORE_LINE:
			switch(ch){
			case EOF:
				break;
			case '\r':
			case '\n':
				state = NEWLINE;
				break;
			}
			break;
		}
		if(ch == EOF) break;
		if(array_index == *out_len) {
			*out_len *= 2;
			*out_array = realloc(*out_array, sizeof(*out_array) * *out_len);
			if(*out_array == NULL) return ERR_MEM;
		}
	}
	*out_len = array_index;
	*out_array = realloc(*out_array, sizeof(**out_array) * *out_len);
	if(*out_array == NULL) return ERR_MEM;
	return SUCCESS;
}
