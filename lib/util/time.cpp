//
// Created by bknun on 5/3/2017.
//
#include <time.h>
#include <chrono>
#ifndef __unix__
#include <afxres.h>
#endif
#include "../../stdimports.h"


#ifdef WIN32_
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

// code snippet goes to: https://gist.github.com/ugovaretto/5875385
int gettimeofday(struct timeval *tv, struct timezone *tz)
{
    FILETIME ft;
    unsigned __int64 tmpres = 0;
    static int tzflag = 0;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        tmpres /= 10;  /*convert into microseconds*/
        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS;
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }
        tz->tz_minuteswest = _timezone / 60;
        tz->tz_dsttime = _daylight;
    }

    return 0;
}

#endif

struct tm  tstruct;
int __os_time(int ty) {
    time_t     now = time(0);
    tstruct = *localtime(&now);
    if(ty==0)
        return tstruct.tm_sec;
    else if(ty==1)
        return tstruct.tm_min;
    else if(ty==2)
        return tstruct.tm_hour;
    else if(ty==3)
        return tstruct.tm_mday;
    else if(ty==4)
        return tstruct.tm_mon;
    else if(ty==5)
        return tstruct.tm_year;
    else if(ty==6)
        return tstruct.tm_wday;
    else if(ty==7)
        return tstruct.tm_yday;
    else if(ty==8)
        return tstruct.tm_isdst;
    else
        return tstruct.tm_sec;
}

/*
* Get the wall-clock date/time, in usec.
*/
long getWallTimeInUsec()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return  tv.tv_usec;
}

/*
* The total ammount of time in micro seconds
* since Jan 1, 1970 (UTC Format)
*/
uint64_t realTimeInUSecs()
{
    uint64_t timeMills, timeUSecs, millsToUsecs;

    timeMills = std::chrono::duration_cast<std::chrono::milliseconds>
            (std::chrono::system_clock::now().time_since_epoch()).count();
    timeUSecs = getWallTimeInUsec();

    /* Subtract the current mills in present second */
    timeMills = timeMills - (timeUSecs / 1000LL);
    millsToUsecs = timeMills * 1000;

    return millsToUsecs + timeUSecs;
}