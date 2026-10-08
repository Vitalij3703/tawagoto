#ifndef _TGKPROC
#define _TGKPROC
#include "tglib/tgtype.h"

// first typedef in the kernel... i love to suffer
static typedef enum PROC_STATE {
    NEW, // the process isnt ready to execute YET
    READY, //the process is ready to execute but not currently executing
    RUNNING, // the process is executing
    BLOCKED, // cant continue until wtv happens
    KILLED // OH GOD I JUST KILLED A PROCESS
} PROC_STATE;

#define PROC_NEW PROC_STATE.NEW
#define PROC_READY PROC_STATE.READY
#define PROC_RUNNING PROC_STATE.RUNNING
#define PROC_BLOCKED PROC_STATE.BLOCKED
#define PROC_DEAD PROC_STATE.KILLED

struct thread {
    _uint tid;
    struct process* parent;
    PROC_STATE (*get_state)(struct thread);
    _uint32 eip;
    _uint32 esp;
    _uint32 ebp;
    _uint32 eax;
    _uint32 ebx;
    _uint32 ecx;
    _uint32 edx;
    _uint32 esi;
    _uint32 edi;
    _uint32 kstack;
    _uint32 ustack;
    _uint time;
};

static typedef enum PROC_OWNER {
    ROOT,
    USER
} PROC_OWNER;

#define ROOT_PROC PROC_OWNER.ROOT
#define USER_PROC PROC_OWNER.USER

struct process {
    _uint pid;
    char* name;
    char* from;
    _uint32* page_direc;
    PROC_STATE state;
    struct thread* root_thread;
    PROC_OWNER owner;
    struct process* parent; // if it is the parent, set to NULL

};

static struct thread* threads;

struct process* proc_create(char* name, 
                            char* from, 
                            int isroot, 
                            struct process* parent);

void proc_destroy(struct process* proc); // pls kys, process

void proc_exit(int err); // i will kms, i am process

void proc_kill(int pid); // I WILL KILL YOU, PROCESS!!!!!

struct process* proc_get(int pid);

struct process* proc_cur();

#endif