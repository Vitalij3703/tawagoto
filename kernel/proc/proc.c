#include "proc/proc.h"
#include "tglib/tgtype.h"
#include "mem/tgkmem.h"
#include "mem/tgkheap.h"
#include "tglib/tgstr.h"
#include "tgkgdt.h"

struct thread* thead;
struct thread* ttail;
struct thread* tcurr;

int last_thread_id = 0;

static inline _uint32 irqsave(void) {
    _uint32 v;
    asm volatile (
        "pushfl\n\t"
        "popl %0\n\t"
        "cli\n\t"
        :"=r"(v) :
        : "memory"
    );
    return v;
}

#define FLAG_IF 0x200

static inline void irqrestore(_uint32 v) {
    if(v&FLAG_IF)
        asm volatile ("sti":::"memory");
}

PROC_STATE get_thread_state(struct thread* t) {
    return t->state;
}

void set_thread_state(struct thread* t, PROC_STATE state) {
    t->state = state;
}

void thread_push(struct thread* t) {
    _uint32 v = irqsave();
    if(thead == NULL) {
        thead = t;
        ttail = t;
        ttail->next = thead;
    } else {
        ttail->next = t;
        ttail = t;
        ttail->next = thead;
    }
    irqrestore(v);
}

struct thread* thread_get(_uint tid) {
    struct thread* t = thead;
    while(t->tid!=tid)
        t = t->next;
    return t;
}

void thread_free(struct thread* t) {
    _uint32 v = irqsave();
    if(t==NULL) return;
    if(get_thread_state(t) != PROC_HALFDEAD) {
        irqrestore(v);
        return;
    }
    t->next->prev = t->prev;
    t->prev->next = t->next;
    if(thead == t) thead = t->next;
    if(ttail == t) ttail = t->prev;
    tgkfreepage(t->kstack);
    set_thread_state(t, PROC_DEAD);
    kmfree(t);
    irqrestore(v);
}

void thread_kill(_uint tid) {
    _uint32 v = irqsave();
    struct thread* t = thead;
    while(t->tid!=tid) {
        t = t->next;
        if(t == thead) {
            irqrestore(v);
            return;
        }
    }
    if(t==NULL) {
        irqrestore(v);
        return;
    }
    set_thread_state(t, PROC_HALFDEAD);
    if(tcurr == t) thread_yield(t);
    irqrestore(v);
}

// a reaper process will destroy all the zombies

void thread_yield(struct thread* t) {
    _uint32 v = irqsave();
    if(get_thread_state(t->next) == PROC_HALFDEAD) thread_yield(t->next);
    ttail->next = t;
    t->next->prev = t->prev;
    t->prev->next = t->next;
    t->next = thead;
    ttail = t;
    irqrestore(v);
}

void thread_stop(int status) {
    thread_kill(tcurr->tid);
}

void thread_jumpto(void) {
    asm volatile ("sti":::"memory");
    thread_stop(
        (int)(((void* (*)(void*))tcurr->ieip)(tcurr->arg)) // "just use c++" no
    );
}

void thread_initstack(struct thread* t) {
    memset((_uint8*)t->kstack, 0, PAGESIZE);
    *(_uint32*)(t->kstackt - 4) = 0;
    *(_uint32*)(t->kstackt - 8) = (_uint32)thread_jumpto;
    *(_uint32*)(t->kstackt - 12) = 0; // ebp
    *(_uint32*)(t->kstackt - 16) = 0; // ebx
    *(_uint32*)(t->kstackt - 20) = 0; // esi
    *(_uint32*)(t->kstackt - 24) = 0; // edi
    t->esp = t->kstackt - 24;
}

struct thread* thread_create(_uint32 func, void* argument, struct process* owning) {
    struct thread* t = kmalloc(sizeof(struct thread));
    if(t==NULL)
        return t;
    memset((_uint8*)t, 0, sizeof(struct thread));
    set_thread_state(t, PROC_NEW);
    t->parent = owning;
    t->kstack = tgkallocpage();
    if(t->kstack == PAGEFAIL) {
        kmfree(t);
        return NULL;
    }
    t->kstackt = t->kstack+PAGESIZE;
    t->esp = 0;
    t->ieip = func;
    t->time = 10; //ms
    t->arg = argument;
    thread_initstack(t);
    set_thread_state(t, PROC_READY);
    _uint32 v = irqsave();
    t->tid = last_thread_id++;
    thread_push(t);
    irqrestore(v);
    return t;
}

//extern _uint8 stack_top[];
//extern _uint8 stack_bottom[];

struct thread* initial_thread_create(_uint32 func, void* argument) {
    struct thread* t = kmalloc(sizeof(struct thread));
    if(t==NULL)
        return t;
    memset((_uint8*)t, 0, sizeof(struct thread));
    set_thread_state(t, PROC_NEW);
    t->kstack = (_uint32)stack_bottom;
    t->kstackt = (_uint32)stack_top;
    t->esp = 0;
    t->ieip = func;
    t->time = 10; //ms
    t->arg = argument;
    //thread_initstack(t);
    tcurr = t;
    set_thread_state(t, PROC_RUNNING);
    _uint32 v = irqsave();
    t->tid = last_thread_id++;
    thread_push(t);
    irqrestore(v);
    return t;
}

void thread_switch(struct thread* next) {
    _uint32 v = irqsave();
    if(next==tcurr) {
        irqrestore(v);
        return;
    }
    set_thread_state(tcurr, PROC_READY);
    set_thread_state(next, PROC_RUNNING);
    struct thread* old = tcurr;
    tcurr = next;
    // later switch page dirs
    __sks(next->kstackt);
    sctx(&old->esp, next->esp);
    irqrestore(v);
}


/* process stuff */



struct process* proc_create(char* name, 
                            char* from, 
                            int isroot, 
                            struct process* parent,
                            _uint32 func) {

}