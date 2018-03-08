
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <errno.h>

//
// Protocol constants
//

#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_CLIENT_HELLO		  1
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_SERVER_HELLO		  2
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_NEW_SESSION_TICKET		  4
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_END_OF_EARLY_DATA		  5
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_ENCRYPTED_EXTENSIONS	  8
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_CERTIFICATE		 11
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_CERTIFICATE_REQUEST	 13
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_CERTIFICATE_VERIFY		 15
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_FINISHED			 20
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_KEY_UPDATE			 24
#define ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_MESSAGE_HASH		254

#define ARCHTESTERD_TLSVER_TLS_HELLO_LEGACY_VERSION		     0x0303
#define ARCHTESTERD_TLSVER_TLS_HELLO_RANDOM_SIZE		         32
#define ARCHTESTERD_TLSVER_TLS_HELLO_SESSION_ID_SIZE_MAX	         32
#define ARCHTESTERD_TLSVER_TLS_HELLO_CIPHERSUITE		          0 /* ... */
#define ARCHTESTERD_TLSVER_TLS_HELLO_COMPRESSION_NULL			  0
#define ARCHTESTERD_TLSVER_TLS_HELLO_TLS_VERSION_13		     0x0304

#define ARCHTESTERD_TLSVER_EXTENSION_SERVER_NAME				0
#define ARCHTESTERD_TLSVER_EXTENSION_MAX_FRAGMENT_LENGTH			1
#define ARCHTESTERD_TLSVER_EXTENSION_STATUS_REQUEST				5
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_GROUPS				10
#define ARCHTESTERD_TLSVER_EXTENSION_SIGNATURE_ALGORITHMS			13
#define ARCHTESTERD_TLSVER_EXTENSION_USE_SRTP					14
#define ARCHTESTERD_TLSVER_EXTENSION_HEARTBEAT					15
#define ARCHTESTERD_TLSVER_EXTENSION_APPLICATION_LAYER_PROTOCOL_NEGOTIATION	16
#define ARCHTESTERD_TLSVER_EXTENSION_SIGNED_CERTIFICATE_TIMESTAMP		18
#define ARCHTESTERD_TLSVER_EXTENSION_CLIENT_CERTIFICATE_TYPE			19
#define ARCHTESTERD_TLSVER_EXTENSION_SERVER_CERTIFICATE_TYPE			20
#define ARCHTESTERD_TLSVER_EXTENSION_PADDING					21
#define ARCHTESTERD_TLSVER_EXTENSION_PRE_SHARED_KEY				41
#define ARCHTESTERD_TLSVER_EXTENSION_EARLY_DATA					42
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS				43
#define ARCHTESTERD_TLSVER_EXTENSION_COOKIE					44
#define ARCHTESTERD_TLSVER_EXTENSION_PSK_KEY_EXCHANGE_MODES			45
#define ARCHTESTERD_TLSVER_EXTENSION_CERTIFICATE_AUTHORITIES			47
#define ARCHTESTERD_TLSVER_EXTENSION_OID_FILTERS				48
#define ARCHTESTERD_TLSVER_EXTENSION_POST_HANDSHAKE_AUTH			49
#define ARCHTESTERD_TLSVER_EXTENSION_SIGNATURE_ALGORITHMS_CERT			50
#define ARCHTESTERD_TLSVER_EXTENSION_KEY_SHARE					51

#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_N			(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET1_COUNT + \
										 ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET2_COUNT + \
										 ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET3_COUNT)
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET1_BASE		0x0303
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET1_COUNT		22
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET2_BASE		0x0401
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET2_COUNT		4
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET3_BASE		0x0501
#define ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET3_COUNT		4

#define ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_GCM_SHA256			0x1301
#define ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_256_GCM_SHA384       		0x1302
#define ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_CHACHA20_POLY1305_SHA256 		0x1303
#define ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_CCM_SHA256       		0x1304
#define ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_CCM_8_SHA256     		0x1305

