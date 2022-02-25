/*
 * heap.h
 *
 * Created: 25.02.2022 15:02:33
 *  Author: qfj
 */ 


#ifndef HEAP_H_
#define HEAP_H_


#define MAX_HEAP_SIZE 10+1

typedef struct{
	uint8_t Key[MAX_HEAP_SIZE];
	void* Value[MAX_HEAP_SIZE];
	
	uint8_t heap_p;
}HeapType;

uint8_t push( HeapType* Heap,uint8_t Key,void* Value);
void* pop(HeapType *Heap);

void reheap_up(HeapType* Heap,uint8_t pointer);
void reheap_down(HeapType * Heap,uint8_t pointer);
void resetHeap(HeapType* Heap);

void swap(HeapType* Heap,uint8_t p, uint8_t new_p);


#endif /* HEAP_H_ */