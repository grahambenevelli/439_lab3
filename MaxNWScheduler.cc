#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "MaxNWScheduler.h"
#include "NWScheduler.h"
#include "util.h"

MaxNWScheduler::MaxNWScheduler(long bytesPerSec)
{
  	assert(1); // TBD: Fill this in
  	maxBytes = bytesPerSec;
	smutex_init(&Lock);
	scond_init(&SafeSend);
	deadline= nowMS();
	dlmet = false;
	threads = 0;
	//if ((alarm = fork()) != 0)
		//startAlarmThread(this);
}

MaxNWScheduler::~MaxNWScheduler() {
	smutex_destroy(&Lock);
	scond_destroy(&SafeSend);
	//kill(alarm, -9);
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
	assert(1); // TBD: Fill this in
	smutex_lock(&Lock);
	threads++;
	while(!canSafelySend()) { // deadline >= now
		//printf("# waiting for wait my turn\n");
		scond_wait(&SafeSend, &Lock);
	}

	// wait till deadline

	threads--;
	//printf("#--DW\n");
	// update totalTransmittedBytes
	totalTransmittedBytes = 1000*lenToSend;
	//totalTransmittedBytes += lenToSend;

	smutex_unlock(&Lock);
}


bool
MaxNWScheduler::canSafelySend() {
	//return deadline < nowMS();
	return totalTransmittedBytes == 0;
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
	smutex_lock(&Lock);
	
	long long nextDL = prevDeadlineMS;
	if(prevDeadlineMS == 0) { // if this is first pass through, add current time.
		nextDL += nowMS();
	}

	if(totalTransmittedBytes > 0) {
		//printf("TTB: %ld\n", totalTransmittedBytes);
		//printf("maxBytes: %ld\n", maxBytes); 
		nextDL += ((double) totalTransmittedBytes / (double) maxBytes);
		assert(nextDL > prevDeadlineMS);
		scond_broadcast(&SafeSend, &Lock);
		totalTransmittedBytes = 0;
	}

	smutex_unlock(&Lock);	
	return nextDL;
}
