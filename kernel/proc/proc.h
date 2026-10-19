#ifndef _TGKPROC
#define _TGKPROC
#include "tglib/tgtype.h"

typedef enum PROC_STATE {
    NEW, // the process isnt ready to execute YET
    READY, //the process is ready to execute but not currently executing
    RUNNING, // the process is executing
    BLOCKED, // cant continue until wtv happens
    ZOMBIE,
    KILLED
} PROC_STATE;

#define PROC_NEW NEW
#define PROC_READY READY
#define PROC_RUNNING RUNNING
#define PROC_BLOCKED BLOCKED
#define PROC_DEAD KILLED
#define PROC_HALFDEAD ZOMBIE

// can also be called a task
struct thread {
    _uint tid;
    struct process* parent;
    PROC_STATE state;
    _uint32 esp; // saved stack
    _uint32 kstack; // kernel stack
    _uint32 kstackt; // kernel stack top
    _uint32 ieip; // initial eip (function)
    void* arg; // the argument for the function
    _uint8 time;
    int keyowning; // is focused by the keyboard driver
    struct thread* next;
    struct thread* prev;
};

PROC_STATE get_thread_state(struct thread* t);


typedef enum PROC_OWNER {
    PROOT,
    PUSER
} PROC_OWNER;

#define ROOT_PROC PROOT
#define USER_PROC PUSER

struct process {
    _uint pid;
    char* name;
    char* from;
    _uint32* page_direc;
    PROC_STATE state;
    struct thread* root_thread;
    PROC_OWNER owner;
    struct process* parent; // if it is the parent, set to NULL
    struct process* next;
    struct process* prev;
};

extern struct thread* thead;
extern struct thread* ttail;
extern struct thread* tcurr;

#define DEFAULT_THREAD_TIME 10

struct thread* thread_create(_uint32 func, void* argument, struct process* owning);
struct thread* thread_get(_uint tid);
void thread_kill(_uint tid);
void thread_free(struct thread* t);
void thread_stop(int status);
void thread_yield(struct thread* t);
extern void sctx(_uint32* old, _uint32 new);
void thread_switch(struct thread* next);
struct thread* initial_thread_create(_uint32 func, void* argument);
struct thread* find_last_keyowning();
void make_thread_keyowning(struct thread* t);

/* processes */

extern struct process* phead;
extern struct process* ptail;
extern struct process* pcurr;

struct process* proc_create(char* name, 
                            char* from, 
                            int isroot, 
                            struct process* parent,
                            _uint32 func);

void proc_destroy(struct process* proc);

void proc_exit(int err);

void proc_kill(int pid); // thread_kill(p->root)

struct process* proc_get(int pid);

struct process* proc_cur();

#endif