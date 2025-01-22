#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUEUE_SIZE 100  
typedef struct {
    char* data[MAX_QUEUE_SIZE];
    int front;  // Указывает на начало очереди
    int rear;   // Указывает на конец очереди
    int size;   // Текущий размер очереди
} StringQueue;
void initQueue(StringQueue* queue);
int isEmpty(StringQueue* queue);
int isFull(StringQueue* queue);
void enqueue(StringQueue* queue, const char* str);
char* dequeue(StringQueue* queue);
char* peek(StringQueue* queue);
char** queueToArray(StringQueue* queue);
void freeArray(char** array, int size);
void freeQueue(StringQueue* queue);