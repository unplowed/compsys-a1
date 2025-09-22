#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "id_query.h"
#include "record.h"

// sized array
struct NaiveData {
  struct Record *records;
  int length;
};

struct NaiveData *mk_naive(struct Record *records, int length) {
  struct NaiveData *data = malloc(sizeof(struct NaiveData));
  data->records = records;
  data->length = length;
  return data;
}

void free_naive(struct NaiveData *data) {
  free(data->records);
  free(data);
}

const struct Record *lookup_naive(struct NaiveData *data, int64_t needle) {
  for (int i = 0; i < data->length; i++) {
    if (data->records[i].osm_id == needle)
      return &data->records[i];
  }
  return NULL;
}

int main(int argc, char **argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_naive,
                       (free_index_fn)free_naive, (lookup_fn)lookup_naive);
}
