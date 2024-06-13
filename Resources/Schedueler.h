//
// Created by ubuntu on 6/10/24.
//

#ifndef SCHEDUELER_H_
#define SCHEDUELER_H_


#include "thread_utils.h"



class Schedueler
{
 private:
    std::deque<thread_t *> rdy_qu_;
    std::map<tid_t, thread_t *> active_threads_;
    std::map<tid_t, thread_t *> blocked_;
    thread_t *cur_run_;

    /**
     * puts the thread at the end of the queue, and puts the main
     * thread afterwards
     * @param n_thread thread to add to ready queue
     */
    void ready_thread(thread_t *n_thread);

    /**
     * Sets the thread in the front of the queue to running
     */
    void run_next_thread();

public:
 /**
* inits a new thread, puts at the end of the ready queue
* if tid = 0, then init a main thread and immediatly set it to running
* @param tid id of new thread
*/
 void init_thread(tid_t tid, itimerval& time_slice, thread_entry_point entry_point= nullptr);

 /**
  *
  * @return the currently running thread
  */
 thread_t *get_cur_thread() { return cur_run_; }

 /**
  * schedule the next thread in the queue to run
  * @param thread thread to add to the end of the queue
  */
 void schedule(thread_t *thread);
};

#endif //SCHEDUELER_H_
