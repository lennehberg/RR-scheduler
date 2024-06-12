//
// Created by ubuntu on 6/11/24.
//

#ifndef _THREAD_UTILS_H_
#define _THREAD_UTILS_H_

#define MAX_THREAD_NUM 100 /* maximal number of threads */
#define STACK_SIZE 4096 /* stack size per thread (in bytes) */

#define INFINITE_TIME (-1)

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <queue>
#include <memory>
#include <unistd.h>
#include <signal.h>


typedef unsigned int address_t;

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
        "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */


#define JB_SP 4
#define JB_PC 5
address_t translate_address(address_t addr)
{
  address_t ret;
  asm volatile("xor    %%gs:0x18,%0\n"
			   "rol    $0x9,%0\n"
	  : "=g" (ret)
	  : "0" (addr));
  return ret;
}
#endif


typedef void (*thread_entry_point)(void);

typedef int tid_t;
typedef enum state_t
{
	READY,
	RUNNING,
	BLOCKED
}state_t;

typedef struct thread_t{
	tid_t tid;
	state_t state;
	itimerval time_slice;
	thread_entry_point entry_point;
	sigjmp_buf env;
	address_t sp;
	address_t pc;
}thread_t;


#endif //_THREAD_UTILS_H_