//
// Capacity, size, etc. definitions
//

#define ARCHTESTERD_TLSVER_MAXMSGSIZE	1000

//
// Some helper macros ----------------------------------------------------
//

#define archtesterd_assert(cond)	if (!(cond)) {                             \
	                                  fatalf("Assertion failed on %s line %u", \
        	                                 __FILE__, __LINE__);		   \
                	                }

//
// Configuration parameters and their defaults -----------------
//

const char* testDestination = "www.google.com";
unsigned int port = 443;
int debug = 0;

//
// Other Variables --------------------------------------------------------
//

static int interrupt = 0;
static struct timeval startTime;

//
// Prototype definitions of functions ------------------------------------
//

static void
fatalf(const char* format, ...);

//
// Functions -------------------------------------------------------------
//

//
// Debug helper function
//

static void
debugf(const char* format, ...) {

  archtesterd_assert(format != 0);
  
  if (debug) {

    va_list args;

    printf("archtesterd_tlsver: debug: ");
    va_start (args, format);
    vprintf(format, args);
    va_end (args);
    printf("\n");
    
  }
  
}

//
// Display a warning
//

static void
warnf(const char* format, ...) {
  
  va_list args;
  
  archtesterd_assert(format != 0);
  
  fprintf(stderr,"archtesterd_tlsver: warning: ");
  va_start (args, format);
  vfprintf(stderr, format, args);
  va_end (args);
  fprintf(stderr,"\n");
}

//
// Display a fatal error
//

static void
fatalf(const char* format, ...) {
  
  va_list args;
  
  archtesterd_assert(format != 0);
  
  fprintf(stderr,"archtesterd_tlsver: error: ");
  va_start (args, format);
  vfprintf(stderr, format, args);
  va_end (args);
  fprintf(stderr," -- exit\n");
  
  exit(1);
}

//
// Display a fatal error a la perror
//

static void
fatalp(const char* message) {
  
  const char* string = strerror(errno);
  archtesterd_assert(message != 0);
  fatalf("system: %s", string);
  
}

//
// Interrupts (Ctrl-C) during program execution should cause the
// current process to end and results printed out.
//

static void
archtesterd_tlsver_interrupt(int dummy) {
  interrupt = 1;
}

//
// Add a byte to the buffer
//

static void
archtesterd_addtobuffer_u8(unsigned char byte,
			   unsigned char* buffer,
			   unsigned int bufferLength,
			   unsigned int* messageLength) {
  
  if (*messageLength == bufferLength) fatalf("out of buffer space");
  buffer[*messageLength++] = byte;
  
}

//
// Add an uint16 to the buffer
//

static void
archtesterd_addtobuffer_u16(unsigned short value,
			    unsigned char* buffer,
			    unsigned int bufferLength,
			    unsigned int* messageLength) {
  
  if (*messageLength >= bufferLength - 2) fatalf("out of buffer space");
  buffer[*messageLength++] = (unsigned char)((value >> 8) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 0) & 0xFF);
  
}

//
// Add an uint24 to the buffer
//

static void
archtesterd_addtobuffer_u24(unsigned int value,
			    unsigned char* buffer,
			    unsigned int bufferLength,
			    unsigned int* messageLength) {
  
  if (*messageLength >= bufferLength - 3) fatalf("out of buffer space");
  buffer[*messageLength++] = (unsigned char)((value >> 16) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 8) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 0) & 0xFF);
  
}

//
// Add an uint32 to the buffer
//

static void
archtesterd_addtobuffer_u32(unsigned int value,
			    unsigned char* buffer,
			    unsigned int bufferLength,
			    unsigned int* messageLength) {
  
  if (*messageLength >= bufferLength - 3) fatalf("out of buffer space");
  buffer[*messageLength++] = (unsigned char)((value >> 24) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 16) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 8) & 0xFF);
  buffer[*messageLength++] = (unsigned char)((value >> 0) & 0xFF);
  
}

//
// Debugging display of messages
//

