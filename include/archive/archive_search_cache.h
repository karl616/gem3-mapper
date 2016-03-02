/*
 * PROJECT: GEMMapper
 * FILE: archive_search_cache.h
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION: // TODO
 */

#ifndef ARCHIVE_SEARCH_CACHE_H_
#define ARCHIVE_SEARCH_CACHE_H_

#include "utils/essentials.h"
#include "archive/archive_search.h"

/*
 * Archive Search Cache
 */
typedef struct {
  /* Archive-Search Cache */
  vector_t* archive_search_cache;         // Already allocated & configured Slab (archive_search_t*)
  /* Archive & Parameters */
  archive_t* archive;                     // Archive
  search_parameters_t* search_parameters; // Search Parameters
} archive_search_cache_t;

/*
 * Setup
 */
archive_search_cache_t* archive_search_cache_new(
    archive_t* const archive,
    search_parameters_t* const search_parameters);
void archive_search_cache_delete(archive_search_cache_t* const archive_search_cache);

/*
 * Allocate/Free
 */
archive_search_t* archive_search_cache_alloc(archive_search_cache_t* const archive_search_cache);
void archive_search_cache_free(
    archive_search_cache_t* const archive_search_cache,
    archive_search_t* const archive_search);

#endif /* ARCHIVE_SEARCH_CACHE_H_ */