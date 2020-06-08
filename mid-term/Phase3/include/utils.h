// parts of this code is taken from PulseAudio API example ( https://freedesktop.org/software/pulseaudio/doxygen/examples.html )
/***
  This file is part of PulseAudio.
  PulseAudio is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published
  by the Free Software Foundation; either version 2.1 of the License,
  or (at your option) any later version.
  PulseAudio is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License
  along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.
***/
#ifndef _utils_H_
#define _utils_H_
#include <pulse/simple.h>
#include <pulse/error.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <msg.h>
#include <errno.h>

#define BUFSIZE 1024

static const pa_sample_spec ss = {
    .format = PA_SAMPLE_S16LE, .rate = 40100, .channels = 2};

struct pulseaudio_con{
    int socket;
    struct group_message message; 
};

int loop_write(int fd,void *data,size_t size)
{
    ssize_t total_byte_sent=0;
    while (size>0){
        ssize_t write_done_cycle;
        if ((write_done_cycle=write(fd,data,size))<0)
            return write_done_cycle;
        if (write_done_cycle == 0)
            break;
        
        total_byte_sent += write_done_cycle;
        data = (uint8_t*) data + write_done_cycle;
        size -= write_done_cycle;
    }

    return total_byte_sent;
}

int loop_read(int fd,void *data,size_t size)
{
    ssize_t total_byte_sent=0;
    while (size>0){
        ssize_t write_done_cycle;
        if ((write_done_cycle=read(fd,data,size))<0)
            return write_done_cycle;
        if (write_done_cycle == 0)
            break;
        
        total_byte_sent += write_done_cycle;
        data = (uint8_t*) data + write_done_cycle;
        size -= write_done_cycle;
    }

    return total_byte_sent;
}

void  *voice_receive(void *sock)
{
    struct pulseaudio_con *socket_recv = (struct pulseaudio_con *)sock;
    int socket_fd=socket_recv->socket;
    pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, socket_recv->message.user_name, PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        if (s)
            pa_simple_free(s);
        return 0;
    }

 

    for (;;) {
        ssize_t r;
        struct group_message incomming;
        /* Read some data ... */
        if ((r = loop_read(socket_fd, &incomming, sizeof(incomming))) <= 0) {
            if (r == 0) /* EOF */
                break;
            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            if (s)
                pa_simple_free(s);
            return 0;
        }
        /* ... and play it */
        if (pa_simple_write(s, incomming.message, (size_t) r, &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            if (s)
                pa_simple_free(s);
            return 0;
        }
    }
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        if (s)
            pa_simple_free(s);
        return 0;
    }
    ret = 0;
    if (s)
        pa_simple_free(s);
    return 0;
}
void *voice_send(void *sock)
{
    struct pulseaudio_con *socket_send = (struct pulseaudio_con *)sock;
    pa_simple *s = NULL;
    int ret = 1;
    int socket_send_fd=socket_send->socket;
    int error;
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, socket_send->message.user_name, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        if (s)
                pa_simple_free(s);
            return 0;
    }
    struct group_message send_msg=socket_send->message;
    for (;;) {
        uint8_t buf[BUFSIZE];
        if (pa_simple_read(s, send_msg.message , sizeof(send_msg.message), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            break;
        }
        /* And write it to STDOUT */
        if (loop_write(socket_send_fd, &send_msg, sizeof(send_msg)) != sizeof(send_msg)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            break;
        }
    }
    ret = 0;
    if (s)
        pa_simple_free(s);
    return 0;    
}
#endif