//
// Created by ubuntu on 6/10/24.
//

#include "Schedueler.h"
#include <iostream>

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
  thread.state_ = READY;
  r_qu_.push(std::make_shared<thread_t>(thread));

  // if the current thread is not set, set the current thread to be the main thread
  if (!cur_thread_)
  {
    cur_thread_ = m_thread_;
  }

  // if the current running thread is main thread (tid 0)
  // if (cur_thread_->tid_ == 0)
  // {
  //   run_next_thread();
  // }
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
	cur_thread_ = std::move(thread_ptr);
    if (r_qu_.empty())
    {
      ready_thread(*m_thread_);
    }
  }

  // set the cur thread to running, set the timer to the relevant time slice
  // left for the cur thread, and siglongjmp
  cur_thread_->state_ = RUNNING;

  setitimer (ITIMER_VIRTUAL, &(cur_thread_->time_slice_), NULL);
  if (cur_thread_->tid_ != 0) {
    std::cout << "jumping to thread..." << std::endl;
    siglongjmp (cur_thread_->env_, 1);
  }

}

std::shared_ptr<thread_t> Schedueler::get_cur_running_thread ()
{
  return cur_thread_;
}

void Schedueler::block_thread (tid_t tid)
{
  // block the current thread by:
  // saving the timer,
  // setting a jump to the current env of thread,
  // setting state to BLOCKED
  getitimer (ITIMER_VIRTUAL, &(cur_thread_->time_slice_));
  sigsetjmp(cur_thread_->env_, 1);
  cur_thread_->state_ = BLOCKED;
  run_next_thread();
}