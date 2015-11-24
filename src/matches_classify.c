/*
 * PROJECT: GEMMapper
 * FILE: matches_metrics.c
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION: TODO
 */

#include "matches_classify.h"
#include "matches_cigar.h"

/*
 * Matches Classes
 */
const char* matches_class_label[] =
{
    [0] = "unmapped",
    [1] = "tie-indistinguishable",
    [2] = "tie-swg-score",
    [3] = "tie-edit-distance",
    [4] = "tie-event-distance",
    [5] = "mmap",
    [6] = "unique"
};

/*
 * Classify
 */
GEM_INLINE void matches_classify_compute_predictors_unmapped(
    matches_predictors_t* const predictors,matches_metrics_t* const metrics,
    const uint64_t read_length,const uint64_t max_region_length,
    const uint64_t proper_length,const uint64_t max_complete_stratum,
    const uint64_t num_zero_regions) {
  // Compute event distances
  predictors->first_map_event_distance = read_length;
  predictors->subdominant_event_distance = read_length;
  predictors->first_map_edit_distance = read_length;
  predictors->subdominant_edit_distance = read_length;
  predictors->first_map_swg_score = 0;
  predictors->subdominant_swg_score = 0;
  // Normalize event distances
  predictors->first_map_event_distance_norm = 0.0;
  predictors->subdominant_event_distance_norm = 0.0;
  // Normalize edit distances
  predictors->first_map_edit_distance_norm = 0.0;
  predictors->subdominant_edit_distance_norm = 0.0;
  // Normalize SWG scores
  predictors->first_map_swg_score_norm =  0.0;
  predictors->subdominant_swg_score_norm =  0.0;
  // Max region length
  predictors->max_region_length = max_region_length;
  predictors->max_region_length_norm = (double)max_region_length/(double)proper_length;
  // Maximum complete stratum
  predictors->mcs = max_complete_stratum-num_zero_regions;
}
GEM_INLINE void matches_classify_compute_predictors_mapped(
    matches_predictors_t* const predictors,matches_metrics_t* const metrics,
    const uint64_t primary_map_distance,const uint64_t primary_map_edit_distance,
    const int32_t primary_map_swg_score,const swg_penalties_t* const swg_penalties,
    const uint64_t read_length,const uint64_t max_region_length,
    const uint64_t proper_length,const uint64_t max_complete_stratum,
    const uint64_t num_zero_regions) {
  // Compute primary/subdominant event distance
  predictors->first_map_event_distance = primary_map_distance;
  if (primary_map_distance!=metrics->min1_counter_value) {
    predictors->subdominant_event_distance = metrics->min1_counter_value;
  } else {
    predictors->subdominant_event_distance = metrics->min2_counter_value;
  }
  if (predictors->subdominant_event_distance==UINT32_MAX) {
    predictors->subdominant_event_distance = read_length;
  }
  // Compute primary/subdominant edit distance
  predictors->first_map_edit_distance = primary_map_edit_distance;
  if (primary_map_edit_distance!=metrics->min1_edit_distance) {
    predictors->subdominant_edit_distance = metrics->min1_edit_distance;
  } else {
    predictors->subdominant_edit_distance = metrics->min2_edit_distance;
  }
  if (predictors->subdominant_edit_distance==UINT32_MAX) {
    predictors->subdominant_edit_distance = read_length;
  }
  // Compute primary/subdominant SWG score
  predictors->first_map_swg_score = primary_map_swg_score;
  if (predictors->first_map_swg_score < 0) predictors->first_map_swg_score = 0;
  if (primary_map_swg_score!=metrics->max1_swg_score) {
    predictors->subdominant_swg_score = metrics->max1_swg_score;
  } else {
    predictors->subdominant_swg_score = metrics->max2_swg_score;
  }
  if (predictors->subdominant_swg_score < 0) predictors->subdominant_swg_score = 0;
  // Normalize event distances
  predictors->first_map_event_distance_norm =
      ((double)(read_length-predictors->first_map_event_distance))/((double)read_length);
  predictors->subdominant_event_distance_norm =
      ((double)(read_length-predictors->subdominant_event_distance))/((double)read_length);
  // Normalize edit distances
  predictors->first_map_edit_distance_norm =
      ((double)(read_length-predictors->first_map_edit_distance))/((double)read_length);
  predictors->subdominant_edit_distance_norm =
      ((double)(read_length-predictors->subdominant_edit_distance))/((double)read_length);
  // Normalize SWG scores
  const double swg_norm_factor = swg_penalties->generic_match_score*read_length;
  predictors->first_map_swg_score_norm = (double)predictors->first_map_swg_score/swg_norm_factor;
  predictors->subdominant_swg_score_norm = (double)predictors->subdominant_swg_score/swg_norm_factor;
  // Max region length
  predictors->max_region_length = max_region_length;
  predictors->max_region_length_norm = (double)max_region_length/(double)proper_length;
  // Maximum complete stratum
  predictors->mcs = max_complete_stratum-num_zero_regions;
}
/*
 * SE Classify
 */
