//
// Created by lennehberg on 6/13/24.
//
#include "uthreads.h"
#include "Schedueler.h"
#include "arch_utils.h"
#include "thread_utils.h"
#include <iostream>
#include <cerrno>

#include <cstring>

#define BLOCK_V_TIMER() sigprocmask(SIG_SETMASK, &sigset_, nullptr);
#define UNBLOCK_V_TIMER() sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);

/// GLOBALS ///
int used_tids_[MAX_THREAD_NUM] = {0};
int quantum = 0;
int total_quantums = 0;
Schedueler sched_;
itimerval v_timer_;
itimerval main_timer = {0};
sigset_t sigset_;





void error_handler(std::string msg)
{
    std::cerr << msg << std::endl;
    exit(1);
}

/**
 * timer handler for timing threads
 */
void timer_handler(int signum)
{
    ++total_quantums;
    if (uthread_get_tid() != 0)
    {

        // fetch the current running thread from the schedueler and
        // reset its timer, set it's state to blocked (purely for semantic purposes)
        thread_t *cur = sched_.get_cur_thread();
        // cur->total_quantums++;
        cur->time_slice_ = v_timer_;
        cur->state_ = BLOCKED;

        // save the state of the thread and schedule the schedueler
        if (sigsetjmp(cur->env_, 1) != 0)
        {
            return;
        }
    }
    else
    {
        thread_t *cur = sched_.get_cur_thread();
        cur->time_slice_ = main_timer;
        // cur->total_quantums++;
        if (sigsetjmp(cur->env_, 1) != 0)
        {
            return;
        }
    }

    sched_.schedule();

}

/*
 * setups timer according to quantum
 */
void setup_v_timer()
{
    v_timer_.it_value.tv_sec = 0;
    v_timer_.it_value.tv_usec = quantum;
    v_timer_.it_interval.tv_sec = 0;
    v_timer_.it_interval.tv_usec = quantum;

    // TODO try to set virtual timer here
    // so that time starts ticking on init,
    // handler shouldn't switch from main until a new thread is spawned
    setitimer(ITIMER_VIRTUAL, &v_timer_, nullptr);

    main_timer.it_interval.tv_usec = quantum;
    main_timer.it_value.tv_usec = quantum;

    struct sigaction sa = {nullptr};
    sa.sa_handler = &timer_handler;
    if (sigaction (SIGVTALRM, &sa, nullptr) == -1)
    {
        // sigprocmask(SIG_UNBLOCK, &sigset_, nullptr);
        // sigaction error
        std::string err_msg = "system error: sigaction failed: ";
        err_msg += strerror(errno);
        error_handler(err_msg);
    }
}

/**
 * sets up a sigset to block SIGVTALRM
 */
void setup_sigmask()
{
   sigemptyset(&sigset_);
   sigaddset(&sigset_, SIGVTALRM);
}


int uthread_init(int quantum_usecs)
{
    int ret = -1;
    // set signal mask to block SIGVTALRM
    setup_sigmask();
    BLOCK_V_TIMER()

    // check if quantum is positive
    if (quantum_usecs > 0)
    {
        // save quantum_usecs in a global quantum variable, to be used in the
        // timers of the rest of the threads.
        quantum = quantum_usecs;

        // set up timer and timer handler
        setup_v_timer();
        // init main thread
        sched_.init_thread(0, main_timer, nullptr);
        UNBLOCK_V_TIMER()
        if (sigsetjmp(sched_.get_cur_thread()->env_, 1) != 0)
        {
            return 0;
        }
        BLOCK_V_TIMER();
        used_tids_[0] = 1;
        total_quantums = 1;
        ret = 0;
    }
    else
    {
        error_handler("thread library error: quantum should be a positive integer! ");
    }
    UNBLOCK_V_TIMER()
    return ret;
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
  	BLOCK_V_TIMER()
  	tid_t min_tid;
    int ret = -1;

    if (entry_point)
    {
        min_tid = get_min_tid();
        if (min_tid > -1)
        {
            ret = min_tid;
            // init a new thread with that tid
            used_tids_[min_tid] = 1;
            sched_.init_thread(min_tid, v_timer_, entry_point);
        }
    }
    else
    {
        error_handler("thread library error: Thread entry point cannot be NULL!");
    }
    UNBLOCK_V_TIMER()
//    if (ret == 0 && uthread_get_tid() == 0)
//    {
//        sched_.schedule();
//    }
    return ret;
}


