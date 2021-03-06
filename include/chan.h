#ifndef __CHAN_H__
#define __CHAN_H__
#include <pthread.h>

#define CHAN_OPEN 1
#define CHAN_CLOSED 0

typedef int chan_state_t;

/* Bounded, blocking channels.
   A channel may be in either of two states: open or closed. A closed channel
   will not admit any new elements, and reading from a closed, empty channel
   will return immediately rather than block, as the reading thread would
   otherwise be blocked indefinitely due to no new elements being able to
   enter the channel.
 */
typedef struct chan *chan_t;

/* Create a new channel with space for max_elems elements, each elem_size
   bytes in size. Elements are not cache aligned.
 */
chan_t chan_new(int elem_size, int max_elems);

/* Put a channel into the closed state. A closed channel can not be reopened.
 */
void chan_close(chan_t c);

/* Read an element from a channel into the given buffer.
   In the open state, attempting to read from an empty channel will block.

   In the closed state, reading from an empty channel will *not* block.
   Consumers should use chan_last_read_ok to check whether a read succeeded
   or not before using the contents of the buffer.
*/
void chan_read(chan_t c, void *buf);

/* Write an element from the given buffer into a channel.
   Writing to a full channel in the open state will block until the channel is
   no longer full, and chan_write will return nonzero upon resumption.

   Writing to a channel in the closed state will always be a non-blocking no-op
   which returns 0. If a write is blocking on a full channel when chan_close is
   called, the write will happen as soon as the channel is not full anymore.
   Any subsequent writes will still be discarded.
*/
int chan_write(chan_t c, void *buf);

/* Returns 0 if this channel was closed and empty at the last attempted
   read, otherwise returns nonzero.
*/
int chan_last_read_ok(chan_t c);

#endif /* __CHAN_H__ */
