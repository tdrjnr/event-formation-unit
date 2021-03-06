//
// Copied from NMXArgs.cpp
//

#include <cinttypes>
#include <getopt.h>
#include <stdlib.h>

#include <mbgen/MBArgs.h>

MBArgs::MBArgs(int argc, char *argv[]) {

  int c;

  while (1) {
    static struct option long_options[] = {
        {"filename", required_argument, 0, 'f'},
        {"outfile", required_argument, 0, 'o'},
        {"ipaddr", required_argument, 0, 'i'},
        {"data", required_argument, 0, 'd'},
        {"packets", required_argument, 0, 'a'},
        {"datapoints", required_argument, 0, 'n'},
        {"port", required_argument, 0, 'p'},
        {"size", required_argument, 0, 's'},
        {"update", required_argument, 0, 'u'},
        {"sndbuf", required_argument, 0, 'x'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}};

    int option_index = 0;

    c = getopt_long(argc, argv, "a:d:f:o:i:n:p:s:t:u:hx", long_options,
                    &option_index);

    if (c == -1)
      break;
    switch (c) {
    case 0:
      if (long_options[option_index].flag != 0)
        break;
    case 'a':
      txPkt = atoi(optarg);
      break;
    case 'd':
      buflen = atoi(optarg);
      break;
    case 'f':
      filename.assign(optarg);
      break;
    case 'o':
      outfile.assign(optarg);
      break;
    case 'i':
      dest_ip.assign(optarg);
      break;
    case 'n':
      dppkg = atoi(optarg);
      break;
    case 'p':
      port = atoi(optarg);
      break;
    case 's':
      txGB = atoi(optarg);
      break;
    case 'u':
      updint = atoi(optarg);
      break;
    case 'x':
      sndbuf = atoi(optarg);
      break;
    case 'h':
    default:
      printf("Usage: bulkdatagen [OPTIONS] \n");
      printf(" --filename -f name     read data from single file \n");
      printf(" --outfile -o name      write image to file \n");
      printf(" --size -s size         size in GB of transmitted data \n");
      printf(" --packets -a number    number of packets to transmit \n");
      printf(" --ipaddr -i ipaddr     destination ip address \n");
      printf(" --port -p port         UDP destination port \n");
      printf(" --data -d len          size of Tx/Tx buffer in bytes (max 9000) "
             "\n");
      printf(" --datapoints -n        number of datapoints per package \n");
      printf(" --update -u interval   update interval (seconds) \n");
      printf(" --sndbuf -x len        kernel tx buffer size \n");
      printf(" -h                     help - prints this message \n");
      exit(1);
    }
  }
  printf("Generating a bulk data stream\n");
  if (!filename.empty())
    printf("  from file:              %s", filename.c_str());
  if (!outfile.empty())
    printf("  to file:                %s", outfile.c_str());
  printf("  number of bytes:        %u GB\n", txGB);
  printf("  number of packets:      %" PRIu64 " packets\n", txPkt);
  printf("Network properties\n");
  printf("  destination ip address: %s\n", dest_ip.c_str());
  printf("  destination udp port:   %d\n", port);
  printf("  tx buffer size:         %dB\n", buflen);
  printf("  sndbuf:                 %uB\n", sndbuf);
  printf("Other properties\n");
  printf("  update interval:        %us\n", updint);
}
