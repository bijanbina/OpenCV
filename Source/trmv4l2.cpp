#include "trmv4l2.h"

trmV4l2::trmV4l2()
{

}

QStringList getDeviceName()
{
    QStringList return_data;
    QDir *dip = new QDir("/dev","video*",QDir::SortFlags(QDir::Name | QDir::IgnoreCase ), QDir::AllEntries | QDir::System);
    return_data = dip->entryList();
    return return_data;
}

//struct buffer *buffers;
//static unsigned int n_buffers;
//static int out_buf;
//static int force_format;
//static int frame_count = 200;
//static int frame_number = 0;


//static int xioctl(int fh, int request, void *arg)
//{
//    int r;

//    do {
//        r = ioctl(fh, request, arg);
//    } while (-1 == r && EINTR == errno);

//    return r;
//}

//static void process_image(const void *p, int size)
//{
//    frame_number++;
//    char filename[15];
//    sprintf(filename, "frame-%d.raw", frame_number);
//    FILE *fp=fopen(filename,"wb");
//    if (out_buf)
//        fwrite(p, size, 1, fp);

//    fflush(fp);
//    fclose(fp);
//}

//static int read_frame(int fd)
//{
//    struct v4l2_buffer buf;
//    unsigned int i;

//    switch (io)
//    {
//    case IO_METHOD_READ:
//        read(fd, buffers[0].start, buffers[0].length);
//        process_image(buffers[0].start, buffers[0].length);
//        break;

//    case IO_METHOD_MMAP:
//        CLEAR(buf);

//        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        buf.memory = V4L2_MEMORY_MMAP;

//        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
//            switch (errno) {
//            case EAGAIN:
//                return 0;

//            case EIO:
//                /* Could ignore EIO, see spec. */

//                /* fall through */

//            default:
//                errno_exit("VIDIOC_DQBUF");
//            }
//        }

//        assert(buf.index < n_buffers);

//        process_image(buffers[buf.index].start, buf.bytesused);

//        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
//            errno_exit("VIDIOC_QBUF");
//        break;

//    }

//    return 1;
//}
