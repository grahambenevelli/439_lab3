#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "Stats.h"


/*
 *------------------------------------------------------------------
 *
 * Stats --
 *
 *          Constructor
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
 *
 *------------------------------------------------------------------
 */
Stats::Stats()
{

    //
    // Initialize per-object state
    //
    //           REMEMBER
    // 
    // You *must* follow the coding standards
    // distributed on the class web page.
    // Solutions failing to conform to these
    // standards will receive little or
    // no credit.
    //
  smutex_init(&lock);
  smutex_lock(&lock);

  int i;
  for (i = 0 ; i < MAX_FLOW_ID ; i++ ) {
    byte_array[i] = 0;
  }

  max_id = 0;

  smutex_unlock(&lock);
}



/*
 *------------------------------------------------------------------
 *
 * ~Stats --
 *
 *          Destructor
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
Stats::~Stats()
{
  smutex_destroy(&lock);
}


/*
 *------------------------------------------------------------------
 *
 * update --
 *
 *          Update the count for a stream.
 *
 * Arguments:
 *      int flowId -- the Id of the stream
 *      int count -- the number of bytes sent/received
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
void 
Stats::update(int flowId, int byteCount)
{
    //
    // Fill in this code. 
    //
    // Keep track of how many bytes each flowId
    // sent since the start of the run.
    //
    // Also keep track of the maximum flowID
    // you've seen.
    //
    // You *may* assume that flowIDs are small
    // consecutive non-negative integers between 0 
    // and MAX_FLOW_ID, inclusive.
    //
    // Do synchronize access to your shared
    // state. 
    //
    //
    //           REMEMBER
    // 
    // You *must* follow the coding standards
    // distributed on the class web page.
    // Solutions failing to conform to these
    // standards will receive little or
    // no credit.
    //


	// get lock
	smutex_lock(&lock);

	// make changes
	if (flowId > max_id) max_id = flowId;
	byte_array[flowId] += byteCount; 

	// release lock
	smutex_unlock(&lock);
}


/*
 *------------------------------------------------------------------
 *
 * toString --
 *
 *          Return a string of counts separated by ' '.
 *
 * Arguments:
 *      char *buffer -- a buffer into which the string
 *                      should be written
 *      int maxLen -- the number of bytes available in this buffer
 *                    (Caller should always hand in large enough
 *                    buffer; callee should be sure not to write 
 *                    past the end of the buffer!)
 *
 * Results:
 *      Return the pointer to the buffer.
 *
 * Side effects:
 *      None.
 *
 *------------------------------------------------------------------
 */
char *
Stats::toString(char *buffer, int maxLen)
{
    //
    // Fill in this code. 
    //
    // Produce a string that has a space-separated
    // list of how much data has been sent by
    // each flow since the last call to toString.
    // e.g.,
    // [sent_0] [sent_1] [sent_2] ... [tot]
    //
    // [sent_i] is the number of bytes for flow i whose
    // IO completed since the last call to toString().
    //
    // [tot] is the total number of bytes whose
    // IO completed since the last call to toString()
    //
    // Note that C or C++ represents strings
    // as an array of characters terminated
    // by the null character (0 or '\0'). Also
    // note that the newline character is '\n'.
    //
    //           REMEMBER
    // 
    // You *must* follow the coding standards
    // distributed on the class web page.
    // Solutions failing to conform to these
    // standards will receive little or
    // no credit.
    //

  smutex_lock(&lock);
  
  int i = 0; // index in bufer
  int j = 0; // index in byte_array
  int k = 0; // index in numstr
  int total = 0;
  char numstr[10];
  for( ; i < max-1 && j < max_id ; j++, i++ ) {
    sprintf(numstr, "%d", byte_array[j]);
    cout << "String: " << numstr << "\n";
    for ( k = 0 ; numstr[k] != '\0' && i < max_id; k++, i++ ) {
      buffer[i] = numstr[k];
    }
    buffer[i] = ' ';
    total += byte_array[j];
  }
  buffer[i] = 0;

  smutex_unlock(&lock);

  return buffer;
}


/*
 *------------------------------------------------------------------
 *
 * unitTest --
 *
 *          Verify simple things that must be true.
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
void
Stats::unitTest()
{
  assert(0); // TBD: add some simple unit tests. 
             // You need to design these tests based on
             // your data structures.
  printf("Stats self test passes.\n");
  return;
  
}
