#ifndef TIMER_H
#define TIMER_H

#ifndef WIN32
#include <sys/time.h>
#else
#include <windows.h>
#endif

class Timer
{
public:
  Timer();
  ~Timer();

  void stop(const char* infoText = "");
  void start();

private:
#ifdef WIN32
  LARGE_INTEGER _t;
  LARGE_INTEGER _f;
#else
  timespec starttime;
#endif
};

#endif
