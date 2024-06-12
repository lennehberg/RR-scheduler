//
// Created by ubuntu on 6/10/24.
//

#ifndef _SCHEDUELER_H_
#define _SCHEDUELER_H_


#include "thread_utils.h"
#include <queue>



class Schedueler
{
 private:
  std::queue<std::shared_ptr<thread_t>> r_qu_;
  std::vector<std::shared_ptr<thread_t>> blocked_;
  // itimerval v_timer_;
  std::shared_ptr<thread_t> cur_thread;

//  void setup_timer();
//
//  void timer_handler(int sig);

 public:
  Schedueler(){
//	cur_thread = std::make_shared<thread_t>(main_t);
//	cur_thread->state = RUNNING;
  }


  /**
   * @brief adds a thread's id to the end of the ready queue
   * @param tid
   * @return 0 on success, -1 on failure
   */
  void ready_thread(thread_t& thread);

  /**
   * @return the next thread's id in the ready queue
   */
  void run_next_thread();


  std::shared_ptr<thread_t> get_cur_running_thread();

  /**
   * @brief block a thread with id tid, no need for error checking
   * @param tid
   * @return
   */
  int block_thread(tid_t tid);
};

#endif //_SCHEDUELER_H_