GEM_INLINE matches_class_t matches_classify(matches_t* const matches) {
  // Parameters
  matches_metrics_t* const metrics = &matches->metrics;
  match_trace_t* const match = matches_get_match_trace_buffer(matches);
  // Classify
  if (metrics->total_matches_sampled == 0) {
    return matches_class_unmapped;
  } else if (metrics->total_matches_sampled == 1) {
    return matches_class_unique;
  } if (matches_cigar_cmp(matches->cigar_vector,match,matches->cigar_vector,match+1)==0) {
    return matches_class_tie_indistinguishable;
  } if (metrics->max1_swg_score == metrics->max2_swg_score) {
    return matches_class_tie_swg_score;
  } else if (metrics->min1_edit_distance == metrics->min2_edit_distance) {
    return matches_class_tie_edit_distance;
  } else if (metrics->min1_counter_value == metrics->min2_counter_value ||    // delta-0
             metrics->min1_counter_value+1 == metrics->min2_counter_value) {  // delta-1
    return matches_class_tie_event_distance;
  } else {
    return matches_class_mmap;
  }
}
GEM_INLINE void matches_classify_compute_predictors(
    matches_t* const matches,matches_predictors_t* const predictors,
    const swg_penalties_t* const swg_penalties,const uint64_t read_length,
    const uint64_t max_region_length,uint64_t const proper_length,
    uint64_t const overriding_mcs,const uint64_t num_zero_regions) {
  const uint64_t mcs = (overriding_mcs!=UINT64_MAX) ? overriding_mcs : matches->max_complete_stratum;
  const uint64_t num_matches = matches_get_num_match_traces(matches);
  if (num_matches==0) {
    matches_classify_compute_predictors_unmapped(predictors,&matches->metrics,
        read_length,max_region_length,proper_length,mcs,num_zero_regions);
    // First/Subdominant strata
    predictors->first_stratum_matches = 0;
    predictors->subdominant_stratum_matches = 0;
  } else {
    match_trace_t* const match = matches_get_match_trace_buffer(matches);
    matches_classify_compute_predictors_mapped(
        predictors,&matches->metrics,match->distance,match->edit_distance,match->swg_score,
        swg_penalties,read_length,max_region_length,proper_length,mcs,num_zero_regions);
    // First/Subdominant strata
    predictors->first_stratum_matches = matches_get_first_stratum_matches(matches);
    predictors->subdominant_stratum_matches = matches_get_subdominant_stratum_matches(matches);
  }
  // Subdominant candidates
  predictors->subdominant_candidates_end1 = matches->metrics.subdominant_candidates;
}
GEM_INLINE double matches_classify_unique(matches_predictors_t* const predictors) {
  // Unique: Probability of the first position-match (primary match) of being a true positive
  const double lr_factor = -222.2971 +
      (double)predictors->first_map_event_distance_norm * 207.3658 +
      (double)predictors->first_map_swg_score_norm * 17.8461 +
      (double)predictors->mcs * 4.7404 +
      (double)predictors->max_region_length_norm * -0.9996 +
      (double)predictors->subdominant_candidates_end1 * 0.2964;
  return 1.0 / (1.0 + (1.0/exp(lr_factor)));
}
GEM_INLINE double matches_classify_mmaps(matches_predictors_t* const predictors) {
  // Classify MMaps wrt the probability of the first position-match (primary match) of being a true positive
  const double lr_factor = -209.7885 +
      (double)predictors->first_map_event_distance_norm * 191.0935 +
      (double)predictors->subdominant_event_distance_norm * 2.7940 +
      (double)predictors->first_map_swg_score_norm * 23.3503 +
      (double)predictors->subdominant_swg_score_norm * -6.2635 +
      (double)predictors->mcs * 3.9139 +
      (double)predictors->max_region_length_norm * -0.5801 +
      (double)predictors->subdominant_candidates_end1 * 0.1275;
  return 1.0 / (1.0 + (1.0/exp(lr_factor)));
}
GEM_INLINE double matches_classify_ties(matches_predictors_t* const predictors) {
  // Classify ties wrt the probability of being a true positive
  const double lr_factor = -158.99528 +
      (double)predictors->first_map_edit_distance_norm * 72.02521 +
      (double)predictors->subdominant_edit_distance_norm * -62.02260 +
      (double)predictors->first_map_event_distance_norm * 229.73507 +
      (double)predictors->subdominant_event_distance_norm * -85.95881 +
      (double)predictors->first_map_swg_score_norm * 22.06638 +
      (double)predictors->subdominant_swg_score_norm * -18.74157 +
      (double)predictors->mcs * 1.66582 +
      (double)predictors->subdominant_stratum_matches * 0.01094 +
      (double)predictors->subdominant_candidates_end1 * 0.03268;
  return 1.0 / (1.0 + (1.0/exp(lr_factor)));
}