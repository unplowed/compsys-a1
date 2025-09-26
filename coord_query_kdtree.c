#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "id_query.h"
#include "queue.h"
#include "record.h"

// sized array
struct KDTree {
  struct Record *value;
  double lon;
  double lat;
  struct KDTree *left;
  struct KDTree *right;
};

enum Axis { AXIS_X, AXIS_Y };

enum Axis swap_axis(enum Axis axis) __attribute__((always_inline));
enum Axis swap_axis(enum Axis axis) {
  switch (axis) {
  case AXIS_X:
    return AXIS_Y;
  case AXIS_Y:
    return AXIS_X;
  }
}

int compare_x(struct Record *a, struct Record *b) { return a->lat - b->lat; }
int compare_y(struct Record *a, struct Record *b) { return a->lon - b->lon; }

__compar_fn_t axis_compare_func(enum Axis axis) {
  switch (axis) {
  case AXIS_X:
    return (__compar_fn_t)compare_x;
  case AXIS_Y:
    return (__compar_fn_t)compare_y;
  }
}

struct Work {
  uint index;
  uint length;
};

struct KDTree *mk_kdtree(struct Record *records, int length) {
  struct KDTree *tree = malloc(sizeof(struct KDTree) * length);
  uint tree_length = 0;
  struct Work *queue = malloc(sizeof(struct Work) * length);
  uint queue_length = 0;

  enum Axis axis = AXIS_X;
  struct Work work = {.index = 0, .length = length};
  queue[queue_length++] = work;

  // 1. Sort by axis
  // 2. Find median
  // 3. Put middle in kdtree array
  // 4. Invert axis
  // 5. Apply step 1 to both halves
  // 6. Repeat until list empty

  while (queue_length-- > 0) {
    // 1. Sort by axis
    qsort(&records[queue[0].index], queue[0].length, sizeof(struct Record),
          axis_compare_func(axis));

    if (queue[0].length <= 2) {
      // end case
      // insert left, then right as child of left
      struct Record *left = &records[queue[0].index];
      struct KDTree left_tree = {
          .value = left,
          .lon = left->lon,
          .lat = left->lat,
      };

      struct Record *right = &records[queue[0].index + 1];
      struct KDTree right_tree = {
          .value = right,
          .lon = right->lon,
          .lat = right->lat,
      };

      tree[tree_length++] = left_tree;
      tree[tree_length++] = right_tree;
      continue;
    }

    // 2. find median
    uint len = queue[0].length / 2;

    // 5. push new work to queue
    struct Work less_half = {.index = 0, .length = len - 1};
    queue[queue_length++] = less_half;
    struct Work more_half = {.index = len + 1,
                             .length = (queue[0].length - len) + 1};
    queue[queue_length++] = more_half;

    struct Record *r = &records[len];
    struct KDTree new = {
        .value = r,
        .lon = r->lon,
        .lat = r->lat,
    };
    tree[tree_length++] = new;
  }
  assert(0);
}

void free_kdtree(struct KDTree *data) { assert(0); }

const struct KDTree *lookup_kdtree(struct KDTree *data, double lon,
                                   double lat) {
  assert(0);
}

int main(int argc, char **argv) {
  return id_query_loop(argc, argv, (mk_index_fn)mk_kdtree,
                       (free_index_fn)free_kdtree, (lookup_fn)lookup_kdtree);
}