static void
archtesterd_showbytes(const char* what,
		      const unsigned char* message,
		      unsigned int length) {
  unsigned int i;
  printf("%s: ", what);
  for (i = 0; i < length; i++) {
    printf("%02x", (unsigned int)message[i]);
  }
  printf("\n");
}

//
// Add a session id to the message
//

static void
archtesterd_tlsver_addsessionid(unsigned char* buffer,
				unsigned int bufferLength,
				unsigned int* messageLength) {
  archtesterd_addtobuffer_u8(ARCHTESTERD_TLSVER_TLS_HELLO_SESSION_ID_SIZE_MAX,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(0,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u32(1,buffer,bufferLength,messageLength);
}

//
// Add cipher suites to the message
//

static void
archtesterd_tlsver_addciphersuites(unsigned char* buffer,
				   unsigned int bufferLength,
				   unsigned int* messageLength) {

  archtesterd_addtobuffer_u16(2*5,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_GCM_SHA256,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_256_GCM_SHA384,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_CHACHA20_POLY1305_SHA256,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_CCM_SHA256,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_CIPHERSUITE_TLS_AES_128_CCM_8_SHA256,buffer,bufferLength,messageLength);
  
}

//
// Add compression methods to the message
//

static void
archtesterd_tlsver_addcompressionmethods(unsigned char* buffer,
					 unsigned int bufferLength,
					 unsigned int* messageLength) {
  archtesterd_addtobuffer_u8(1,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u8(ARCHTESTERD_TLSVER_TLS_HELLO_COMPRESSION_NULL,buffer,bufferLength,messageLength);
}

//
// Add supported version extension to the message
//

static void
archtesterd_tlsver_addsupportedversionsextension(unsigned char* buffer,
						 unsigned int bufferLength,
						 unsigned int* messageLength) {

  unsigned int i;
  
  //
  //  struct {
  //     ExtensionType extension_type;
  //     opaque extension_data<0..2^16-1>;
  // } Extension;
  //
  //  enum {
  //     server_name(0),                             /* RFC 6066 */
  //     max_fragment_length(1),                     /* RFC 6066 */
  //     status_request(5),                          /* RFC 6066 */
  //     supported_groups(10),                       /* RFC 4492, 7919 */
  //     signature_algorithms(13),                   /* [[this document]] */
  //     use_srtp(14),                               /* RFC 5764 */
  //     heartbeat(15),                              /* RFC 6520 */
  //     application_layer_protocol_negotiation(16), /* RFC 7301 */
  //     signed_certificate_timestamp(18),           /* RFC 6962 */
  //     client_certificate_type(19),                /* RFC 7250 */
  //     server_certificate_type(20),                /* RFC 7250 */
  //     padding(21),                                /* RFC 7685 */
  //     pre_shared_key(41),                         /* [[this document]] */
  //     early_data(42),                             /* [[this document]] */
  //     supported_versions(43),                     /* [[this document]] */
  //     cookie(44),                                 /* [[this document]] */
  //     psk_key_exchange_modes(45),                 /* [[this document]] */
  //     certificate_authorities(47),                /* [[this document]] */
  //     oid_filters(48),                            /* [[this document]] */
  //     post_handshake_auth(49),                    /* [[this document]] */
  //     signature_algorithms_cert(50),              /* [[this document]] */
  //     key_share(51),                              /* [[this document]] */
  //     (65535)
  // } ExtensionType;
  //
  //
  //  struct {
  //      select (Handshake.msg_type) {
  //          case client_hello:
  //               ProtocolVersion versions<2..254>;
  //
  //           case server_hello: /* and HelloRetryRequest */
  //               ProtocolVersion selected_version;
  //      };
  //  } SupportedVersions;
  //
  
  archtesterd_addtobuffer_u8(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS,buffer,bufferLength,messageLength);
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_N * 2,buffer,bufferLength,messageLength);
  
  //
  // Set 1
  //
  
  for (i = 0; i < ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET1_COUNT; i++) {
    archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET1_BASE + i,buffer,bufferLength,messageLength);
  }
  
  //
  // Set 2
  //
  
  for (i = 0; i < ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET2_COUNT; i++) {
    archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET2_BASE + i,buffer,bufferLength,messageLength);
  }
  
  //
  // Set 3
  //
  
  for (i = 0; i < ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET3_COUNT; i++) {
    archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_EXTENSION_SUPPORTED_VERSIONS_SET3_BASE + i,buffer,bufferLength,messageLength);
  }
  
}

