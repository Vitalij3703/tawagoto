#include "proc/proc.h"
#include "proc.h"
#include "tglib/tgtype.h"
#include "mem/tgkmem.h"
#include "mem/tgkheap.h"
#include "tglib/tgstr.h"
#include "tgkgdt.h"
#include "mem/tgkvmem.h"
#include "tty/tty.h"
#include "tglib/tgtconv.h"
#include "mem/tgkvmem.h"

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
        thead->prev = ttail;
    } else {
        t->prev = ttail;
        ttail->next = t;
        ttail = t;
        ttail->next = thead;
        thead->prev = ttail;
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

extern struct thread* focus;

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
    if(t->keyowning) {
        t->keyowning = 0;
        focus = find_last_keyowning();
    }
    set_thread_state(t, PROC_HALFDEAD);
    irqrestore(v);
    if(tcurr == t) thread_yield(t);
}

void thread_yield(struct thread* t) {
    _uint32 v = irqsave();
    if (t == ttail || t == thead || t->next == t) { // already last or boot
        irqrestore(v);
        return;
    }
    t->prev->next = t->next;
    t->next->prev = t->prev;
    t->prev = ttail;
    t->next = thead;
    ttail->next = t;
    thead->prev = t;
    ttail = t;
    irqrestore(v);
}

void thread_stop(int status) {
    thread_kill(tcurr->tid);
    for(;;)
        asm volatile ("hlt");
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
    char ti[13];
    its(t->tid, ti);
    ttyputcharsf(":- created thread, tid %\n", ti);
    return t;
}

int isalive(struct thread* t) {
    return (get_thread_state(t) == PROC_RUNNING || get_thread_state(t) == PROC_READY);
}

struct thread* find_last_keyowning() {
    struct thread* t = ttail;
    while(!isalive(t) || t->keyowning!=1) {
        t = t->prev;
        if(t == ttail) {
            return NULL;
        }
    }
    return t;
}

void make_thread_keyowning(struct thread* t) {
    t->keyowning = 1;
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

int reap_threads() {
    struct thread* t = thead;
    for(;;) {
        t = t->next; // thead is always the boot thread
        if(get_thread_state(t) == PROC_HALFDEAD) {
            set_thread_state(t, PROC_DEAD);
            if (t == ttail) ttail = t->prev;
            t->next->prev = t->prev;
            t->prev->next = t->next;
            thread_free(t);
        }
    }
    return 0;
}

struct thread* find_next_ready() {
    _uint32 v = irqsave();
    struct thread* t = tcurr->next;
    while(get_thread_state(t) != PROC_READY) {
        t = t->next;
        if(t == tcurr) {
            struct thread* chosen = get_thread_state(tcurr) == PROC_RUNNING ? tcurr : thread_get(0);
            irqrestore(v);
            return chosen;
        }
    }
    irqrestore(v);
    return t;
}

void thread_switch(struct thread* next) {
    _uint32 v = irqsave();
    if(next==tcurr) {
        irqrestore(v);
        return;
    }
    if(get_thread_state(next) != PROC_READY) {
        irqrestore(v);
        return;
    }
    if(get_thread_state(tcurr) == PROC_RUNNING)
        set_thread_state(tcurr, PROC_READY);
    set_thread_state(next, PROC_RUNNING);
    struct thread* old = tcurr;
    tcurr = next;
    __sks(next->kstackt);
    sctx(&old->esp, next->esp);
    irqrestore(v);
}


/* process stuff */

struct process* phead;
struct process* ptail;
struct process* pcurr;

#define USERSPACE_START_PDE 256
#define USERSPACE_END_PDE 960

// create a new page directory with the kernel part shared
_uint32* addrspacenew() {
    _uint32* newdir = (_uint32*)tgkallocpage();
    if(newdir == PAGEFAIL) return (_uint32*)PAGEFAIL;
    memset((_uint8*)newdir, 0, PAGESIZE);
    // copy the first kernel part
    for(_uint32 i = 0; i < USERSPACE_START_PDE; i++) {
        _uint32 entry = page_direc[i];
        newdir[i] = entry;
    }
    // copy the second kernel part (where the framebuffer is located)
    for(_uint32 i = USERSPACE_END_PDE; i < 1024; i++) {
        _uint32 entry = page_direc[i];
        newdir[i] = entry;
    }
    return newdir;
}

// extern _uint32* page_direc;

// free a page directory
void addrspacefree(_uint32* dir) {
    if(dir == PAGEFAIL) return;
    if(dir == page_direc) return;
    // the reaper will load the kernel directory
    for(_uint32 i = USERSPACE_START_PDE; i < USERSPACE_END_PDE; i++) {
        _uint32 entry = dir[i] & ~0xFFF;
        if(entry == 0) continue;
        if((entry & (1 << /*flag*/PRESENT)) == 0) continue;
        for(_uint32 ii = 0; ii < 1024; ii++) {
            _uint32 table = ((_uint32*)entry)[ii] & ~0xFFF;
            if(table == 0) continue;
            if((table & (1 << /*flag*/PRESENT)) == 0) continue;
            tgkfreepage(table);
        }
        tgkfreepage(entry);
    }
    tgkfreepage((_uint32)dir);
}

// push a process onto the linked list
void push_process(struct process* p) {
    _uint32 v = irqsave();
    if(phead == NULL) {
        phead = p;
        ptail = p;
        ptail->next = phead;
        phead->prev = ptail;
    } else {
        p->prev = ptail;
        ptail->next = p;
        ptail = p;
        ptail->next = phead;
        phead->prev = ptail;
    }
    irqrestore(v);
}

// find a process with pid [1]
struct process* process_get(_uint /*[1]*/pid) {
    struct process* p = phead;
    while(p->pid!=pid) {
        p = p->next;
        if(p == phead)
            return NULL;
    }
    return p;
}

// obviously create a process
struct process* proc_create(char* name,
                            char* from,
                            int isroot,
                            struct process* parent,
                            _uint32 func) {

}

// set the state of process [1] to [2]
void set_process_state(struct process* /*[1]*/proc, PROC_STATE /*[2]*/state) {
    proc->root_thread->state = state;
}

// kill the process with pid [1]
void proc_kill(int /*[1]*/pid) {
    _uint32 v = irqsave();
    struct process* p = phead;
    while(p->pid!=pid) {
        p = p->next;
        if(p == phead) {
            irqrestore(v);
            return;
        }
    }
    thread_kill(p->root_thread->tid);
    irqrestore(v);
}
