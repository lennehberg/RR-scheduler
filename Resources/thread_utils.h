//
// Created by ubuntu on 6/11/24.
//

#ifndef UUTILS_H_
#define UUTILS_H_

#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <queue>
#include <memory>
#include <unistd.h>
#include <signal.h>
#include "arch_utils.h"

#define MAX_THREAD_NUM 100 /* maximal number of threads */
#define STACK_SIZE 4096 /* stack size per thread (in bytes) */

#define INFINITE_TIME (-1)

typedef void (*thread_entry_point)(void);

typedef int tid_t;
typedef enum state_t
{
	READY,
	RUNNING,
	BLOCKED
}state_t;

typedef struct thread_t{
	tid_t tid_;
	state_t state_;
	itimerval time_slice_;
	thread_entry_point entry_point_;
	sigjmp_buf env_;
	char* stack_;
	address_t sp_;
	address_t pc_;

	/**
	 * @brief Constructor for main thread
	 */
	thread_t()
	{
		tid_ = 0;
		state_ = READY;
		time_slice_ = {0};
		entry_point_ = nullptr;
		// sigsetjmp(env_, 1);
		stack_ = nullptr;
		sp_ = 0;
		pc_ = 0;
	}

	/**
	 * @brief Default constructor
	 * @param tid
	 * @param time_slice
	 * @param entry_point
	 */
	thread_t(tid_t tid,itimerval& time_slice, thread_entry_point entry_point= nullptr):
	tid_(tid), state_(BLOCKED), time_slice_(time_slice), entry_point_(entry_point)
	{
		stack_ = new char[STACK_SIZE];
		// set the stack pointer to the bottom of the stack and program counter to the entry point of the thread
		sp_ = (address_t) stack_ + STACK_SIZE - sizeof(address_t);
		pc_ = (address_t) entry_point;
		sigsetjmp(env_, 1);
		env_->__jmpbuf[JB_SP] = translate_address(sp_);
		env_->__jmpbuf[JB_PC] = translate_address(pc_);
		sigemptyset(&env_->__saved_mask);
	}

	~thread_t()
	{
		delete[] stack_;
	}


}thread_t;


#endif //UUTILS_H_
