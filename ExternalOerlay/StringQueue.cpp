#pragma once
#include "StringQueue.h"

#define MAX_QUEUE_SIZE 100  // ������������ ������ �������

// ������� ��� ������������� �������
void initQueue(StringQueue* queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

// ��������, ����� �� �������
int isEmpty(StringQueue* queue) {
    return queue->size == 0;
}

// ��������, ����� �� �������
int isFull(StringQueue* queue) {
    return queue->size == MAX_QUEUE_SIZE;
}

// ���������� �������� � ������� (enqueue)
void enqueue(StringQueue* queue, const char* str) {
    if (isFull(queue)) {
        printf("������: ������� �����������!\n");
        return;
    }
    queue->data[queue->rear] = _strdup(str);  // �������� ������ � �������
    queue->rear = (queue->rear + 1) % MAX_QUEUE_SIZE;
    queue->size++;
}

// �������� �������� �� ������� (dequeue)
char* dequeue(StringQueue* queue) {
    if (isEmpty(queue)) {
        printf("������: ������� �����!\n");
        return NULL;
    }
    char* removed = queue->data[queue->front];
    queue->front = (queue->front + 1) % MAX_QUEUE_SIZE;
    queue->size--;
    return removed;
}

// �������� ������� �������� (peek)
char* peek(StringQueue* queue) {
    if (isEmpty(queue)) {
        printf("������: ������� �����!\n");
        return NULL;
    }
    return queue->data[queue->front];
}

// ������� ��� �������� ������� � ������ �����
char** queueToArray(StringQueue* queue) {
    if (isEmpty(queue)) {
        return NULL;
    }

    char** array = (char**)malloc(queue->size * sizeof(char*));
    if (!array) {
        printf("������ ��������� ������!\n");
        return NULL;
    }

    int current = queue->front;
    for (int i = 0; i < queue->size; i++) {
        array[i] = _strdup(queue->data[current]);  // �������� ������ � ������
        current = (current + 1) % MAX_QUEUE_SIZE;
    }

    return array;
}

// ������� ��� ������������ ������ ������� �����
void freeArray(char** array, int size) {
    for (int i = 0; i < size; i++) {
        free(array[i]);
    }
    free(array);
}

// ������� ��� ������������ ������ �������
void freeQueue(StringQueue* queue) {
    while (!isEmpty(queue)) {
        free(dequeue(queue));
    }
}

