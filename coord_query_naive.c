#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "coord_query.h"
#include "record.h"

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
  free_records(data->records, data->length);
  free(data);
}

double calculate_distance(struct Record *record, double lon, double lat) {
  return sqrt(pow((record->lat - lat), 2) + pow((record->lon - lon), 2));
}

const struct Record *lookup_naive(struct NaiveData *data, double lon,
                                  double lat) {

  struct Record *closest_record = NULL;
  double closest_distance = 100000000;
  for (int i = 0; i < data->length; i++) {
    double dist = calculate_distance(&data->records[i], lon, lat);
    if (dist < closest_distance) {
      closest_record = &data->records[i];
      closest_distance = dist;
    }
  }
  return closest_record;
}

int main(int argc, char **argv) {
  return coord_query_loop(argc, argv, (mk_index_fn)mk_naive,
                          (free_index_fn)free_naive, (lookup_fn)lookup_naive);
}
