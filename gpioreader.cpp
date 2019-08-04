#include "gpioreader.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "klog.h"
#include "wiringPi.h"

int GPIOReader::sighupFd[2];

GPIOReader::GPIOReader()
{
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sighupFd))
       qFatal("Couldn't create socketpair");

    snHup = new QSocketNotifier(sighupFd[1], QSocketNotifier::Read, this);
    connect(snHup, &QSocketNotifier::activated, this, &GPIOReader::handleSigHup);

    int result = wiringPiSetup();
    KLog::sysLogText(KLOG_DEBUG, "wiringPiSetup %d", result);

    result = wiringPiISR (26, INT_EDGE_BOTH, &hupSignalHandler);
    KLog::sysLogText(KLOG_DEBUG, "ISR setup %d", result);
}

void GPIOReader::handleSigHup(int fd)
{
    KLog::sysLogText(KLOG_DEBUG, "HUP on %d--- BAM!", fd);
}

void GPIOReader::hupSignalHandler()
{
    char a = 1;
    ::write(sighupFd[0], &a, sizeof(a));
}
