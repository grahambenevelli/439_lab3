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
	runOff = 0;
	needAddTime = false;
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
	
	while(!canSafelySend()) { // no one is sending
		//printf("# waiting for wait my turn\n");
		scond_wait(&SafeSend, &Lock);
	}
	
	
	// wait till deadline
	//printf("#%d sent %d bytes\n", ignoredFlowID, lenToSend);
	
	
	
	

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
		nextDL += nowMS() + 1;
	}

	if(totalTransmittedBytes > 0) { 
	        //nextDL += (((double) totalTransmittedBytes + 1) / (double) maxBytes);
		//nextDL += ((totalTransmittedBytes) / maxBytes);
		//printf("Add Time: \t%ld\n", ((totalTransmittedBytes) / maxBytes));
		nextDL = calRunOff(totalTransmittedBytes, maxBytes, nextDL);		
		// calculate run off
		// calRunOff(totalTransmittedBytes, maxBytes);
		assert(nextDL > prevDeadlineMS);
		scond_broadcast(&SafeSend, &Lock);
		totalTransmittedBytes = 0;
	}

	smutex_unlock(&Lock);	
	return nextDL;
}

long long
MaxNWScheduler::calRunOff(long tran, long max, long long time) {
	//printf("tran: \t%ld\n", tran);
	//printf("max: \t%ld\n", max);
	//assert(0);
	
	while (tran >= max) { time++; tran -= max; }
	runOff += tran;
	if (runOff >= max) {time += 2; runOff -= max;}
	//if (runOff > 0  && needAddTime){
	//	time++;
	//	needAddTime = false;	
	//}
	return time;
}
