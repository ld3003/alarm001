#include "RingBuffer.h"


void InitQueue(CirQueue *Q)
{
		Q->index = 0;
		Q->index2 = 0;
    Q->count = 0;
}
void EnQueue(CirQueue *Q,Queue_DataType x)
{
	if (Q->count >= QueueSize)
		return;
	Q->count ++;
	Q->data[Q->index] = x;
	Q->index=(Q->index+1)%QueueSize;      //??????????1
}
Queue_DataType DeQueue(CirQueue *Q)
{
	Queue_DataType tmp;
	if (Q->count <= 0)
		return tmp;
	
	
	tmp = Q->data[Q->index2];
	Q->index2=(Q->index2+1)%QueueSize;
	Q->count --;
	return tmp;
	//
}

char QueueIsEmpty(CirQueue *Q)
{
	if (Q->count <= 0)
		return 1;
	return 0;
	//
}