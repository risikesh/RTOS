#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/videodev2.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>
#include <vc_config.h>


#define FRAME_PS 15
#define BUFFER_SIZE 4


int thread_exit_sig_STREAM_video=0;

struct v4l2_mem_buffer {
  void *start;
  unsigned int length;
} *mem_buffer;
struct fd_connect{
    int fd_stream;
    int fd_con;
};
int capture_open(const char* device)
{
        struct stat st;

        if (-1 == stat(device, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                         device, errno, strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", device);
                exit(EXIT_FAILURE);
        }

        return open(device, O_RDWR /* required */ | O_NONBLOCK, 0);
}


int init_device(int fd)
{
    //SET FORMAT,FRAMERATE,
    struct v4l2_streamparm fps_set;
    struct v4l2_format fmt;
    
    //Setting format to YUYV 640 x 480
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.height = IMAGE_HEIGHT;
    fmt.fmt.pix.width = IMAGE_WIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1) {
        fprintf(stderr, "ERROR: Unable to set format\n");
    return -1;  }

    //setting frame rate    

    memset(&fps_set, 0, sizeof(struct v4l2_streamparm));
    fps_set.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fps_set.parm.capture.timeperframe.numerator = 1;
    fps_set.parm.capture.timeperframe.denominator = FRAME_PS;
    if (ioctl(fd, VIDIOC_S_PARM, &fps_set) == -1) 
    {  fprintf(stderr, "ERROR: Framerate not set \n"); return -1; }

    //Creating mmap, and requesting a buffer 4

    struct v4l2_requestbuffers req;
    req.count = BUFFER_SIZE;// we are using a buffer size of 4
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1) {
        fprintf(stderr, "ERROR: buffer request error\n"); return -1;    }
    // mmap for buffers
    mem_buffer = malloc(req.count * sizeof(struct v4l2_mem_buffer));
    if (!mem_buffer) {
        fprintf(stderr, "ERROR: system out of memory\n");return -1;}

    struct v4l2_buffer buf;
    unsigned int n_buffers;
    for (n_buffers = 0; n_buffers < req.count; n_buffers++) {
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {
            fprintf(stderr, "ERROR: query buffer error\n"); return -1;}
        mem_buffer[n_buffers].length = buf.length;
        mem_buffer[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (mem_buffer[n_buffers].start == MAP_FAILED) {
            fprintf(stderr, "buffer map error %u\n", n_buffers);return -1;}
    }

    struct v4l2_buffer buf_streaming;
    buf_streaming.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf_streaming.memory = V4L2_MEMORY_MMAP;
    for (n_buffers = 0; n_buffers < BUFFER_SIZE; n_buffers++) {
        buf_streaming.index = n_buffers;
        if (ioctl(fd, VIDIOC_QBUF, &buf_streaming) == -1) {
            fprintf(stderr, "queue buffer failed\n"); return -1;}
    }

    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd, VIDIOC_STREAMON, &type) == -1) {
        fprintf(stderr, "stream on failed\n"); return -1;}
    return 0;
}


int close_all(int fd){
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type)) {
        fprintf(stderr, "stream off failed\n");return -1;}
    printf("stream is off\n");

    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
        if (munmap(mem_buffer[i].start, mem_buffer[i].length) == -1) {
            fprintf(stderr, "munmap failure %d\n", i);return -1;}
    }
    close(fd);
return 0;
}
static void* STREAM_video(void *arg){


    int fd = ((struct fd_connect *) ( arg))->fd_stream;
    int fd_con=((struct fd_connect *) ( arg))->fd_con;
    fd_set fds;
    struct v4l2_buffer buf;
    void *buffsend;
    while (!thread_exit_sig_STREAM_video) {
        int ret;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
        ret = select(fd + 1, &fds, NULL, NULL, &tv);
        if (-1 == ret) {
        fprintf(stderr, "select error\n");
        return NULL;
        } else if (0 == ret) {
        fprintf(stderr, "timeout waiting for frame\n");
        continue;
        }
        
        if (FD_ISSET(fd, &fds)) {
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (-1 == ioctl(fd, VIDIOC_DQBUF, &buf)) {
                fprintf(stderr, "VIDIOC_DQBUF failure\n"); return NULL; }
            buffsend=mem_buffer[buf.index].start;
            write(fd_con,buffsend,mem_buffer[buf.index].length);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
                fprintf(stderr, "VIDIOC_QBUF failure\n"); return NULL; }
        }
    }
    return NULL;
}
