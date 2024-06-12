//
// Created by ubuntu on 6/9/24.
//

#include "uthreads.h"
#include <iostream>

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */


/// GLOBALS ///
int avail_tids_[MAX_THREAD_NUM] = {0};
int quantum = 0;
Schedueler sched_;
itimerval v_timer;

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
  std::shared_ptr<thread_t> cur = sched_.get_cur_running_thread();
  cur->time_slice_ = v_timer;
  sched_.ready_thread (*cur);
  sched_.run_next_thread();
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
  // check if quantum_usecs is a positive integer
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
	  // TODO sigaction error
	  return -1;
	}

	// initialize the main thread
	thread_t main_t;
  	sched_.set_main_thread(main_t);
  	sched_.ready_thread(main_t);
  	avail_tids_[0] = 1;
	sigsetjmp(main_t.env_, 1);

	// init schedueler
	sched_.ready_thread (main_t);
	sched_.run_next_thread();

	// return 0 on success
	return 0;
  }

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
		if (!avail_tids_[id])
		{
			return id;
		}
	}
	return -1;
}



int uthread_spawn(thread_entry_point entry_point)
{
	// get a new minimal tid, if tid == -1 then now more avaialable room for new threads
	tid_t tid = get_min_tid();
	if (tid > -1)
	{
		thread_t new_thread(tid, v_timer, entry_point);
		// add thread to scheduler queue
		std::cout << "putting thread in queue..." << std::endl;
		sched_.ready_thread(new_thread);
		std::cout << "SUCCESS" << std::endl;
		return 0;
	}

	return -1;
}