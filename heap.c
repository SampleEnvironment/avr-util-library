/*
* heap.c
*
* Created: 25.02.2022 15:02:49
*  Author: qfj
*/

#include <stddef.h>
#include <stdint.h>
#include "heap.h"



uint8_t push( HeapType* Heap,uint8_t Key,void* Value){
	Heap->heap_p ++;
	if (Heap->heap_p == MAX_HEAP_SIZE);
	{
		//heap to small
		return 0;
	}
	Heap->Key[Heap->heap_p]   = Key;
	Heap->Value[Heap->heap_p] = Value;
	reheap_up(Heap,Heap->heap_p);
	
	return 1;
}

void* pop(HeapType *Heap){
	void* res = Heap->Value[1];
	Heap->Key[1] = Heap->Key[Heap->heap_p];
	Heap->heap_p--;
	reheap_down(Heap,1);
	return res;
}

void reheap_down(HeapType * Heap,uint8_t pointer){
	uint8_t new_p;
	if (2 * pointer > Heap->heap_p){
		return;
	}
	if (2 * pointer + 1 <= Heap->heap_p
	&& Heap->Key[2*pointer+1] < Heap->Key[2*pointer]){
		new_p = 2 * pointer + 1;
		} else {
		new_p = 2 * pointer;
	}
	if (Heap->Key[pointer] > Heap->Key[new_p]){
		swap(Heap,pointer,new_p);
		reheap_down(Heap,new_p);
	}
}

void reheap_up(HeapType* Heap,uint8_t pointer){
	uint8_t p = pointer;
	uint8_t new_p = p / 2;
	
	while (p > 1 && Heap->Key[new_p] > Heap->Key[p]){
		/* swap the two elements */
		swap(Heap,p,new_p);
		p = p / 2;
		new_p = new_p / 2;
	}
}



void swap(HeapType* Heap,uint8_t p, uint8_t new_p){
	uint8_t tmp_key = Heap->Key[new_p];
	void * tmp_val = Heap->Value[new_p];
	
	Heap->Key[new_p]   = Heap->Key[p];
	Heap->Value[new_p] = Heap->Value[p];
	
	Heap->Key[p]   = tmp_key;
	Heap->Value[p] = tmp_val;
}

void resetHeap(HeapType* Heap){
	Heap->heap_p = 0;
	for (uint8_t i = 0;i<MAX_HEAP_SIZE;i++)
	{
		Heap->Key[i] = 0;
		Heap->Value[i] = NULL;
	}
}