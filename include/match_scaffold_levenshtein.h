/*
 * PROJECT: GEMMapper
 * FILE: match_scaffold_levenshtein.h
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION:
 */

#ifndef MATCH_SCAFFOLD_LEVENSHTEIN_H_
#define MATCH_SCAFFOLD_LEVENSHTEIN_H_

#include "essentials.h"
#include "match_scaffold.h"
#include "match_align_dto.h"
#include "matches.h"

/*
 * Compute Levenshtein Alignment (for scaffolding)
 */
void match_scaffold_levenshtein_align(
    match_scaffold_t* const match_scaffold,match_align_input_t* const align_input,
    const uint64_t max_error,const bool left_gap_alignment,
    matches_t* const matches,mm_stack_t* const mm_stack);

/*
 * Levenshtein Scaffold Tiled
 */
bool match_scaffold_levenshtein(
    match_scaffold_t* const match_scaffold,match_align_input_t* const align_input,
    match_align_parameters_t* const align_parameters,matches_t* const matches,
    mm_stack_t* const mm_stack);


#endif /* MATCH_SCAFFOLD_LEVENSHTEIN_H_ */
