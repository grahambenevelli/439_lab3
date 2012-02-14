#include <assert.h>
#include <stdio.h>
#include <sys/time.h>
#include "MaxNWScheduler.h"
#include "NWScheduler.h"
#include "util.h"

MaxNWScheduler::MaxNWScheduler(long bytesPerSec)
{
  	maxBytes = bytesPerSec; 
	assert( maxBytes == bytesPerSec);
	assert( maxBytes > 0);

	smutex_init(&Lock);	//init lock
	scond_init(&SafeSend);	//init Condition variable
	
	runOff = 0;
	BytesToSend = 0;
}

MaxNWScheduler::~MaxNWScheduler() {
	smutex_destroy(&Lock);
	scond_destroy(&SafeSend);
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
	smutex_lock(&Lock);
	assert(lenToSend <= maxBytes);

	while(!canSafelySend()) { // check if we can safely send
		scond_wait(&SafeSend, &Lock); //wait if we can't
	}

	// update BytesToSend
	// we times by 1000 here to accomodate for the second to millisecond conversion for our deadline
	BytesToSend = 1000*lenToSend;

	smutex_unlock(&Lock);
}


bool
MaxNWScheduler::canSafelySend() {
	// we can only safely send if the amount of bytes to send is zero
	// otherwise, another thread is currently sending
	return BytesToSend == 0;
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

	if(prevDeadlineMS == 0) { // if this is first pass through, upadte to current time.
		nextDL += nowMS() + 1;
	}

	if(BytesToSend > 0) { // 
		// calculate run off
		nextDL = calRunOff(BytesToSend, maxBytes, nextDL);
     		scond_broadcast(&SafeSend, &Lock);	//broadcast to other waiting threads.
		BytesToSend = 0;			//reset amount of bytes to send, allows other threads to continue.
	}

	smutex_unlock(&Lock);
	return nextDL;
}


/*
calRunOff(long tran, long max, long long time)

Calculates the amount of time to add to the next deadline
while keeping track of runoff bytes.

tran = amount of bytes to transmit * 1000;
max = max amount of bytes per second
time = last deadline
*/
long long
MaxNWScheduler::calRunOff(long tran, long max, long long time) {
	while (tran >= max) {   // essentially time += (tran / max); but without the rounding
		time++;
		tran -= max;
	}

	runOff += tran;  //add the run off remaining from tran

	if (runOff >= max) {  //whenever enough runoff is accumulated, add 2 ms to our deadline
		time += 1;
		runOff -= max;  // keep track of remaining runoff
	}

	return time;
}



/*

Unit Tests!

*/

void
MaxNWScheduler::unitTest() {
  int test = 1;
  bool result;

  //test 1 - check maxBytes is set in constructor
  result = maxBytes == 1000;
  if (!result) {
    printf("Test %d failed\n", test);
    return;
  }
  test++;

  //test 2 - check to see that update sets Bytes to send
  waitMyTurn(0, 0.0, 500);
  result = maxBytes == 1000;
  if (!result) {
    printf("Test %d failed\n", test);
    return;
  }
  
  result = BytesToSend == 500*1000;
  if (!result) {
    printf("Test %d failed\n", test);
    return;
  }
  test++;

  // test 3 - see if threads are scheduled
  if (fork()) {
    wait();
    if(fork()) {
      signalNextDeadline(nowMS());
      wait();
    } else {
      waitMyTurn(0, 0.0, 1);
      result = BytesToSend == 1*1000;
      if (!result) {
        printf("Test %d failed\n", test);
        return;
      }
      exit(0);
    }
  } else {
    waitMyTurn(0, 0.0, 2);
    result = BytesToSend == 2*1000;
    if (!result) {
      printf("Test %d failed\n", test);
      return;
    }
  }
  test++;

  // test 4 - test runoff
  result = runOff == 0;
  if (!result) {
    printf("Test %d failed\n", test);
    return;
  }
  test++;

  // test 5
  maxBytes = 10000;
  runOff = 0;
  waitMyTurn(0, 0.0, 1);
  signalNextDeadline(0);
  result = runOff == 1000;
  if (!result) {
    printf("Test %d failed\n", test);
    return;
  }
  test++;



  printf("All Tests Passed: MaxNWScheduler\n");
}