//
// Add extensions to the message
//

static void
archtesterd_tlsver_addextensions(unsigned char* buffer,
				 unsigned int bufferLength,
				 unsigned int* messageLength) {
  archtesterd_tlsver_addsupportedversionsextension(buffer,bufferLength,messageLength);
}

//
// Construct a client hello message
//

static void
archtesterd_tlsver_makeclienthello(unsigned char* buffer,
				   unsigned int bufferLength,
				   unsigned int* messageLength) {

  unsigned int i;
  
  //
  //      enum {
  //          client_hello(1),
  //          server_hello(2),
  //          new_session_ticket(4),
  //          end_of_early_data(5),
  //          encrypted_extensions(8),
  //          certificate(11),
  //          certificate_request(13),
  //          certificate_verify(15),
  //          finished(20),
  //          key_update(24),
  //          message_hash(254),
  //          (255)
  //      } HandshakeType;
  //
  //      struct {
  //          HandshakeType msg_type;    /* handshake type */
  //          uint24 length;             /* bytes in message */
  //          select (Handshake.msg_type) {
  //              case client_hello:          ClientHello;
  //              case server_hello:          ServerHello;
  //              case end_of_early_data:     EndOfEarlyData;
  //              case encrypted_extensions:  EncryptedExtensions;
  //              case certificate_request:   CertificateRequest;
  //              case certificate:           Certificate;
  //              case certificate_verify:    CertificateVerify;
  //              case finished:              Finished;
  //              case new_session_ticket:    NewSessionTicket;
  //              case key_update:            KeyUpdate;
  //          };
  //      } Handshake;
  //
  //
  //
  //   Structure of this message:
  //  
  //      uint16 ProtocolVersion;
  //      opaque Random[32];
  //
  //      uint8 CipherSuite[2];    /* Cryptographic suite selector */
  //
  //      struct {
  //          ProtocolVersion legacy_version = 0x0303;    /* TLS v1.2 */
  //          Random random;
  //          opaque legacy_session_id<0..32>;
  //          CipherSuite cipher_suites<2..2^16-2>;
  //          opaque legacy_compression_methods<1..2^8-1>;
  //          Extension extensions<8..2^16-1>;
  //      } ClientHello;
  //
  
  *messageLength = 0;

  //
  // msg_type
  //
  
  archtesterd_addtobuffer_u8(ARCHTESTERD_TLSVER_TLS_HANDSHAKETYPE_CLIENT_HELLO,buffer,bufferLength,messageLength);

  //
  // length
  //
  
  archtesterd_addtobuffer_u24(0,buffer,bufferLength,messageLength);

  //
  // legacy_version
  //
  
  archtesterd_addtobuffer_u16(ARCHTESTERD_TLSVER_TLS_HELLO_LEGACY_VERSION,buffer,bufferLength,messageLength);

  //
  // random
  //
  
  for (i = 0; i < ARCHTESTERD_TLSVER_TLS_HELLO_RANDOM_SIZE; i++) {
    archtesterd_addtobuffer_u8(rand() & 0xFF,buffer,bufferLength,messageLength);
  }

  //
  // legacy_session_id
  //
  
  archtesterd_tlsver_addsessionid(buffer,bufferLength,messageLength);

  //
  // cipher_suites
  //
  
  archtesterd_tlsver_addciphersuites(buffer,bufferLength,messageLength);

  //
  // legacy_compression_methods
  //
  
  archtesterd_tlsver_addcompressionmethods(buffer,bufferLength,messageLength);
  
  //
  // extensions
  //
  
  archtesterd_tlsver_addextensions(buffer,bufferLength,messageLength);
}

