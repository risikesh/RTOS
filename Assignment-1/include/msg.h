#ifndef _msg_H_
#define _msg_H_

#define gname_size 30
#define uname_size 30
#define msg_size 1000

struct grpchat_request
{
    char group_name[gname_size];
    char user_name[uname_size];    
};

struct grpchat_indentifier
{
    int grp_identifier;
    int user_identifier;
    struct timeval server_time;
};

struct grp_message
{
    int grp_identifier;
    int user_identifier;
    char message[msg_size];
    char user_name[uname_size];
    struct timeval time;
};

#endif