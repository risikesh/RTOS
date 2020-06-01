#ifndef _msg_H_
#define _msg_H_
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#define gname_size 10
#define uname_size 10
#define msg_size 100

enum MSG_STATUS{CLOSE_ME,MESSAGE};

enum Mode{DEV,PROD};

//--------------------------------- Group Chat Request ------------------------------------------//

struct group_chat_request
{
    char group_name[gname_size];
    char user_name[uname_size];    
};


//--------------------------------- Group Chat Acknowledge---------------------------------------//

struct group_chat_acknowledge
{
    int group_identifier;
    int user_identifier;    
};

//--------------------------------- Group Message -----------------------------------------------//
struct group_message
{
    enum MSG_STATUS status;
    int group_identifier;
    int user_identifier;
    char message[msg_size];
    char user_name[uname_size];
    struct timeval time_stamp;
    int msgid;//issued by server
};
//----------------------------------Message Queue-----------------------------------------------//
struct message_queue
{
    struct group_message message_q_ll;
    struct message_queue *next;
};

struct timeval GetTimeStamp() {
    /*
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv;
    */
   struct timeval timestamp;
    gettimeofday(&timestamp, NULL);
    long seconds = timestamp.tv_sec;
    long microseconds = timestamp.tv_usec;
    //seconds += secondOffset;
    //microseconds += usecondOffset;
    while(microseconds < 0) {
        seconds--;
        microseconds += 1000 * 1000;
    }
    while(microseconds >= 1000 * 1000) {
        seconds++;
        microseconds -= 1000 * 1000;
    }
    timestamp.tv_sec = seconds;
    timestamp.tv_usec = microseconds;
    return timestamp;

}


#endif
