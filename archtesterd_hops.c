
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <errno.h>

//
// Constants
//

#define IP4_HDRLEN		20
#define ICMP4_HDRLEN		 8

//
// Variables
//

static int debug = 0;

//
// Finding out an interface index for a named interface
//

static void
archtesterd_getifindex(const char* interface,
		       int* ifIndex,
		       struct ifreq* ifrp,
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
  *ifIndex = ifr.ifr_ifindex;
  *ifrp = ifr;
  
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
  
  if ((rcode = getaddrinfo(destination, NULL, &hints, &res)) != 0) {
    fprintf (stderr, "archtesterd_hops: cannot resolve address %s: %s\n", destination, gai_strerror (rcode));
    exit(1);
  }
  *address = *(struct sockaddr_in*)res->ai_addr;
}

//
// Mapping addresses to strings (n.n.n.n or h:h:...:h)
//

const char*
archtesterd_iptostring(struct sockaddr_in* in) {
  char* result = (char*)malloc(INET_ADDRSTRLEN+1);
  memset(result,0,INET_ADDRSTRLEN+1);
  if (inet_ntop (AF_INET, (void*)&in->sin_addr, result, INET_ADDRSTRLEN) == NULL) {
    fprintf(stderr, "archtesterd_hops: inet_ntop() failed -- exit\n");
    exit(1);
  }
}

//
// Checksum per RFC 1071
//

uint16_t
archtesterd_checksum(uint16_t* data,
		     int length)
{
  register uint32_t sum = 0;
  int count = length;
  
  while (count > 1) {
    sum += *(data++);
    count -= 2;
  }
  
  if (count > 0) {
    sum += *(uint8_t*)data;
  }
  
  while (sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  
  return(~sum);
}

//
// Construct an ICMPv4 packet
//

static void
archtesterd_constructicmp4packet(struct sockaddr_in* source,
				 struct sockaddr_in* destination,
				 unsigned char ttl,
				 char** resultPacket,
				 unsigned int* resultPacketLength)  {

  static const char* message = "archtester";
  static char data[IP_MAXPACKET];
  static char packet[IP_MAXPACKET];
  struct icmp icmphdr;
  struct ip iphdr;
  unsigned int dataLength;
  unsigned int icmpLength;
  unsigned int packetLength;
  
  //
  // Fill in ICMP parts
  //
  
  icmphdr.icmp_type = ICMP_ECHO;
  icmphdr.icmp_code = 0;
  icmphdr.icmp_id = 0;
  icmphdr.icmp_seq = 0;
  icmphdr.icmp_cksum = 0;
  dataLength = strlen(message);
  strcpy(data,message);
  icmpLength = ICMP4_HDRLEN + dataLength;
  memcpy(packet + IP4_HDRLEN,&icmphdr,ICMP4_HDRLEN);
  memcpy(packet + IP4_HDRLEN + ICMP4_HDRLEN,data,dataLength);
  icmphdr.icmp_cksum = archtesterd_checksum((uint16_t*)(packet + IP4_HDRLEN), icmpLength);
  memcpy(packet + IP4_HDRLEN,&icmphdr,ICMP4_HDRLEN);
  
  //
  // Fill in the IPv4 header
  //
  
  iphdr.ip_hl = 5;
  iphdr.ip_v = 4;
  iphdr.ip_tos = 0;
  packetLength = IP4_HDRLEN + icmpLength;
  iphdr.ip_len = htons (packetLength);
  iphdr.ip_id = 0;
  iphdr.ip_off = 0;
  iphdr.ip_ttl = ttl;
  iphdr.ip_p = IPPROTO_ICMP;
  iphdr.ip_src = source->sin_addr;
  iphdr.ip_dst = destination->sin_addr;
  iphdr.ip_sum = 0;
  iphdr.ip_sum = archtesterd_checksum((uint16_t*)&iphdr,IP4_HDRLEN);
  memcpy (packet, &iphdr, IP4_HDRLEN);

  //
  // Debugs
  //

  if (debug) printf("archtesterd_hops: debug: constructed a packet of %u bytes, ttl = %u\n", packetLength, ttl);
  
  //
  // Return the packet
  //
  
  *resultPacket = packet;
  *resultPacketLength = packetLength;
}

//
// The main program for running a test
//

static void
archtesterd_runtest(unsigned int startTtl,
		    const char* interface,
		    const char* destination) {

  struct sockaddr_in destinationAddress;
  struct sockaddr_in sourceAddress;
  unsigned int packetLength;
  struct ifreq ifr;
  int hdrison = 1;
  int ttl = startTtl;
  char* packet;
  int ifindex;
  int sd;
  
  //
  // Find out ifindex, own address, destination address
  //
  
  archtesterd_getifindex(interface,&ifindex,&ifr,&sourceAddress);
  archtesterd_getdestinationaddress(destination,&destinationAddress);

  //
  // Debugs
  //
  
  if (debug) printf("archtesterd_hops: debug: ifindex = %d\n", ifindex);
  if (debug) printf("archtesterd_hops: debug: source = %s\n", archtesterd_iptostring(&sourceAddress));
  if (debug) printf("archtesterd_hops: debug: destination = %s\n", archtesterd_iptostring(&destinationAddress));
    
  //
  // Get a raw socket
  //
  
  if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
    perror ("archtesterd_hops: socket() failed to get socket descriptor for using ioctl() ");
    exit(1);
  }
  
  if (setsockopt (sd, IPPROTO_IP, IP_HDRINCL, &hdrison, sizeof (hdrison)) < 0) {
    perror ("archtesterd_hops: setsockopt() failed to set IP_HDRINCL ");
    exit(1);
  }
  
  if (setsockopt (sd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof (ifr)) < 0) {
    perror ("archtesterd_hops: setsockopt() failed to bind to interface ");
    exit(1);
  }

  //
  // Create a packet
  //

  archtesterd_constructicmp4packet(&sourceAddress,
				   &destinationAddress,
				   ttl,
				   &packet,
				   &packetLength);

  //
  // Send the packet
  //
  
  if (sendto (sd, packet, packetLength, 0, (struct sockaddr *) &destinationAddress, sizeof (struct sockaddr)) < 0)  {
    perror ("archtesterd_hops: sendto() failed ");
    exit(1);
  }
  
  //
  // Done. Return.
  //
}


int
main(int argc,
     char** argv) {

  const char* testDestination = "www.google.com";
  const char* interface = "eth0";
  unsigned int startTtl = 10;
  
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
    } else if (strcmp(argv[0],"-t") == 0 && argc > 1) {
      startTtl = atoi(argv[1]);
      if (startTtl <= 0) {
	fprintf(stderr,"archtesterd_hops: invalid TTL value -- exit\n");
	exit(1);
      }
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

  archtesterd_runtest(startTtl,
		      interface,
		      testDestination);
  exit(0);
}
