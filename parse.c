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
    Stack tokens;
    // Stack output;
    // Stack opstack;
} Lexer;

typedef struct {
    int type;
    int size;
    char *lexeme;
} Token;

typedef enum {
    TOKEN_UNKNOWN,
    TOKEN_NUMBER,
    TOKEN_VARIABLE,
    TOKEN_BINARY_OPERATOR,
    TOKEN_FUNCTION
} TokenTypes;

typedef enum {
    LEXER_TEST,
    LEXER_GMP
} LexerBackends;

typedef enum {
    TOKENIZER_STATE_OK,
    TOKENIZER_STATE_INVALID,
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
        .tokens = StackInit(100, sizeof(Token)),
        // .output = StackInit(100, sizeof(Token)),
        // .opstack = StackInit(100, sizeof(Token))
    };
    return lexer;
}

void LexerFree(Lexer *lex) {
    StackFree(&lex->tokens);
    // StackFree(&lex->output);
    // StackFree(&lex->opstack);
}

int Tokenize(Lexer *lex) {
    Token t = {0};

    char *head = lex->raw_str;
    char *start = head;
    
    while(*head) {
        switch (*head) {
        case '\0':
            break;
        case ' ':
        case '\n':
        case '\t':
        case '\r':
        case '\v':
            head++;
            break;

        // Operators
        case '+':
        case '-':
        case '/':
        case '*':
            t.lexeme = head;
            t.size = 1;
            t.type = TOKEN_BINARY_OPERATOR;
            StackPush(&lex->tokens, &t);
            head++;
            break;

        // Numbers
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            t.lexeme = head;
            start = head;
            t.type = TOKEN_NUMBER;
            
            while (head[0] >= '0' && head[0] <= '9') ++head;
            if (head[0] == '.') ++head;
            while (head[0] >= '0' && head[0] <= '9') ++head;

            t.size = head-start;
            StackPush(&lex->tokens, &t);
            break;
        default:
            // Names
            if(
                (head[0] >= 'A' && head[0] <= 'Z') ||
                (head[0] >= 'a' && head[0] <= 'z') ||
                head[0] == '_')
            {
                t.lexeme = head;
                start = head;
                do {
                    ++head;
                } while (
                    (head[0] >= 'A' && head[0] <= 'Z') ||
                    (head[0] >= 'a' && head[0] <= 'z') ||
                    (head[0] >= '0' && head[0] <= '9') ||
                    head[0] == '_');
                t.type = TOKEN_VARIABLE;
                t.size = head - start;
                StackPush(&lex->tokens, &t);
                break;
            }

            // Invalid token
            lex->head = head;
            return TOKENIZER_STATE_INVALID;
        }
    }

    return TOKENIZER_STATE_OK;
}

void TokenPrint(Token t) {
    for(int i = 0; i <= t.size; i++){
        printf("%c", t.lexeme[i]);
    }
    printf("\n");
}

void TokenStackPrint(Lexer *lex) {
    printf("Input buffer size: %lu\n", lex->tokens.head);

    Token *t = (Token*) lex->tokens.buffer;
    for(int i = 0; i < lex->tokens.head; i++){
        TokenPrint(t[i]);
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
