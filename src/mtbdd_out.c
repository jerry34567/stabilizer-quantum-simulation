#include <stdio.h>
#include <assert.h>
#include "mtbdd_out.h"
#include "error.h"

/// Global variable for the map (needed as custom arguments cannot be added to Sylvan's mtbdd_fprintdot())
static lnum_map_t out_map;

/// Step for array reallocation when the array size isn't sufficient when adding
#define REALLOC_STEP 5

void lnum_map_init(size_t size)
{
    assert(out_map.msize == 0); // Check if not already initialized
    out_map.msize = size;
    out_map.map = my_malloc(sizeof(coef_t*) * size);
    out_map.next_var = 0;
}

uint64_t lnum_map_add(coef_t *n_ptr)
{
    if (out_map.next_var >= out_map.msize) {
        out_map.map = my_realloc(out_map.map, sizeof(coef_t*) * (out_map.msize + REALLOC_STEP));
        out_map.msize += REALLOC_STEP;
    }

    out_map.map[out_map.next_var] = n_ptr;

    return out_map.next_var++; // Returns the current variable that has been just assigned to the number
}

bool lnum_map_is_empty()
{
    return (out_map.next_var == 0);
}

void lnum_map_print(FILE *f)
{
    for (size_t i = 0; i < out_map.next_var; i++) {
        fprintf(f,VAR_NAME_FMT " = ", i);
        if (mpz_out_str(f, 10, *(out_map.map[i])) == 0) {
            error_exit("An error occurred when writing the value of " VAR_NAME_FMT " to the output file.\n", i);
        }
        fprintf(f,"\n\n");
    }
}

void lnum_map_clear()
{
    free(out_map.map);
    out_map.msize = 0;
    out_map.next_var = 0;
}

/* end of "mtbdd_out.c" */