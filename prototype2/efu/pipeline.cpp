/** Copyright (C) 2016 European Spallation Source ERIC*/

#include <common/EFUArgs.h>
#include <efu/Launcher.h>
#include <iostream>
#include <libs/include/Timer.h>
#include <unistd.h> // sleep()

using namespace std;

/**
 * Load detector, launch pipeline threads, then sleep forever
 */
int main(int argc, char *argv[]) {

  EFUArgs opts(argc, argv);

  cout << "Launching EFU as Instrument " << opts.det << endl;

  Loader dynamic(opts.det);

  Launcher(&dynamic, &opts, 12, 13, 14);

  Timer stop;
  while (stop.timeus() < opts.stopafter * 1000000LU) {
    sleep(2);
  }
  sleep(2);
  printf("Exiting...\n");
  exit(1);
  return 0;
}
