/*
 * PROJECT: GEMMapper
 * FILE: buffered_output_file.h
 * DATE: 06/06/2012
 * AUTHOR(S): Santiago Marco-Sola <santiagomsola@gmail.com>
 * DESCRIPTION: // TODO
 */

#ifndef BUFFERED_OUTPUT_FILE_H_
#define BUFFERED_OUTPUT_FILE_H_

#include "essentials.h"
#include "output_buffer.h"
#include "output_file.h"

typedef struct {
  /* Output file */
  output_file_t* output_file;
  /* Output Buffer */
  output_buffer_t* buffer;
} buffered_output_file_t;

// Codes status
#define BUFFERED_OUTPUT_FILE_OK 0
#define BUFFERED_OUTPUT_FILE_FAIL -1

/*
 * Checkers
 */
#define BUFFERED_OUTPUT_FILE_CHECK(buffered_output_file) \
  GEM_CHECK_NULL(buffered_output_file); \
  OUTPUT_FILE_CHECK((buffered_output_file)->output_file)

/*
 * Setup
 */
buffered_output_file_t* buffered_output_file_new(output_file_t* const output_file);
void buffered_output_file_close(buffered_output_file_t* const buffered_output);

/*
 * Utils
 */
void buffered_output_file_request_buffer(
    buffered_output_file_t* const buffered_output,const uint32_t block_id);
void buffered_output_file_dump_buffer(buffered_output_file_t* const buffered_output);
void buffered_output_file_safety_dump_buffer(buffered_output_file_t* const buffered_output);
void buffered_output_file_reserve(buffered_output_file_t* const buffered_output,const uint64_t num_chars);

/*
 * Fast-printer
 */
void bofprintf_uint64(buffered_output_file_t* const buffered_output,const uint64_t number);
void bofprintf_int64(buffered_output_file_t* const buffered_output,const int64_t number);
void bofprintf_char(buffered_output_file_t* const buffered_output,const char character);
void bofprintf_string(
    buffered_output_file_t* const buffered_output,const int string_length,const char* const string);
#define bofprintf_string_literal(buffered_output,string) bofprintf_string(buffered_output,sizeof(string)-1,string)

/*
 * Error Messages
 */
#define GEM_ERROR_BUFFER_SAFETY_DUMP "Output buffer. Could not perform safety dump (no buffer extension)"
#define GEM_ERROR_BUFFER_RESERVE "Output buffer. Could not reserve buffer-memory for dump"


#endif /* BUFFERED_OUTPUT_FILE_H_ */
