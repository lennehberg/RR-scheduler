//
// Created by ubuntu on 6/9/24.
//

#include "uthreads.h"
#include "Schedueler.h"

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */


/// GLOBALS ///
tid_t min_tid = -1;
tid_t max_tid = 1;
tid_t cur_tid = 0;
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
  cur->time_slice = v_timer;
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
	thread_t main_t = {0};
	sigsetjmp(main_t.env, 1);

	// init schedueler
	sched_.ready_thread (main_t);
	sched_.run_next_thread();

	// return 0 on success
	return 0;
  }

  return -1;
}