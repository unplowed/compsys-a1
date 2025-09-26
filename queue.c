#include "queue.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

void initialize(struct Queue *queue, uint size, uint capacity) {
  queue->capacity = capacity;
  queue->length = 0;
  queue->index = 0;
  queue->size = size;
  queue->data = malloc(size * capacity);
}
void resize(struct Queue *queue, uint capacity) {
  assert(queue->length < capacity && "Cannot resize a queue to a size smaller "
                                     "than its current number of items");
  void *data = malloc(queue->size * capacity);
  queue->data = memcpy(data, queue->data, queue->size * capacity);
}

void push_queue(struct Queue *queue, void *element) {
  if (queue->length + 1 == queue->capacity)
    resize(queue, (uint)(queue->capacity * 1.5f));

  memcpy(queue->data + ((queue->index + queue->length) % queue->capacity),
         element, queue->size);
  queue->length++;
}

void pop_queue(struct Queue *queue, void *out) {
  assert(queue->length > 0 && "Tried to pop empty queue!");

  get_queue(queue, 0, out);
  queue->length--;
  queue->index++;
}
void get(struct Queue *queue, uint index, void *out) {
  memcpy(out,
         &queue->data[(queue->index + queue->length - 1 + index) %
                      queue->capacity],
         queue->size);
}
