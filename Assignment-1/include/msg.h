#ifndef _msg_H_
#define _msg_H_
#include <stdlib.h>
#include <stdio.h>
#define gname_size 30
#define uname_size 30
#define msg_size 1000

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
    double time_stamp;
    int msgid;//issued by server
};
//----------------------------------Message Queue-----------------------------------------------//
struct message_queue
{
    struct group_message message_q_ll;
    struct message_queue *next;
};



#endif