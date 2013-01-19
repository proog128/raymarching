#include "Timer.h"
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

Timer::Timer()
{
  start();
}

Timer::~Timer()
{
}

#ifdef WIN32
void Timer::stop(const char* infoText)
{
  LARGE_INTEGER t2;
  QueryPerformanceCounter(&t2);
  printf("%s%.3f ms\n", infoText, ((float)(t2.QuadPart - _t.QuadPart) / (float)_f.QuadPart) * 1000.f);
}

void Timer::start()
{
  QueryPerformanceFrequency(&_f);
  QueryPerformanceCounter(&_t);
}
#else
void Timer::stop(const char* infoText)
{
	timespec end;
	clock_gettime(CLOCK_REALTIME, &end);

	int nSecDiff = end.tv_nsec - starttime.tv_nsec;
	int secDiff = end.tv_sec - starttime.tv_sec;

	if (nSecDiff < 0)
	{
		nSecDiff += 1000000000;
		secDiff -= 1;
	}

  int mSecDiff = nSecDiff / 1000000;
	int ySecDiff = (nSecDiff / 1000) % 1000;
	nSecDiff = nSecDiff % 1000;

  printf("%s%d.%03d%03d%03d s\n", infoText, secDiff, mSecDiff, ySecDiff, nSecDiff);
}
void Timer::start()
{
  clock_gettime(CLOCK_REALTIME, &starttime);
}
#endif