int uthread_terminate(int tid)
{
    BLOCK_V_TIMER()
    int ret = -1;
    // if tid == 0, then terminate the whole program and exit(0)
    if (tid == 0)
    {
        sched_.terminate();
        exit(0);
    }
   // else, remove the thread from the schedueler
    else if (used_tids_[tid])
    {
        used_tids_[tid] = 0;
        total_quantums++;
        sched_.remove_thread(tid);
        ret = 0;
    }
    // negative tid error, no existing tid error
    else
    {
        std::string erro_msg = "thread library error: trying to terminate a thread with id ";
        erro_msg += std::to_string(tid);
        erro_msg += " which doesn't exist!";
        error_handler(erro_msg);
    }

    UNBLOCK_V_TIMER()
    return ret;
}


int uthread_block(int tid)
{
    BLOCK_V_TIMER()
    // if id is 0, raise en error
    int ret = -1;
    if (tid == 0)
    {
        error_handler("thread library error: Blocking main thread is NOT allowed!");
    }


    else if (used_tids_[tid])
    {
        // if the thread is blocking itself (the currently runnning
        // theead, save its state (if the thread is not currently running,
        // then it's state was saved previously, either here or in the timer
		// handler)
        if (tid == sched_.get_cur_thread()->tid_)
        {
            if (sigsetjmp(sched_.get_cur_thread()->env_, 1) != 0)
            {
                return 0;
            }
        }
        sched_.block_thread(tid);
        ret = 0;
    }
    else  // negative tid error, no existing tid error
    {
        error_handler("thread library error: Thread id does not exist!");
    }
    UNBLOCK_V_TIMER()

    return ret;
}

int uthread_resume(int tid)
{
    BLOCK_V_TIMER()
    int ret = -1;
    // if id not in use, raise error
    if (tid == 0 || !used_tids_[tid])
    {
        error_handler("thread library error: Invalid ID to resume!");
    }
    if (used_tids_[tid])
    {
        sched_.resume_thread(tid);
        if (uthread_get_tid() == 0)
        {
            sched_.schedule();
        }
        ret = 0;
    }
    UNBLOCK_V_TIMER()
    return ret;

}

int uthread_sleep(int num_quantums)
{
    BLOCK_V_TIMER()
    int ret = -1;
    if (uthread_get_tid() == 0)
    {
        error_handler("thread library error: Cannot put main to sleep!");
    }
    // negative num_quantums error
    if (num_quantums > 0)
    {
        // TODO main thread is the only thread not sleeping
        // TODO sets the timer to intervals of quantums
        // set the jump point for the cur thread
        if (sigsetjmp(sched_.get_cur_thread()->env_, 1) != 0)
        {
            return 0;
        }
        sched_.sleep_running_thread(num_quantums);
        ret = 0;
    }
    else
    {
        error_handler("thread library error: number of quantums must be a positive integer!");
    }
    UNBLOCK_V_TIMER()
    return ret;
}

int uthread_get_tid()
{
    return sched_.get_cur_thread()->tid_;
}

int uthread_get_total_quantums()
{
    return total_quantums;
}

int uthread_get_quantums(int tid)
{
    // TODO non existant tid error
    if (!used_tids_[tid])
    {
        error_handler("thread library error: Can't get quantums as no thread exists with that ID");
    }
    itimerval debug_timer;
    getitimer(ITIMER_VIRTUAL, &debug_timer);
    return sched_.get_total_quantums(tid);
}
