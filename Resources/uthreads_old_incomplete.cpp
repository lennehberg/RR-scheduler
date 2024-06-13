//
// Created by ubuntu on 6/9/24.
//

#include "uthreads.h"
#include <iostream>
#include <thread>

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */


/// GLOBALS ///
int used_tids_[MAX_THREAD_NUM] = {0};
int quantum = 0;
Schedueler sched_;
itimerval v_timer;
sigset_t sigset_;

void setup_v_timer()
{
  v_timer.it_value.tv_sec = 0;
  v_timer.it_value.tv_usec = quantum;
  v_timer.it_interval.tv_sec = 0;
  v_timer.it_interval.tv_usec = 0;
}

void timer_handler(int signum)
{
  // time has run out for the thread, reset its timer and  add it to the end
  // of the ready queue, and run the next thread
  thread_t* cur = &(sched_.get_cur_running_thread());
  cur->time_slice_ = v_timer;
  cur->state_ = BLOCKED;
  if (sigsetjmp(cur->env_, 1) != 0)
  {
	  return;
  }
  // cur.reset();
  sched_.schedule(cur);
}


//std::queue<std::shared_ptr<thread_t>> r_queue;
//sigjmp_buf env[MAX_THREAD_NUM];


/**
 * @brief initializes the thread library.
 *
 * Once this function returns, the main thread (tid == 0) will be set as RUNNING. There is no need to
 * provide an entry_point or to create a stack for the main thread - it will be using the "regular" stack and PC.
 * You may assume that this function is called before any other thread library function, and that it is called
 * exactly once.
 * The input to the function is the length of a quantum in micro-seconds.
 * It is an error to call this function with non-positive quantum_usecs.
 *
 * @return On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
  // TODO SIGNAL MASKING !!!
  // ignore SIGVTALRM
  sigemptyset(&sigset_);
  sigaddset(&sigset_, SIGVTALRM);
  sigprocmask(SIG_SETMASK, &sigset_, nullptr);
  // check if quantum_usecs is a positive integeR
  if (quantum_usecs > 0)
  {
	// save quantum_usecs in a global quantum variable, to be used in the
	// timers of the rest of the threads.
	quantum = quantum_usecs;

	// set up timer and timer handler
	setup_v_timer();

	struct sigaction sa = {nullptr};
	sa.sa_handler = &timer_handler;
	if (sigaction (SIGVTALRM, &sa, nullptr) == -1)
	{
		sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
	  // TODO sigaction error
	  return -1;
	}

	// initialize the main thread
	thread_t* main_t = new thread_t();

  	// sched_.ready_thread(main_t);
  	used_tids_[0] = 1;

  	// init scheduler
	sched_.set_main_thread(main_t);

	// init schedueler
	// sched_.ready_thread (main_t);
	// sched_.run_main_thread();

	// return 0 on success
  	// sigsetjmp(main_t.env_, 1);
  	sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
	return 0;
  }
  sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
  return -1;
}

/**
 * @brief get the minimal available tid
 * @return minimal available tid, -1 if no availabe tids (more than MAX_THREADS are currently running
 */
tid_t get_min_tid()
{
	// iterate over avail tids and get the minimal one
	for (tid_t id = 0; id < MAX_THREAD_NUM; ++id)
	{
		if (!used_tids_[id])
		{
			return id;
		}
	}
	return -1;
}



int uthread_spawn(thread_entry_point entry_point)
{
	sigprocmask(SIG_SETMASK, &sigset_, nullptr);
	if (sigsetjmp(sched_.get_main_thread().env_, 1) != 0)
	{
		return 0;
	}
	// get a new minimal tid, if tid == -1 then now more avaialable room for new threads
	tid_t tid = get_min_tid();
	used_tids_[tid] = 1;
	if (tid > -1)
	{
		thread_t *new_thread = new thread_t(tid, v_timer, entry_point);
		// add thread to scheduler queue
		std::cout << "putting thread in queue..." << std::endl;

		sched_.schedule(new_thread);
		std::cout << "SUCCESS" << std::endl;
		sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
		return 0;
	}
	sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
	return -1;
}


int uthread_terminate(int tid)
{
	sigprocmask(SIG_SETMASK, &sigset_, nullptr);
	// if tid in use, the thread exists and can be termianted
	if (used_tids_[tid])
	{
		// set the tid to unused
		used_tids_[tid] = 0;
		// termination of a non main thread:
		if (tid != 0)
		{
			// get thread from scheduler
			thread_t* t_ptr = sched_.get_thread(tid);
			// add thread to ignored queue, which will remove the
			// thread from the queue when the time comes
			// and delete the stack
			sched_.add_to_ignored(tid);
			delete [] t_ptr->stack_;
		}
		return 0;
	}

	sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
	return -1;
}
