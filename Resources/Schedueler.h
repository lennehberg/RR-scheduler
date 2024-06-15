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
    std::vector<thread_t *> sleeping_;
    thread_t *cur_run_ = nullptr;
    thread_t *m_thread_ = nullptr;

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
 void init_thread(tid_t tid, itimerval time_slice, thread_entry_point entry_point= nullptr);

 /**
  *
  * @return the currently running thread
  */
 thread_t *get_cur_thread() { return cur_run_; }

 /**
  * schedule the next thread in the queue to run
  * @param thread thread to add to the end of the queue
  */
 void schedule();

 /**
  * removes thread from the active threads map
  * delete its instance and makes the schedueler skip
  * the thread and remove from queue when the time comes
  * @param tid
  * @return
  */
  void remove_thread(tid_t tid);

    /**
     * blocks thread with tid, if the blocked thread is currently running,
     * stops, save its time_slice_ using getitimer, and put thread in blocked
     * list
     * if the thread is not currently running, just put it in the blocked list
     * @param tid
     */
    void block_thread(tid_t tid);

    /**
     * resumes a blocked thread with id tid, if the thread is running or ready,
     * no effect on the thread
     * @param tid
     */
    void resume_thread(tid_t tid);

    /**
     * puts the currently running thread to sleep for num_quantum cycles,
     * and runs the next thread
     * @param num_quantums
     */
    void sleep_running_thread(int num_quantums);

    /**
    * reutrns the total quantums a thread with the
    * specified tid has run
    * @param tid
    * @return
    */
    int get_total_quantums(tid_t tid);

    /**
    * wake threads after their asleep period has ended, and puts
    * relevant threads at the end of the queue
    */
    void wake_threads();

    /**
     * terminates all the threads from the schedueler, other than the main thread
     */
    void terminate();
};

#endif //SCHEDUELER_H_
