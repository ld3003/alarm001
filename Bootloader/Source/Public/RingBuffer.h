#ifndef __ringbuffer_h__
#define __ringbuffer_h__

#define QueueSize 128   //???????????
typedef char Queue_DataType;

typedef struct{
		 int count;
		 int index;
		 int index2;
		 Queue_DataType data[QueueSize];
}CirQueue;

void InitQueue(CirQueue *Q);
void EnQueue(CirQueue *Q,Queue_DataType x);
Queue_DataType DeQueue(CirQueue *Q);
char QueueIsEmpty(CirQueue *Q);


#endif