
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

static void
archtesterd_getifindex(const char* interface,
		       int* ifIndex,
		       struct sockaddr_in *addr) {
  
  struct ifreq ifr;
  int sd;
  
  //
  // Get a raw socket
  //
  
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("archtesterd_hops: socket() failed to get socket descriptor for using ioctl() ");
    exit(1);
  }
  
  //
  // Find interface index
  //
  
  memset (&ifr,0,sizeof (ifr));
  strncpy(ifr.ifr_name, interface, sizeof (ifr.ifr_name));
  if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
    perror ("archtesterd_hops: ioctl() failed to find interface ");
    exit(1);
  }
  *ifindex = ifr.ifr_ifindex;
  
  //
  // Find own source address
  //
  
  memset (&ifr,0,sizeof (ifr));
  strncpy(ifr.ifr_name, interface, sizeof (ifr.ifr_name));
  if (ioctl(sd,SIOCGIFADDR,&ifr)==-1) {
    perror ("archtesterd_hops: ioctl() failed to find interface address ");
    exit(1);
  }
  *addr = *(struct sockaddr_in*)&ifr.ifr_addr;
  
  //
  // Cleanup and return
  //
  
  close (sd);
}

static void
archtesterd_getdestinationaddress(const char* destination,
				  struct sockaddr_in* address) {
  
  struct addrinfo hints, *res;
  struct sockaddr_in *addr;
  int rcode;
  
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = hints.ai_flags | AI_CANONNAME;
  
  if ((rcode = getaddrinfo (target, NULL, &hints, &res)) != 0) {
    fprintf (stderr, "archtesterd_hops: cannot resolve address %s: %s\n", destination, gai_strerror (rcode));
    exit(1);
  }
  *address = *(struct sockaddr_in*)res->ai_addr;
}

//
// The main program for running a test
//

static void
archtesterd_runtest(const char* interface,
		    const char* destination) {

  struct sockaddr_in sourceAddress;
  struct sockaddr_in destinationAddress;
  int ifindex;
  int sd;

  //
  // Find out ifindex, own address, destination address
  //
  
  archtesterd_getifindex(interface,&ifindex,&sourceAddress);
  archtesterd_getdestinationaddress(destination,&destinationAddress);
  
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
    } else if (strcmp(argv[0],"-i") == 0 && argc > 1) {
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
