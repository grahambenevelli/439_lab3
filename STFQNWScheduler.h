#ifndef _STFQ_NW_SCHEDULER_H_
#define _STFQ_NW_SCHEDULER_H_
#include "NWScheduler.h"
#include "sthread.h"


class STFQNWScheduler:public NWScheduler{
  /*
   * TBD: Fill this in
   */
 public:
  STFQNWScheduler(long bytesPerSec);
  void waitMyTurn(int flowId, float weight, int lenToSend);
  long long signalNextDeadline(long long deadlineMS);

};
#endif 
