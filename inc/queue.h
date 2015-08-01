#ifndef __QUEUE_H
#define __QUEUE_H

#include <type.h>

#define QUEUE_MAX_LEN  256 
struct s_queue{
    /* we hava no malloc() now, 
     * so we use static array. */ 
    uint8_t base[QUEUE_MAX_LEN];
    int front;
    int rear;  
};
typedef struct s_queue queue;
typedef queue* pqueue;

bool queue_init(pqueue Q);
bool queue_en(pqueue Q, uint8_t e);
bool queue_de(pqueue Q, uint8_t *e);
int queue_getlen(pqueue Q);
bool queue_gethead(pqueue Q, uint8_t *e);

#endif
