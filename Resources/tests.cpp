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

int main()
{
	test_init();
	test_spawn();
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


void thread_func()
{
	while(true)
	{
		std::cout << "Thread func executing ... " << std::endl;
	}

}

int test_spawn()
{
	int ret[3] = {0};
	if (uthread_init(10) == 0)
	{
		ret[0] = uthread_spawn(&thread_func);
		std::cout << "Thread successfuly spawned!" << std::endl;
		ret[1] = uthread_spawn(&thread_func);
		ret[2] = uthread_spawn(&thread_func);
	}
	if (ret[0] == 0 && ret[1] == 0 && ret[2] == 0)
	{
		std::cout << "thread spawning successful!" << std::endl;
		return 0;
	}
	std::cout << "thread spawning failed!" << std::endl;
	return -1;
}
