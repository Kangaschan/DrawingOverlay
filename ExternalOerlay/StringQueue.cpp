#pragma once
#include "StringQueue.h"

#define MAX_QUEUE_SIZE 100  // Максимальный размер очереди

// Функция для инициализации очереди
void initQueue(StringQueue* queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

// Проверка, пуста ли очередь
int isEmpty(StringQueue* queue) {
    return queue->size == 0;
}

// Проверка, полна ли очередь
int isFull(StringQueue* queue) {
    return queue->size == MAX_QUEUE_SIZE;
}

// Добавление элемента в очередь (enqueue)
void enqueue(StringQueue* queue, const char* str) {
    if (isFull(queue)) {
        printf("Ошибка: Очередь переполнена!\n");
        return;
    }
    queue->data[queue->rear] = _strdup(str);  // Копируем строку в очередь
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->size++;
}

// Удаление элемента из очереди (dequeue)
char* dequeue(StringQueue* queue) {
    if (isEmpty(queue)) {
        printf("Ошибка: Очередь пуста!\n");
        return NULL;
    }
    char* removed = queue->data[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return removed;
}

// Просмотр первого элемента (peek)
char* peek(StringQueue* queue) {
    if (isEmpty(queue)) {
        printf("Ошибка: Очередь пуста!\n");
        return NULL;
    }
    return queue->data[queue->front];
}

// Функция для перевода очереди в массив строк
char** queueToArray(StringQueue* queue) {
    if (isEmpty(queue)) {
        return NULL;
    }

    char** array = (char**)malloc(queue->size * sizeof(char*));
    if (!array) {
        printf("Ошибка выделения памяти!\n");
        return NULL;
    }

    int current = queue->front;
    for (int i = 0; i < queue->size; i++) {
        array[i] = _strdup(queue->data[current]);  // Копируем строку в массив
        current = (current + 1) % MAX_QUEUE_SIZE;
    }

    return array;
}

// Функция для освобождения памяти массива строк
void freeArray(char** array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

// Функция для освобождения памяти очереди
void freeQueue(StringQueue* queue) {
    while (!isEmpty(queue)) {
        free(dequeue(queue));
    }
}

