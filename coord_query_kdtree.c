#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "coord_query.h"
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

enum Axis swap_axis(enum Axis axis) {
  switch (axis) {
  case AXIS_X:
    return AXIS_Y;
  case AXIS_Y:
    return AXIS_X;
  }
}

int compare_x(const void *a, const void *b) {
  return ((struct Record *)a)->lat - ((struct Record *)b)->lat;
}
int compare_y(const void *a, const void *b) {
  return ((struct Record *)a)->lon - ((struct Record *)b)->lon;
}

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

struct KDTree *make_tree_rec(struct Record *records, int length,
                             enum Axis axis);

struct KDTree *mk_kdtree(struct Record *records, int length) {
  // 1. Sort by axis
  // 2. Find median
  // 3. Put middle in kdtree array
  // 4. Invert axis
  // 5. Apply step 1 to both halves
  // 6. Repeat until list empty

  struct KDTree *tree = make_tree_rec(records, length, AXIS_X);

  return tree;
}

struct KDTree *make_tree_rec(struct Record *records, int length,
                             enum Axis axis) {
  struct KDTree *tree = malloc(sizeof(struct KDTree));

  // 1. sort
  qsort(records, length, sizeof(struct Record), axis_compare_func(axis));

  // 2. find median
  struct Record *median = records + (length / 2);

  tree->lat = median->lat;
  tree->lon = median->lon;
  tree->value = median;

  if (length < 2) {
    tree->left = NULL;
    tree->right = NULL;
    return tree;
  }

  tree->left = make_tree_rec(records, length / 2 - 1, swap_axis(axis));
  tree->right =
      make_tree_rec(records + length / 2 + 1, length / 2 - 1, swap_axis(axis));

  return tree;
}

void free_kdtree(struct KDTree *data) {
  if (data->left != NULL)
    free_kdtree(data->left);
  if (data->right != NULL)
    free_kdtree(data->right);

  free(data->value);
  free(data);
}

double dist_to_record(struct Record *record, double lat, double lon) {
  return pow(record->lat - lat, 2) + pow(record->lon - lon, 2);
}

const struct Record *lookup_kdtree(struct KDTree *data, double lon,
                                   double lat) {
  struct KDTree **tree = malloc(sizeof(struct KDTree *) * 100000);
  uint size = 0;
  struct Record *closest;
  double closest_dist = 999999999999999999999999.0f;

  tree[size++] = data;

  while (size > 0) {
    struct KDTree *e = tree[--size];

    double dist = dist_to_record(e->value, lat, lon);
    if (dist < closest_dist) {
      closest_dist = dist;
      closest = e->value;
    }

    if (e->left != NULL)
      tree[size++] = e->left;

    if (e->right != NULL)
      tree[size++] = e->right;
  }

  free(tree);

  return closest;
}

int main(int argc, char **argv) {
  return coord_query_loop(argc, argv, (mk_index_fn)mk_kdtree,
                          (free_index_fn)free_kdtree, (lookup_fn)lookup_kdtree);
}