//
// Get current time
//

static void
archtesterd_tlsver_getcurrenttime(struct timeval* result) {
  
  archtesterd_assert(result != 0);
  if (gettimeofday(result, 0) < 0) {
    fatalp("cannot determine current time via gettimeofday");
  }
  
}

//
// Map a DNS name to an address
//

static void
archtesterd_getdestinationaddress(const char* destination,
				  struct sockaddr_in* address) {
  
  struct addrinfo hints, *res;
  struct sockaddr_in *addr;
  int rcode;
  
  archtesterd_assert(destination != 0);
  archtesterd_assert(address != 0);
  
  memset(&hints,0,sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = hints.ai_flags | AI_CANONNAME;
  
  if ((rcode = getaddrinfo(destination, NULL, &hints, &res)) != 0) {
    fatalf("cannot resolve address %s: %s", destination, gai_strerror (rcode));
    exit(1);
  }
  
  *address = *(struct sockaddr_in*)res->ai_addr;
}

//
// Run the actual probing
//

static const char*
archtesterd_tlsver_runtest(const char* destination) {
  
  int sock;
  struct sockaddr_in server;
  unsigned char sentMessage[ARCHTESTERD_TLSVER_MAXMSGSIZE];
  unsigned int sentMessageSize;
  unsigned char receivedMessage[ARCHTESTERD_TLSVER_MAXMSGSIZE];
  int receivedMessageSize;
  
  //
  // Create socket
  //
  
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1) {
    fatalf("could not create socket");
  }

  archtesterd_getdestinationaddress(destination,&server);
  server.sin_port = htons(port);
  
  //
  // Connect to remote server
  //
  
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0) {
    fatalf("connect failed");
  }
  
  //
  // Communicate with the server
  //
  
  archtesterd_showbytes("sending",sentMessage,sentMessageSize);
  if (send(sock,
	   (const char*)sentMessage,
	   sentMessageSize,
	   0) < 0) {
    fatalf("send failed");
  }

  //
  // Receive a reply from the server
  if ((receivedMessageSize = recv(sock,
				  (void*)receivedMessage,
				  2000,
				  0)) < 0) {
    fatalf("recv failed");
  }
  
  archtesterd_showbytes("received",receivedMessage,receivedMessageSize);
     
  close(sock);

  //
  // Return without knowledge of what version this was
  //
  
  return(0);
}

//
// Report result
//

static void
archtesterd_tlsver_report(const char* result) {
  if (result == 0) printf("unknown\n");
  else printf("%s\n", result);
}

//
// Main program ------------------------------------------------
//

int
main(int argc,
     char** argv) {

  const char* result;
  
  //
  // Process arguments
  //
  
  argc--; argv++;
  while (argc > 0) {
    
    if (strcmp(argv[0],"-version") == 0) {
      
      printf("version 0.2\n");
      exit(0);
      
    } else if (strcmp(argv[0],"-debug") == 0) {
      
      debug = 1;
      
    } else if (strcmp(argv[0],"-no-debug") == 0) {
      
      debug = 0;
      
    } else if (strcmp(argv[0],"-port") == 0 && argc > 1 && isdigit(argv[1][0])) {

      port = atoi(argv[1]);
      argc--; argv++;
      
    } else if (argv[0][0] == '-') {
      
      fatalf("unrecognised option %s", argv[0]);
      
    } else if (argc > 1) {
      
      fatalf("too many arguments");
      
    } else {
      
      testDestination = argv[0];
      
    }
    
    argc--; argv++;
    
  }
  
  signal(SIGINT, archtesterd_tlsver_interrupt);
  
  archtesterd_tlsver_getcurrenttime(&startTime);
  
  result = archtesterd_tlsver_runtest(testDestination);
  archtesterd_tlsver_report(result);
  
  //
  // Done
  //
  
  exit(0);
  
}
