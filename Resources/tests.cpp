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

int main()
{
	test_init();
}

int test_init()
{
  if (uthread_init (1000) == 0)
  {
	std::cout << "Init library successful" << std::endl;
  }
  else
  {
	std::cout << "Init failed!" << std::endl;
  }
}