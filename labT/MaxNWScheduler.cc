#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "MaxNWScheduler.h"
#include "NWScheduler.h"

MaxNWScheduler::MaxNWScheduler(long bytesPerSec)
{
  assert(0); // TBD: Fill this in
}

//-------------------------------------------------
// waitMyTurn -- return only after caller may safely
// send. If prev send s0 at time t0 transmitted b0
// bytes, the next send may not send before
// t1 >= t0 + b0/bytesPerSec
//
// NOTE: See the assignent for important restriction.
// In particular, this call must use scond_wait()
// and it may not call sthread_sleep().
// Instead, you must rely on an alarmThread to 
// signal/broadcast when it is OK to proceed.
//
// Note: You can get the current time using
// gettimeofday(), which is defined in <sys/time.h>.
// You will need to convert the struct timeval
// into milliseconds.
//-------------------------------------------------
void
MaxNWScheduler::waitMyTurn(int ignoredFlowID, float ignoredWeight, int lenToSend)
{
  assert(0); // TBD: Fill this in
}

//-------------------------------------------------
// This method is called by the alarm thread.
// It
//   (1) Updates the scheduler's local state to indicate
//       that the time deadlineMS (a time expressed in
//       milliseconds) has been reached.)
//   (2) Signal/broadcast to allow any threads waiting
//       for that deadline to proceed.
//   (3) Wait until the next deadline has been calculated
//   (4) Return the next deadline to the caller
//
// Note: You can get the current time using
// gettimeofday(), which is defined in <sys/time.h>.
// You will need to convert the struct timeval
// into milliseconds.
//-------------------------------------------------
long long
MaxNWScheduler::signalNextDeadline(long long prevDeadlineMS)
{
  assert(0); // TBD: Fill this in
}
