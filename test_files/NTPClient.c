/*
 *
 * (C) 2014 David Lettier.
 *
 * http://www.lettier.com/
 *
 * NTP client.
 *
 * Compiled with gcc version 4.7.2 20121109 (Red Hat 4.7.2-8) (GCC).
 *
 * Tested on Linux 3.8.11-200.fc18.x86_64 #1 SMP Wed May 1 19:44:27 UTC 2013 x86_64 x86_64 x86_64 GNU/Linux.
 *
 * To compile: $ gcc NTPClient.c -o ntpClient.out
 *
 * Usage: $ ./ntpClient.out
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull

#define LI(packet)   (uint8_t) ((packet.li_vn_mode & 0xC0) >> 6) // (li   & 11 000 000) >> 6
#define VN(packet)   (uint8_t) ((packet.li_vn_mode & 0x38) >> 3) // (vn   & 00 111 000) >> 3
#define MODE(packet) (uint8_t) ((packet.li_vn_mode & 0x07) >> 0) // (mode & 00 000 111) >> 0


void error( char* msg )
{
    perror( msg ); // Print the error message to stderr.

    exit( 0 ); // Quit the process.
}

// Structure that defines the 48 byte NTP packet protocol.

typedef struct 
  {

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

  }ntp_packet;              // Total: 384 bits or 48 bytes.


typedef struct
{
    int sockfd; // Socket file descriptor and the n return result from writing/reading from the socket.

    int portno;//=123;// NTP UDP port number.

    char host_name[20];//="us.pool.ntp.org"; // NTP server host-name.

    ntp_packet packet;// = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    struct sockaddr_in serv_addr; // Server address data structure.
    struct hostent *server;      // Server data structure.

} NTPClient;

ntp_packet zero_packet= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


void NTPClient_init(NTPClient *client)
{
    client->portno=123;
    strcpy(client->host_name,"in.pool.ntp.org");
    // Create and zero out the packet. All 48 bytes worth.
    
    client->packet =zero_packet;
    memset( &client->packet, 0, sizeof( ntp_packet ) );
     // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
    *( ( char * ) &client->packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

    // Create a UDP socket, convert the host-name to an IP address, set the port number,
    // connect to the server, send the packet, and then read in the return packet.
    client->sockfd = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP ); // Create a UDP socket.

    if ( client->sockfd < 0 )
        error( "ERROR opening socket" );

    client->server = gethostbyname( client->host_name ); // Convert URL to IP.

    if ( client->server == NULL )
        error( "ERROR, no such host" );

    // Zero out the server address structure.

    bzero( ( char* ) &client->serv_addr, sizeof( client->serv_addr ) );

    client->serv_addr.sin_family = AF_INET;

    // Copy the server's IP address to the server address structure.

    bcopy( ( char* )client->server->h_addr, ( char* ) &client->serv_addr.sin_addr.s_addr, client->server->h_length );

    // Convert the port number integer to network big-endian style and save it to the server address structure.

    client->serv_addr.sin_port = htons( client->portno );

    // Call up the server using its IP address and port number.

    if ( connect( client->sockfd, ( struct sockaddr * ) &client->serv_addr, sizeof( client->serv_addr) ) < 0 )
        error( "ERROR connecting" );

    // Send it the NTP packet it wants. If n == -1, it failed.
}
double get_time_now(NTPClient *client)
{
    client->packet=zero_packet;
    memset( &client->packet, 0, sizeof( ntp_packet ) );
     // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
    *( ( char * ) &client->packet + 0 ) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.
    int n = write( client->sockfd, ( char* ) &client->packet, sizeof( ntp_packet ) );

    if ( n < 0 )
        error( "ERROR writing to socket" );

    // Wait and receive the packet back from the server. If n == -1, it failed.
    n = read( client->sockfd, ( char* ) &client->packet, sizeof( ntp_packet ) );

    if ( n < 0 )
        error( "ERROR reading from socket" );

    // These two fields contain the time-stamp seconds as the packet left the NTP server.
    // The number of seconds correspond to the seconds passed since 1900.
    // ntohl() converts the bit/byte order from the network's to host's "endianness".

    client->packet.txTm_s = ntohl( client->packet.txTm_s ); // Time-stamp seconds.
    client->packet.txTm_f = ntohl( client->packet.txTm_f ); // Time-stamp fraction of a second.

    // Extract the 32 bits that represent the time-stamp seconds (since NTP epoch) from when the packet left the server.
    // Subtract 70 years worth of seconds from the seconds since 1900.
    // This leaves the seconds since the UNIX epoch of 1970.
    // (1900)------------------(1970)**************************************(Time Packet Left the Server)

    uint32_t txTm = ( client->packet.txTm_s - NTP_TIMESTAMP_DELTA );
    double millisecond =(double) txTm * 1000;
    millisecond +=((double) client->packet.txTm_f/ 4294967295) * 1000.0;
    return millisecond;
}


int main( int argc, char* argv[ ] )
{
    NTPClient *client=malloc(sizeof(NTPClient));
    NTPClient_init(client);
    double txTm=get_time_now(client);
    // Print the time we got from the server, accounting for local timezone and conversion from UTC time.
    
    usleep(1200000);
    double txTm2=get_time_now(client);
    // Print the time we got from the server, accounting for local timezone and conversion from UTC time.
    printf("%f\n",txTm2-txTm);
    
    return 0;
}
