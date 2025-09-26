#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "id_query.h"
#include "record.h"

struct IndexRecord {
  int64_t osm_id;
  const struct Record *record;
};

// sized array
struct IndexedData {
  struct IndexRecord *records;
  int length;
};

struct IndexedData *mk_indexed(struct Record *records, int length) {
  struct IndexedData *data = malloc(sizeof(struct IndexedData));
  data->records = malloc(sizeof(struct IndexRecord) * length);
  data->length = length;

  for (int i = 0; i < length; i++) {
    data->records[i].osm_id = records[i].osm_id;
    data->records[i].record = &records[i];
  }

  return data;
}

void free_indexed(struct IndexedData *data) {
  for (int i = 0; i < data->length; i++) {
    free((void *)data->records[i].record);
    free(&data->records[i]);
  }
  free(data);
}

const struct Record *lookup_indexed(struct IndexedData *data, int64_t needle) {
  for (int i = 0; i < data->length; i++) {
    if (data->records[i].osm_id == needle)
      return data->records[i].record;
  }
  return NULL;
}

int main(int argc, char **argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_indexed,
                       (free_index_fn)free_indexed, (lookup_fn)lookup_indexed);
}
