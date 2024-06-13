//
// Created by lennehberg on 6/13/24.
//

#include "Schedueler.h"

#include <utility>

/**
 * Schedueler class, in charge of managing thread timings and the ready queue
 */

void Schedueler::ready_thread(thread_t *n_thread)
{
    if (!rdy_qu_.empty())
    {
        // insert the thread to the end of the queue, and insert the main thread behind it
        // assuming the main thread is always at the end of the queue
        thread_t *tmp_thread = rdy_qu_.back();
        rdy_qu_.pop_back();
        rdy_qu_.push_back(n_thread);
        rdy_qu_.push_back(tmp_thread);
    }
    else
    {
        // insert the main thread and the new thread manually
        // seeing as though the main thread is not in the queue, it is the
        // currenly running thread, because it cannot be blocked, so we can
        // just extract it from there:
        rdy_qu_.push_back(n_thread);
        cur_run_->state_ = READY; // set the state of the main thread to READY
        rdy_qu_.push_back(cur_run_);
        // if the queue is empty and we receive a new thread, run it
        // run_next_thread();
    }
}

void Schedueler::run_next_thread()
{
    cur_run_ = rdy_qu_.front();
    rdy_qu_.pop_front();
    cur_run_->state_ = RUNNING;
    // set timer to the timer in thread
    setitimer(ITIMER_VIRTUAL, &cur_run_->time_slice_, nullptr);
    siglongjmp(cur_run_->env_, 1);
}

void Schedueler::init_thread(tid_t tid, itimerval& time_slice, thread_entry_point entry_point)
{
    thread_t *n_thread;
    // if the tid == 0, then init a main thread and set it to running
    if (tid == 0)
    {
        n_thread = new thread_t;
        n_thread->state_ = RUNNING;
        cur_run_ = n_thread;
    }
    else
    {
        n_thread = new thread_t(tid, time_slice, entry_point);
        ready_thread(n_thread);
    }
    // active_threads_.insert({n_thread->tid_,n_thread});
}

void Schedueler::schedule(thread_t *thread)
{
    ready_thread(thread);
    run_next_thread();
}



