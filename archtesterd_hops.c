
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>

static int debug = 0;

static int
archtesterd_getifindex(const char* interface) {
  
  struct ifreq ifr;
  int sd;
  
  //
  // Get a raw socket
  //
  
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() failed to get socket descriptor for using ioctl() ");
    exit(1);
  }
  
  //
  // Find interface index
  //
  
  memset (&ifr, 0, sizeof (ifr));
  snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
  if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
    perror ("ioctl() failed to find interface ");
    exit(1);
  }
  close (sd);
  
  return(ifr.ifr_ifindex);
}

//
// The main program for running a test
//

static void
archtesterd_runtest(const char* interface,
		    const char* destination) {
  
  int ifindex = archtesterd_getifindex(interface);
  int sd;
  
  //
  // Get a raw socket
  //
  
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("socket() failed to get socket descriptor for using ioctl() ");
    exit(1);
  }
  
}


int
main(int argc,
     char** argv) {

  const char* testDestination = "www.google.com";
  const char* interface = "eth0";
  
  argc--; argv++;
  while (argc > 0) {
    if (strcmp(argv[0],"-v") == 0) {
      printf("version 0.2\n");
      exit(0);
    } else if (strcmp(argv[0],"-d") == 0) {
      debug = 1;
    } else if (strcmp(argv[0],"-i" && argc > 1) == 0) {
      interface = argv[1];
      argc--; argv++;
    } else if (argv[0][0] == '-') {
      fprintf(stderr,"archtesterd_hops: unrecognised option -- exit\n");
      exit(1);
    } else if (argc > 1) {
      fprintf(stderr,"archtesterd_hops: too many arguments -- exit\n");
      exit(1);
    } else {
      testDestination = argv[0];
    }
    argc--; argv++;
  }

  runtest(testDestination);
  exit(0);
}
