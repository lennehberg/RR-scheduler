//
// Created by ubuntu on 6/12/24.
//
#include "uthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

/**
 * tests the init function of the library
 */
int test_init();

/**
 * tests spawning new threads
 * @return
 */
int test_spawn();

/**
 * test treminating a thread
 * @return
 */
int test_terminate();

/**
 * test the blocking ability of the library
 * @return
 */
int test_blocking();

/**
 * tests the resuming ability of the library
 * @return
 */
int test_resuming();

int test_sleeping();

int main()
{
	// test_init();
	// test_spawn();
	// test_terminate();
	// test_blocking();
	test_resuming();
	// test_sleeping();
}

int test_init()
{
  int ret = 0;
  if (uthread_init (1000) == 0)
  {
	std::cout << "Init library successful" << std::endl;
	ret = 0;
  }
  else
  {
	std::cout << "Init failed!" << std::endl;
	ret = -1;
  }
  return ret;
}

void thread_func1()
{
	int i = 0;
	while (true)
	{
		++i;
		if (i == 1)
			std::cout << "Thread func 1 executing... " << std::endl;
	}
}

void thread_func()
{
	int i = 0;
	while(true)
	{
		++i;
		if (i % 500000000000 == 0)
		{
			std::cout << "Thread func executing ... " << std::endl;
			std::cout << "Spawning a nested thread... " << std::endl;
			uthread_spawn(&thread_func1);
		}}

}

void self_term_thread()
{
	unsigned int i = 0;
	while(true)
	{
		if (i == 0)
		{
			std::cout << "In thread... " << std::endl;
		}
		++i;
		if (i % 50000000 == 0)
		{
			std::cout << "Terminating thread... " << std::endl;
			uthread_terminate(1);
		}
	}
}

int test_spawn()
{
	int ret[3] = {0};
	if (uthread_init(1) == 0)
	{
		ret[0] = uthread_spawn(&thread_func1);
		std::cout << "Thread successfuly spawned!" << std::endl;
		ret[1] = uthread_spawn(&thread_func1);
		// ret[2] = uthread_spawn(&thread_func);
	}
	if (ret[0] == 0 && ret[1] == 0 && ret[2] == 0)
	{
		std::cout << "thread spawning successful!" << std::endl;
		return 0;
	}
	std::cout << "thread spawning failed!" << std::endl;
	return -1;
}


int test_terminate()
{
	int ret = 0;
	if (uthread_init(10) == 0)
	{
		uthread_spawn(&self_term_thread);
		std::cout << "Terminating thread tid 1... " << std::endl;
		// ret = uthread_terminate(1);
		if (ret == 0)
		{
			std:: cout << "Terminated successfully!" << std::endl;
		}

		std::cout << "Spawning another thread... " << std::endl;
		uthread_spawn(&self_term_thread);
		std::cout << "Terminating thread tid 1... " << std::endl;
		ret = uthread_terminate(1);
		if (ret == 0)
		{
			std:: cout << "Terminated successfully!" << std::endl;
		}
	}
	return ret;
}

void blocking_thread();

void thread_to_block()
{
	int i = 0;
	while(true)
	{
		++i;
		if (i == 1)
		{
			std::cout << "thread 1 waiting to be blocked... " << std::endl;
			// uthread_spawn(&blocking_thread);
		}
	}

}

void blocking_thread()
{
	int i = 0;
	while(true)
	{
		++i;
		if (i == 1)
		{
			std::cout << "thread 2" << std::endl;
			std::cout << "blocking thread 1... ";
			if (uthread_block(1) == 0)
			{
				std::cout << "block successful!" << std::endl;
			}
			std::cout << "blocking myself... " << std::endl;
			if (uthread_block(2) == 0)
			{
				std::cout << "huh?" << std::endl;
			}
		}
	}
}

int test_blocking()
{
	int ret = 0;
	if (uthread_init(1000) == 0)
	{
		uthread_spawn(&thread_to_block);
		uthread_spawn(&blocking_thread);

	}
	return ret;
}

int test_resuming()
{
	int ret = 0;
	if (uthread_init(1000) == 0)
	{
		uthread_spawn(&thread_to_block);
		uthread_spawn(&blocking_thread);
		std::cout << "Resuming thread 1..." << std::endl;
		if (uthread_resume(1) == -1)
		{
			ret = -1;
		}
		itimerval debug_t;
		getitimer(ITIMER_VIRTUAL, &debug_t);
		std::cout << "Resuming thread 2..." << std::endl;
		if (uthread_resume(2) == -1)
		{
			ret = -1;
		}

	}
	return ret;
}

bool woke_up = false;
void sleeping_thread()
{
	int i = 0;
	while (true)
	{
		++i;
		if (i == 1)
		{
			std::cout << "Going to sleep at ... " << uthread_get_total_quantums() << " quantums of the process" << std::endl;
			uthread_sleep(3);
			std::cout << "I resumed after... " << uthread_get_total_quantums() \
						<< " quantums of the process" << std::endl;
			woke_up = true;
			uthread_terminate(uthread_get_tid());
		}
	}
}

void father_thread()
{
	int i = 0;
	while(true)
	{
		++i;
		if (i == 1)
		{
			std::cout << "Spawning a sleeping thread... " << std::endl;
			uthread_spawn(&sleeping_thread);
		}
		if (i == 0)
		{
			std::cout << "father thread has been running for... " << uthread_get_quantums(uthread_get_tid()) << std::endl;;
			std::cout << "terminating... " << std::endl;
			uthread_terminate(uthread_get_tid());
		}

	}
}

int test_sleeping()
{
	int ret = -1;
	if (uthread_init(100000) == 0)
	{

		int i = 0;
		while(!woke_up)
		{
			itimerval debug_timer;
			getitimer(ITIMER_VIRTUAL, &debug_timer);
			if (i == 0)
			{

				{
					std::cout << "Spawning the father thread... " << std::endl;
					uthread_spawn(&father_thread);
					ret = 0;
					++i;
				}
			}

		}
	}
	return ret;
}