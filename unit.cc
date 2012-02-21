#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include "sthread.h"
#include "common.h"
#include "OutputStream.h"
#include "SocketLibrary/socklib.h"
#include "Stats.h"
#include "MaxNWScheduler.h"
#include "STFQueue.h"
#include "STFQNWScheduler.h"
#include "rec_queue.h"

/*
 *------------------------------------------------------------------
 *
 * main --
 *
 *          Call the unit test methods on the objects.
 *
 * Arguments:
 *      None.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
int main(int argc, char **argv)
{
  Stats *s = new Stats();
  
  s->unitTest();

  MaxNWScheduler *nws = new MaxNWScheduler(1000);

  nws->unitTest();

  STFQueue *q = new STFQueue();

  q->unit();

  STFQNWScheduler *sh = new STFQNWScheduler(1000000);
  
  sh->unit();

  rec_queue *rq = new rec_queue();

  rq->unit();

  delete rq;
  delete sh;
  delete s;
  delete nws;
  delete(q);

  assert(1); // Add unit tests for other modules
}
