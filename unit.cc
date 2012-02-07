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

  assert(0); // Add unit tests for other modules
}
