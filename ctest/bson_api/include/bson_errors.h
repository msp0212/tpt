/** @file bson_errors.h
 * @brief This header file defines the error code used by library
 * */
#ifndef BSON_ERRROS_H
#define BSON_ERRORS_H

#define MIN_ERR_CODE -2100

/** @enum bson_api_err
 *  @brief List of bson api error codes
 * */
typedef enum bson_api_err {
	BSON_SUCCESS = 0,
	BSON_ERR_NULL_PARAMS = MIN_ERR_CODE,
	BSON_ERR_CONTEXT_NEW,
	BSON_ERR_MEM_ALLOC,
	BSON_ERR_CREAT_NEW,
	BSON_ERR_CONVERT_JSON,
	BSON_ERR_APPEND_VAL,
	BSON_ERR_WRITER_NEW,
	BSON_ERR_WRITER_BEGIN,
	BSON_ERR_INVALID_BUFLEN,
	BSON_ERR_READER_NEW,
	BSON_ERR_READER_TELL,
	BSON_ERR_READER_READ,
	BSON_ERR_ITER_INIT
}bson_api_err_t;

#define BSON_ERR_STR(code)  #code

/** @def CASE(code)
 * Macro to return the error string for an error code
 * */
#define CASE(code) case code : return BSON_ERR_STR(code)
#endif
