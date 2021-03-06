/** Copyright (C) 2016 European Spallation Source */

#include <iostream>
#include <libs/include/Thread.h>
#include <pthread.h>

Thread::Thread(int lcore, void (*func)(void)) : lcore_(lcore) {
  t_ = std::thread(func);
  affinity(lcore_);
}

Thread::Thread(int lcore, void (*func)(void *a), void *arg) : lcore_(lcore) {
  t_ = std::thread(func, arg);
  affinity(lcore);
}

void Thread::affinity(int __attribute__((unused)) lcore) {
#ifdef __linux__
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(lcore, &cpuset);

  int s =
      pthread_setaffinity_np(t_.native_handle(), sizeof(cpu_set_t), &cpuset);
  if (s != 0) {
    std::cout << "thread affinity error" << std::endl;
    exit(1);
  }
#else
#pragma message("No thread affinity implemented on Mac OS X")
#endif
}
