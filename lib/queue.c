/* queue.c
 * this file define a queue, used as a buffer.
 */
#include <type.h>
#include <queue.h> 

bool queue_init(pqueue Q){
    Q->rear = Q->front = 0;
    return OK;
}

int queue_getlen(pqueue Q){
    return (Q->rear - Q->front + QUEUE_MAX_LEN)%QUEUE_MAX_LEN;
}

bool queue_en(pqueue Q, uint8_t e){
    if ((Q->rear + 1) % QUEUE_MAX_LEN == Q->front) return ERROR;
    Q->base[Q->rear] = e;
    Q->rear = (Q->rear + 1) % QUEUE_MAX_LEN;
    return OK;
}

bool queue_de(pqueue Q, uint8_t *e){
    /* if queue is full, exit */
    if (Q->rear == Q->front) return ERROR;
    *e = Q->base[Q->front];
    Q->front = (Q->front + 1) % QUEUE_MAX_LEN;
    return OK;
}

bool queue_gethead(pqueue Q, uint8_t *e){
    if (Q->rear == Q->front) return ERROR;
    *e = Q->base[Q->front];
    return OK;
}

