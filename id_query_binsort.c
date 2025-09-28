#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "id_query.h"
#include "record.h"

// A struct for a single index entry
typedef struct IndexRecord
{
    int64_t osm_id;              // Record ID
    const struct Record *record; // pointer to record itself
} IndexRecord;

// Struct that holds the sorted index array and # of records
typedef struct IndexedData
{
    IndexRecord *irc; // sorting osm_id by ascending order
    int n;            // number of records
} IndexedData;

// Helper function
int comparator(const void *a, const void *b)
{
    const int64_t x = ((const IndexRecord *)a)->osm_id;
    const int64_t y = ((const IndexRecord *)b)->osm_id;
    return (x > y) - (x < y);
}

// Binary search over the sorted index array.
// Returns pointer --> Record if success,if not found --> NULL
const struct Record *indexed_bin_search(const IndexedData *indexed_data, int64_t needle)
{
    int low = 0;
    int high = indexed_data->n - 1;

    while (low <= high)
    {

        int mid = low + (high - low) / 2;
        int64_t id = indexed_data->irc[mid].osm_id;

        if (id < needle)
        {
            low = mid + 1;
        }
        else if (id > needle)
        {
            high = mid - 1;
        }
        else
        {
            return indexed_data->irc[mid].record;
        }
    }
    return NULL;
}

void *mk_indexed(const struct Record *rc, int n)
{
    if (rc == NULL || n < 0)
    {
        errno = EINVAL;
        return NULL;
    }

    IndexedData *indexed_data = (IndexedData *)malloc(sizeof *indexed_data);
    if (!indexed_data)
        return NULL;

    indexed_data->n = n;

    indexed_data->irc = (IndexRecord *)malloc((size_t)n * sizeof *indexed_data->irc);
    if (!indexed_data->irc)
    {
        free(indexed_data);
        return NULL;
    }

    // Populating the index with id's and poointers to rc
    for (int i = 0; i < n; ++i)
    {
        indexed_data->irc[i].osm_id = rc[i].osm_id;
        indexed_data->irc[i].record = &rc[i];
    }

    // Sort by ID
    qsort(indexed_data->irc, (size_t)n, sizeof(IndexRecord), comparator);

    // Returns indexed data after processing
    return indexed_data;
}

// Freeing memory from built index
void free_indexed(void *data)
{
    if (!data)
        return;
    IndexedData *indexed_data = (IndexedData *)data;
    free(indexed_data->irc);
    free(indexed_data);
}

// Lookup an ID via binary search on the sorted indexed data
const struct Record *lookup_indexed(void *data, int64_t needle)
{
    if (!data)
        return NULL;
    const IndexedData *indexed_data = (const IndexedData *)data;
    if (indexed_data->n <= 0)
        return NULL;
    return indexed_bin_search(indexed_data, needle);
}

int main(int argc, char **argv)
{
    return id_query_loop(argc, argv, mk_indexed, free_indexed, lookup_indexed);
}