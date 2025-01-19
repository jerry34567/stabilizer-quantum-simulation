/**
 * @file mtbdd_out.h
 * @brief Large number output using variables in the MTBDD result
 * 
 * @details Separate from 'vmap_t' for better clarity and extensibility.
 *          Works with a single global map, as one cannot add custom arguments to Sylvan's output functions.
 */

#include <stdbool.h>
#include <stdint.h>
#include <gmp.h>
#include "mtbdd.h"

#ifndef MTBDD_OUT_H
#define MTBDD_OUT_H


/// Format string for the variable name
#define VAR_NAME_FMT "large-number[%ld]"


/// Type for saving and using the variable to value mapping for the separate output of large numbers
typedef struct lnum_map {
    /// array for saving the variable mapping to their values (ptrs to complex numbers to avoid realloc)
    coef_t **map;
    /// map size
    size_t msize;
    /// next variable index to be assigned
    uint64_t next_var;
} lnum_map_t;

/**
 * Allocates and initializes the variable mapping structure for the separate output of large numbers
 */
void lnum_map_init(size_t size);

/**
 * Adds the given pointer to complex number (and how much it should be shifted before output) to the map.
 * Returns the variable (index to both arrays).
 */
uint64_t lnum_map_add(coef_t *n_ptr);

/**
 * Return true if the mapping structure for the separate output of large numbers is empty
 */
bool lnum_map_is_empty();

/**
 * For separate output of the mapping in the specified file
 */
void lnum_map_print(FILE *f);

/**
 * Clears and deallocates the arrays in the mapping structure (no delete function as the map is on stack)
 */
void lnum_map_clear();


#endif
/* end of "mtbdd_out.h" */