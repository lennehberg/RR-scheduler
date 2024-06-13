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

int test_terminate();

int main()
{
	test_init();
	test_spawn();
	// test_terminate();
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
		if (i % 100000000000 == 0)
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

int test_spawn()
{
	int ret[3] = {0};
	if (uthread_init(10) == 0)
	{
		ret[0] = uthread_spawn(&thread_func);
		std::cout << "Thread successfuly spawned!" << std::endl;
		// ret[1] = uthread_spawn(&thread_func);
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


// int test_terminate()
// {
// 	int ret = 0;
// 	if (uthread_init(10) == 0)
// 	{
// 		uthread_spawn(&thread_func);
// 		std::cout << "Terminating thread tid 1... " << std::endl;
// 		ret = uthread_terminate(1);
// 		if (ret == 0)
// 		{
// 			std:: cout << "Terminated successfully!" << std::endl;
// 		}
//
// 		std::cout << "Spawning another thread... " << std::endl;
// 		uthread_spawn(&thread_func);
// 		std::cout << "Terminating thread tid 1... " << std::endl;
// 		ret = uthread_terminate(1);
// 		if (ret == 0)
// 		{
// 			std:: cout << "Terminated successfully!" << std::endl;
// 		}
// 	}
// 	return ret;
// }
