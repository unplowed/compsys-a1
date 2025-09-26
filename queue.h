#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct Queue {
  uint index; // current index in the ringbuffer
  uint length; // number of items in data
  uint size; // size of each item
  uint capacity; // capacity for items
  void *data; // ringbuffer pointer
};

void push_queue(struct Queue *queue, void* element);
void pop_queue(struct Queue *queue, void* out);
void get_queue(struct Queue *queue, uint index, void* out);
void resize_queue(struct Queue *queue, uint capacity);
void initialize_queue(struct Queue *queue, uint size, uint capacity);
