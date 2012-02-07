#ifndef _MAX_NW_SCHEDULER_H_
#define _MAX_NW_SCHEDULER_H_
#include "NWScheduler.h"
#include "sthread.h"
class MaxNWScheduler:public NWScheduler{
  /*
   * TBD: Fill this in
   */
 public:
  MaxNWScheduler(long bytesPerSec);
  void waitMyTurn(int flowId, float weight, int lenToSend);
  long long signalNextDeadline(long long deadlineMS);
};
#endif 
