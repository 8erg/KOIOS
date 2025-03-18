
#ifndef KEYBOARD_INI_H
#define KEYBOARD_INI_H

#include <windows.h>


typedef struct {
    int vKey;
} KeyPress;

typedef struct {
    KeyPress* elements;
    int front;
    int rear;
    int size;
    int capacity;
    CRITICAL_SECTION queueCriticalSection;
    HKL hkl;
} ExpandableQueue;

void initQueue(ExpandableQueue* queue, int initialCapacity, HKL hkl);
void Enter_Queue(ExpandableQueue* queue, KeyPress item);
void keyboard_layout(BYTE vKey, HKL hkl);
void *keyboard_state_queue(void* queue_arg);

#endif 