#ifndef _STACK_H
#define _STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    size_t n;
    size_t member_size;
    size_t size;
    size_t head;
    bool in_heap;
    void *buffer;
}Stack;

Stack StackInit(size_t nmemb, size_t size);
void StackPush(Stack *stack, void* member);
void* StackPop(Stack *stack);
void StackClean(Stack *stack);
void StackFree(Stack *stack);

// Implementation
Stack StackInit(size_t nmemb, size_t size) {
    Stack new_stack = {
        .n = nmemb,
        .member_size = size,
        .size = nmemb * size,
        .head = 0,
        .in_heap = true
    };

    new_stack.buffer = malloc(new_stack.size);

    return new_stack;
}

void StackPush(Stack *stack, void* member) {
    if(stack->head < stack->size) {
        void *pos = stack->buffer + stack->member_size * stack->head;
        printf("pos ptr: %p\n", pos);
        memcpy(pos, member, stack->member_size);
        stack->head++;
    } else if (stack->in_heap) {
        stack->buffer = realloc(stack->buffer, 2 * stack->size);
        stack->size = 2 * stack->size;
        stack->n = 2 * stack-> n;
        void *pos = stack->buffer + stack->member_size * stack->head;
        memcpy(pos, member, stack->member_size);
        stack->head++;            
    }
}

void* StackPop(Stack *stack) {
    if(stack->head > 0) {
        stack->head--;
        return stack->buffer + stack->member_size * stack->head;
    }
}

void StackClean(Stack *stack) {
    memset(stack->buffer, 0, stack->size);
    stack->head = 0;
    
}

void StackFree(Stack *stack) {
    if(stack->in_heap) {
        free(stack->buffer);
        *stack = (Stack){0};
    }
}

#ifdef STACK_TEST
int main(void) {
    Stack int_stack = StackInit(20, sizeof(int));
    FILE *fp = fopen("/dev/urandom", "rb");
    
    for(int i = 0; i < 20; i++) {
        uint32_t rand = 0;
        fread(&rand, sizeof(rand), 1, fp);
        rand = (rand % 2001) - 1000;
        printf("head %lu: %d\n", int_stack.head, rand);
        StackPush(&int_stack, &rand);      
    }
    fclose(fp);

    printf("================================\n");

    do {
        int *pop_int = StackPop(&int_stack);
        printf("poped %lu: %d\n", int_stack.head, pop_int[0]);
    }while(int_stack.head > 0);

    StackFree(&int_stack);
}
#endif /* STACK_TEST */

#endif /* _STACK_H */
