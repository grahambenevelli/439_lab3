#ifndef _STATS_H_
#define _STATS_H_
#include <sys/time.h>
#include "sthread.h"
class Stats{
 public:
  Stats();
  ~Stats();
  void update(int flowId, int byteCount);
  char *toString(char *buffer, int maxLen);
  void unitTest();

  static const int MAX_FLOW_ID = 1023;

 private:
  smutex_t lock; // lock variable
  long long byte_array[MAX_FLOW_ID]; // an array of bytes that have been sent by flow id
  int max_id; // of the threads
};
#endif  
