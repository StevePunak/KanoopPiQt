#include "kanooppiqt.h"
#include "klog.h"
#include <stdio.h>

KanoopPiQt::KanoopPiQt()
{
}

void init()
{
    KLog::sysLogText(KLOG_INFO, "Library entry");
}

void deinit()
{
    KLog::sysLogText(KLOG_INFO, "Library exit");
}
