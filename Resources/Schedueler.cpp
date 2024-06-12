//
// Created by ubuntu on 6/10/24.
//

#include "Schedueler.h"

//void Schedueler::setup_timer ()
//{
//  // set up the timer to be paused upon initialization
//  v_timer_.it_interval.tv_sec = 0;
//  v_timer_.it_interval.tv_usec = 0;
//  v_timer_.it_value.tv_sec = 0;
//  v_timer_.it_value.tv_usec = 0;
//  if (setitimer (ITIMER_VIRTUAL, &v_timer_, NULL) == -1)
//  {
//	// TODO handle error in case setitimer fails
//	return;
//  }
//
//}
//
//void Schedueler::timer_handler (int sig)
//{
//  // when a SIGVTALRM is raised, save the current time left for the thread
//  // start the scheduling process
//  if (getitimer (ITIMER_VIRTUAL, &v_timer_) == 0)
//  {
//	cur_thread->time_slice.tv_sec = v_timer_.it_value.tv_sec;
//	cur_thread->time_slice.tv_usec = v_timer_.it_value.tv_usec;
//  }
//
//}

void Schedueler::ready_thread(thread_t& thread)
{
  // add the thread to the ready queue and set the state of the thread to READY
  thread.state = READY;
  r_qu_.push(std::make_shared<thread_t>(thread));
}

void Schedueler::run_next_thread ()
{
  // get the thread at the head of the queue and set timer to the timeslice
  // left in the thread
  std::shared_ptr<thread_t> thread_ptr;
  if (!(r_qu_.empty()))
  {
	thread_ptr = r_qu_.front ();
	r_qu_.pop ();
	cur_thread = thread_ptr;
  }

  // set the cur thread to running, set the timer to the relevant time slice
  // left for the cur thread, and siglongjmp
  cur_thread->state = RUNNING;

  setitimer (ITIMER_VIRTUAL, &(cur_thread->time_slice), NULL);

  siglongjmp ((cur_thread->env), 1);
}

std::shared_ptr<thread_t> Schedueler::get_cur_running_thread ()
{
  return cur_thread;
}

int Schedueler::block_thread (tid_t tid)
{
  // block the current thread by:
  // saving the timer,
  // setting a jump to the current env of thread,
  // setting state to BLOCKED
  getitimer (ITIMER_VIRTUAL, &(cur_thread->time_slice));
  sigsetjmp(cur_thread->env, 1);
  cur_thread->state = BLOCKED;
  run_next_thread();
}