#ifndef TRMV4L2_H
#define TRMV4L2_H

#include <QtWidgets>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <fcntl.h>
#include <sys/ioctl.h>

QStringList getDeviceName();

class trmV4l2
{
public:
    trmV4l2();
};

#endif // TRMV4L2_H
