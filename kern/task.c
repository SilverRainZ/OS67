#include <type.h>
#include <pm.h>
#include <pmm.h>
#include <vmm.h>
#include <heap.h>
#include <dbg.h>
#include <string.h>
#include <sched.h>
#include <task.h>

pid_t now_pid = 0;

int32_t kernel_thread(int (*fn)(void *), void *arg){
    return 0;
}

void kthread_exit(){

}
