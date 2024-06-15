//
// Created by lennehberg on 6/13/24.
//

#include "Schedueler.h"

#include <thread>
#include <utility>

/**
 * Schedueler class, in charge of managing thread timings and the ready queue
 */

void Schedueler::ready_thread(thread_t *n_thread)
{
    // set the thread state to READY and push to the back of the queue
    n_thread->state_ = READY;
    rdy_qu_.push_back(n_thread);
    // if the current thread is the main thread, run the next thread
    // if (cur_run_->tid_ == 0) // cur_run is initialize to main thread when init is called
    // {
    //     run_next_thread();
    // }
}

void Schedueler::run_next_thread()
{
    if (!rdy_qu_.empty())
    {
        // pop the first thread from the queue and set it to the cur running thread
        cur_run_ = rdy_qu_.front();
        rdy_qu_.pop_front();

        // if the queue is empty after the pop, insert the main thread
        // if (rdy_qu_.empty())
        // {
        //     m_thread_->state_ = READY;
        //     rdy_qu_.push_back(m_thread_);
        // }
    }
    else
    {
        cur_run_ = m_thread_;
    }
    // set the cur running state to running and jump
    cur_run_->state_ = RUNNING;
    ++cur_run_->total_quantums;
    itimerval debug_timer;
    setitimer(ITIMER_VIRTUAL, &cur_run_->time_slice_, nullptr);
    getitimer(ITIMER_VIRTUAL, &debug_timer);
    siglongjmp(cur_run_->env_, 1);
}

void Schedueler::init_thread(tid_t tid, itimerval time_slice, thread_entry_point entry_point)
{
    thread_t *n_thread;
    // if the tid == 0, then init a main thread and set it to running
    if (tid == 0)
    {
        n_thread = new thread_t;
        n_thread->state_ = RUNNING;
        n_thread->time_slice_ = time_slice;
        cur_run_ = n_thread;
        m_thread_ = n_thread;
    }
    else
    {
        n_thread = new thread_t(tid, time_slice, entry_point);
        active_threads_.insert({n_thread->tid_,n_thread});
        ready_thread(n_thread);
    }
    active_threads_.insert({n_thread->tid_,n_thread});
}

void Schedueler::schedule()
{
    // wake sleeping threads that need to wake up,
    // put the cur_run thread at the end of the queue
    wake_threads();
    ready_thread(cur_run_);
    run_next_thread();
}

void Schedueler::remove_thread(tid_t tid)
{
    bool run_next = false;
    // set the state of the thread to terminated and
    // erase the thread from the active map
    if (active_threads_[tid]->state_ == READY || active_threads_[tid]->state_ == RUNNING)
    {
        active_threads_[tid]->state_ = TERMINATED;


        // remove thread from the queue
        for (auto q_iter = rdy_qu_.cbegin(); q_iter != rdy_qu_.cend(); ++q_iter)
        {
            if ((*q_iter)->tid_ == tid)
            {
                rdy_qu_.erase(q_iter);
                break;
            }
        }

        if (cur_run_->tid_ == tid)
        {
            run_next = true;
        }
        delete active_threads_[tid];
        active_threads_.erase(tid);

    }
    else // state of thread is blocked, remove from blocked list / sleeping list
    {
        // find thread in sleeping list
        bool in_sleeping = false;
        thread_t *e_thread = nullptr;
        for (auto thread_iter = sleeping_.begin(); thread_iter != sleeping_.end(); ++thread_iter)
        {
            if ((*thread_iter)->tid_ == tid)
            {
                e_thread = *thread_iter;
                in_sleeping = true;
                sleeping_.erase(thread_iter);
                break;
            }
        }
        if (!in_sleeping)
        {
            e_thread = blocked_[tid];
            blocked_.erase(tid);
        }

        delete e_thread;

    }
    active_threads_.erase(tid);
    if (run_next)
    {
        run_next_thread();
    }
}

