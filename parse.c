#include <stdbool.h>
#include "stack.h"

#define ARRLEN(X) (sizeof(X)/sizeof((X)[0]))

typedef struct {
    char *op;
    int level;
} OperatorPrecedence;

typedef struct {
    char *raw_str;
    char *head;
    int backend;
    Stack input;
    Stack output;
    Stack opstack;
} Lexer;

typedef struct {
    int type;
    char *head;
    char *tail;
    bool assoc;
    int precedence;
} Token;

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_OPERATOR,
    TOKEN_FUNCTION
} TokenTypes;

typedef enum {
    LEXER_TEST,
    LEXER_GMP
} LexerBackends;

typedef enum {
    TOKEN_OK,
    TOKEN_EXTRA_DOT
} TokenErrors;

static const OperatorPrecedence g_OpPrec[] = {
    {"*", 3},
    {"/", 3},
    {"-", 2},
    {"+", 2}
};

Lexer LexerInit(char *expr, int backend) {
    Lexer lexer = {
        .raw_str = expr,
        .head = expr,
        .backend = backend,
        .input = StackInit(100, sizeof(Token)),
        .output = StackInit(100, sizeof(Token)),
        .opstack = StackInit(100, sizeof(Token))
    };
    return lexer;
}

void LexerFree(Lexer *lex) {
    StackFree(&lex->input);
    StackFree(&lex->output);
    StackFree(&lex->opstack);
}

int Tokenize(Lexer *lex) {

    char *end = lex->raw_str + strlen(lex->raw_str);
    
    while(lex->head < end) {
		while(
			lex->head[0] == ' '	    ||
			lex->head[0] == '\n'	||
			lex->head[0] == '\t'	||
			lex->head[0] == '\r'    ||
			lex->head[0] == '\v'
		) lex->head++;

		if(lex->head[0] == '\0') break;

        Token t = {0};
        
        // numbers starting with .
		if(lex->head[0] == '.') {
            t.head = lex->head;
            do {
                ++lex->head;
                if(lex->head[0] == '.') return TOKEN_EXTRA_DOT;
            } while(lex->head[0] >= '0' && lex->head[0] <= '9');

		    t.type = TOKEN_NUMBER;
		    t.tail = lex->head;
		    StackPush(&lex->input, &t);
		    continue;
		}

		// numbers starting with numbers
		if(lex->head[0] >= '0' && lex->head[0] <= '9') {
            t.head = lex->head;
            bool dot_seen = false;
            do {
                ++lex->head;
                if(dot_seen == true && lex->head[0] == '.') {
                    return TOKEN_EXTRA_DOT;
                }
                if(lex->head[0] == '.') {
                    dot_seen = true;
                }
            } while((lex->head[0] >= '0') && (lex->head[0] <= '9') || (lex->head[0] == '.'));
            t.type = TOKEN_NUMBER;
            t.tail = lex->head;
            StackPush(&lex->input, &t);
            continue;
		}

		// variables
		if(
		    (lex->head[0] >= 'A' && lex->head[0] <= 'Z') ||
		    (lex->head[0] >= 'a' && lex->head[0] <= 'z') ||
		    lex->head[0] == '_') {
		    t.head = lex->head;
		    do {
		        ++lex->head;
		    } while (
				(lex->head[0] >= 'A' && lex->head[0] <= 'Z') ||
				(lex->head[0] >= 'a' && lex->head[0] <= 'z') ||
				(lex->head[0] >= '0' && lex->head[0] <= '9') ||
				lex->head[0] == '_');

			t.type = TOKEN_VARIABLE;
			t.tail = lex->head;
			StackPush(&lex->input, &t);
			continue;
		}

		// operators
		switch(lex->head[0]) {
		    case '/':
		        t.precedence = 3;
		        break;
		    case '*':
		        t.precedence = 3;
		        break;
		    case '-':
		        t.precedence = 2;
		        break;
		    case '+':
		        t.precedence = 2;
		        break;
		}
		if(t.precedence) {
		    t.head = lex->head;
		    t.tail = lex->head;
            t.type = TOKEN_OPERATOR;

            StackPush(&lex->input, &t);
            lex->head++;
		}

    }
}

void TokenPrint(Token t) {
    for(int i = 0; &t.head[i] <= &t.tail[0]; i++){
        printf("%c", t.head[i]);
    }
    printf("\n");
}

void TokenStackPrint(Lexer *lex) {
    printf("Input buffer size: %lu\n", lex->input.head);

    Token *t = (Token*) lex->input.buffer;
    for(int i = 0; i < lex->input.head; i++){
        TokenPrint(t[i]);
        // for(int j = 0; &t[i].head[j] <= t[i].tail; j++){
        //     printf("%c", t[i].head[j]);
        // }
    }
}

int main(void) {

    char *str = "2.3 + 5 / 2 - 12 * 2.4";

    Lexer lex = LexerInit(str, LEXER_TEST);

    int err = Tokenize(&lex);

    if(err) printf("Error %i\n", err);

    TokenStackPrint(&lex);

    // shunting yard
    // Token *t = (Token*) StackPop(&lex.input);
    // TokenPrint(*t);

    LexerFree(&lex);

    return 0;
}