void Schedueler::block_thread(tid_t tid)
{
    if (tid == cur_run_->tid_)
    {
        // get a snapshot of the timer of the thread
        getitimer(ITIMER_VIRTUAL, &cur_run_->time_slice_);
        // move thread to the blocked list and set it state as blocked
        cur_run_->state_ = BLOCKED;
        blocked_.insert({tid, cur_run_});
        // run next thread
        // if (rdy_qu_.empty())
        // {
        //     ready_thread(m_thread_);
        // }
        run_next_thread();
    }
    else // tid is not of cur_run_
    {
        active_threads_[tid]->state_ = BLOCKED;
        blocked_.insert({tid, active_threads_[tid]});
        // remove thread from ready queue
        for (auto q_iter = rdy_qu_.cbegin(); q_iter != rdy_qu_.cend(); ++q_iter)
        {
            if ((*q_iter)->tid_ == tid)
            {
                rdy_qu_.erase(q_iter);
                break;
            }
        }
    }
}

void Schedueler::resume_thread(tid_t tid)
{
    // if the id is in blocked, put the thread at the end of the queue
    thread_t* b_thread = blocked_[tid];
    if (b_thread)
    {
        blocked_.erase(tid);
        ready_thread(b_thread);
    }
}

void Schedueler::sleep_running_thread(int num_quantums)
{
    // set the state of the current thread to BLOCKED,
    // save num_quantums in asleep for
    // save the timer, move to sleeping, run next thread
    cur_run_->state_ = BLOCKED;
    cur_run_->asleep_for_ = num_quantums;
    getitimer(ITIMER_VIRTUAL, &cur_run_->time_slice_);
    sleeping_.push_back(cur_run_);
    run_next_thread();
}

void Schedueler::wake_threads()
{
    for (auto thread_iter = sleeping_.begin(); thread_iter != sleeping_.end(); ++thread_iter)
    {
        if (sleeping_.empty())
        {
            break;
        }
        // update threads asleep for time, if any reached 0
        // wake up the thread
        thread_t* to_wake = *thread_iter;
        --to_wake->asleep_for_;
        if ((*thread_iter)->asleep_for_ <= 0)
        {
            --to_wake->asleep_for_;
            sleeping_.erase(thread_iter);
            ready_thread(to_wake);
        }
    }
}

int Schedueler::get_total_quantums(tid_t tid)
{
    return active_threads_[tid]->total_quantums;
}

void Schedueler::terminate()
{
    // remove threads from active threads, no need to worry about other lists
    for (auto thread_p : active_threads_)
    {
        if (thread_p.first == 0)
        {
            continue;
        }
        delete thread_p.second;
    }
}






















/// COMMENTED OUT REVIOUS RUNNABLE CODE ///

/// READY & RUN THREAD ///
// // if the queue only has one thread, insert the main thread (cur_run)
// // after it
// thread_t *main_thread = nullptr;
// if (rdy_qu_.size() == 1)
// {
//     main_thread = cur_run_;
//     cur_run_ = nullptr;
//
// }
//
// // note, even though the ready and run methods
// // are recursive within each other, they wont reach a
// // recusive "loop" as due to the conditions of this
// // schedueler
// cur_run_ = rdy_qu_.front();
// rdy_qu_.pop_front();
//
// cur_run_->state_ = RUNNING;
// if (main_thread && !rdy_qu_.empty())
// {
//     ready_thread(main_thread);
// }
// set timer to the timer in thread

// if (!rdy_qu_.empty())
// {
//     // insert the thread to the end of the queue, and insert the main thread behind it
//     // assuming the main thread is always at the end of the queue
//     thread_t *tmp_thread = rdy_qu_.back();
//     rdy_qu_.pop_back();
//     rdy_qu_.push_back(n_thread);
//     rdy_qu_.push_back(tmp_thread);
// }
// else
// {
//     // insert the main thread and the new thread manually
//     // seeing as though the main thread is not in the queue, it is the
//     // currenly running thread, because it cannot be blocked, so we can
//     // just extract it from there:
//     rdy_qu_.push_back(n_thread);
//     cur_run_->state_ = READY; // set the state of the main thread to READY
//     rdy_qu_.push_back(cur_run_);
//     // if the queue is empty and we receive a new thread, run it
//     run_next_thread();
// }