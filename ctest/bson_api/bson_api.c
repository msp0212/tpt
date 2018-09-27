/** @file bson_api.c
 * @brief This file implements the API exposed by the library
 * @date 11 June 2015
 */
/*System Headers*/
#include <stdio.h>
#define _XOPEN_SOURCE 501
#include <string.h>
/*Local Headers*/
#include <bson_api.h>
#include <bson_errors.h>

#define BSON_ARRAY_SIZE 4


static int read_and_alloc_bson_array(bson_reader_t *reader,
					bool *eof, off_t *parsed_len,
					bson_t ***doc_arr, size_t *arr_len);
static int bson_buffer_write(bson_writer_t *writer, bson_t **doc_arr, size_t arr_size);
static int get_parsed_len(bson_reader_t *reader, bool eof, off_t *parsed_len);
static int bson_doc_copy_by_iter(bson_t *dst , bson_t *src);
static int bson_fill_element_array(bson_element_t *earr, bson_t *bdoc);

/**@brief set a bson value as 32 bit integer
 * @param[in] val pointer to bson value
 * @param[in] i32 32 bit integer value
 * @return On Success 0,
 *	   On Error <0, values indicates the error code
 * */
int bson_value_set_int32(bson_value_t *val, uint32_t i32)
{
	int ret = BSON_SUCCESS;

	if (val == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	val->value_type = BSON_TYPE_INT32;
	val->value.v_int32 = i32;
exit:
	return ret;
}

/**@brief set a bson value as 64 bit integer
 * @param[in] val pointer to bson value
 * @param[in] i64 64 bit integer value
 * @return On Success 0,
 *	   On Error <0, value indicates the error code
 * */
int bson_value_set_int64(bson_value_t *val, uint64_t i64)
{
	int ret = BSON_SUCCESS;

	if (val == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	val->value_type = BSON_TYPE_INT64;
	val->value.v_int64 = i64;
exit:
	return ret;
}

/**@brief set a bson value as double
 * @param[in] val pointer to bson value
 * @param[in] d double value
 * @return On Success 0,
 *	   On Error <0, values indicates the error code
 * */
int bson_value_set_double(bson_value_t *val, double d)
{
	int ret = BSON_SUCCESS;
	if (val == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	val->value_type = BSON_TYPE_DOUBLE;
	val->value.v_double = d;
exit:
	return ret;
}

/**@brief set a bson value as char string.
 * @details Since this function dynamically allocates memory for value 
 *		for bson from str parameter, dont forget to free the memory 
 *					using bson_value_free
 * @param[in] val pointer to bson value
 * @param[in] str character string
 * @return On Success 0,
 *	   On Error <0, value indicates the error code
 * */
int bson_value_set_str(bson_value_t *val, char *str)
{
	int ret = BSON_SUCCESS;

	if (val == NULL || str == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	val->value_type = BSON_TYPE_UTF8;
	val->value.v_utf8.len =  strlen(str);
	val->value.v_utf8.str = strdup(str);
exit:
	return ret;
}

/**@brief set a bson value as an array of bson values
 * @details Since this function dynamically allocates memory for bson value 
 *		from arr_val parameter,	dont forget to free the memory using
 *				bson_value_free
 * @param[in] val pointer to bson value
 * @param[in] array_vals array of bson values
 * @return On Success 0,
 *	   On Error <0, value indicates the error code
 * */
int bson_value_set_array(bson_value_t *val, 
				bson_value_t *arr_vals, int arr_len)
{
	int ret = BSON_SUCCESS;
	int i = 0;
	bson_t *doc = NULL;
	char str[16];
	const char *key = NULL;
	uint8_t *buf = NULL;
	size_t len = 0;

	if (val == NULL || arr_vals == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((doc = bson_new()) == NULL) {
		ret = BSON_ERR_CREAT_NEW;
		goto exit;
	}
	for (i = 0; i < arr_len; i++) {
		bson_uint32_to_string(i, &key, str, sizeof(str));
		if (!bson_append_value(doc, key, -1, arr_vals + i)) {
			ret = BSON_ERR_APPEND_VAL;
			bson_destroy(doc);
			goto exit;
		}
	}
	if ((ret = bson_write_to_buffer(&buf, &len, &doc, 1)) < 0) {
		bson_destroy(doc);
		goto exit;
	}
	val->value_type = BSON_TYPE_ARRAY;
	val->value.v_doc.data = buf;
	val->value.v_doc.data_len = len;
	bson_destroy(doc);
exit:
	return ret;
}

/**@brief set a bson value as bson document
 * @details Since this function dynamically allocates memory for bson value 
 *	           from arr_val parameter, dont forget to free the memory using
 *                              bson_value_free
 * @param[in] val a bson value
 * @param[in] doc a bson document
 * @return On success, 0
 *	   On error <0, value indicates error code
 * */
int bson_value_set_doc(bson_value_t *val, bson_t *doc)
{
	int ret = BSON_SUCCESS;
	uint8_t *buf = NULL;
	size_t len = 0;

	if (val == NULL || doc == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((ret = bson_write_to_buffer(&buf, &len, &doc, 1)) < 0) {
		goto exit;
	}
	val->value_type = BSON_TYPE_DOCUMENT;
	val->value.v_doc.data = buf;
	val->value.v_doc.data_len = len;
exit: 
	return ret;
}
/**@brief get an array of bson values from a bson value 
 *				having an array of bson values
 * @param[in] val a bson value
 * @param[out] arr_vals an array of bson values
 * @param[out] arr_len length of the array
 * @return On success 0,
 *	   On Error <0, value indicates the error code
 * */
int bson_value_get_array(bson_value_t *val, 
				bson_value_t **arr_vals, int *arr_len)
{
	int ret = BSON_SUCCESS;
	bson_t *doc = NULL;
	bson_iter_t iter;	
	int count = 0;

	if (val == NULL || arr_vals == NULL || arr_len == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((doc = bson_new_from_data(val->value.v_doc.data,
					val->value.v_doc.data_len)) == NULL) {
		ret = BSON_ERR_CREAT_NEW;
		goto exit;
	}
	if ((*arr_len = bson_count_keys(doc)) == 0) {
		bson_destroy(doc);
		goto exit;
	}
	if ((*arr_vals = malloc(*arr_len * sizeof(**arr_vals))) == NULL) {
		ret = BSON_ERR_MEM_ALLOC;
		bson_destroy(doc);
		goto exit;
	}
	if (!bson_iter_init(&iter, doc)) {
		ret = BSON_ERR_ITER_INIT;
		bson_destroy(doc);
		free(*arr_vals); 
		*arr_vals = NULL;
		goto exit;
	}
	while (bson_iter_next(&iter)) {
		bson_value_copy(bson_iter_value(&iter), *arr_vals + count);
		count++;
	}
	bson_destroy(doc);
exit:
	return ret;
}
/**@brief get the bson doc from a bson value
 * @param[in] val a bson value
 * @param[out] bdoc a bson document
 * @return On success 0,
 *	   On error <0, value indicates the error code
 * */
int bson_value_get_doc(bson_value_t *val, bson_t **bdoc)
{
	int ret = BSON_SUCCESS;
	if (val == NULL || bdoc == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((*bdoc = bson_new_from_data(val->value.v_doc.data,
					val->value.v_doc.data_len)) == NULL) {
		ret = BSON_ERR_CREAT_NEW;
		goto exit;
	}
exit:
	return ret;
}
/**@brief free memory associated with a bson value
 * @param[in] val A bson value
 * */
void bson_value_free(bson_value_t *val)
{
	bson_value_destroy(val);
}

/**@brief Create a bson document
 * @param[in] earr An array of bson elements
 * @param[in] ecount Count of array elements
 * @param[out] bdoc value at this address points to newly created bson doc
 * @return On Success 0,
 *	   On Error <0, values indicates the error code
 * */
int bson_doc_create(bson_element_t *earr, int ecount, bson_t **bdoc)
{
	int ret = BSON_SUCCESS;
	int i = 0;

	if (earr == NULL || bdoc == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	*bdoc = bson_new();
	if (bdoc == NULL) {
		ret = BSON_ERR_CREAT_NEW;
		goto exit;
	}
	for (i = 0; i < ecount; i++) {
		if (!bson_append_value(*bdoc, earr[i].key, -1, &earr[i].val)) {
			ret = BSON_ERR_APPEND_VAL;
			bson_destroy(*bdoc);
			goto exit;
		}
	}
exit:
	return ret;
}

/**@brief get bson elements from a bson doc
 * @param[in] bdoc pointer to a bson document
 * @param[out] earr value at this address will point to a newly allocated 
 *			bson element array
 * @param[out] ecount value at this address is set to length of 
 *						bson element array
 * */
int bson_doc_get_elements(bson_t *bdoc, bson_element_t **earr, int *ecount)
{
	int ret = BSON_SUCCESS;

	if (bdoc == NULL || earr == NULL || ecount == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((*ecount = bson_count_keys(bdoc)) == 0) {
		goto exit;
	}
	if ((*earr = calloc(*ecount, sizeof(**earr))) == NULL ) {
		ret = BSON_ERR_MEM_ALLOC;
		goto exit;
	}
	if ((ret = bson_fill_element_array(*earr, bdoc)) < 0) {
		goto exit;
	}
exit:
	return ret;
}

/**@brief Writes a list of bson docs serially to a buffer
 * @param[out] buf value at this address points to a buffer
 *				containing the bson docs
 * @param[out] buf_len value at this address is set to the length of the buffer
 * @param[in] doc_arr An array of bson docs
 * @param[in] arr_size length of array of bson docs
 * @return On Success 0,
 *	   On Error <0, values indicates the error code
 * */

int bson_write_to_buffer(uint8_t **buf, size_t *buf_len,
                                        bson_t **doc_arr, size_t arr_size)
{
	int ret = BSON_SUCCESS;
	bson_writer_t *writer = NULL;
	uint8_t *tmp = NULL;

	if (buf == NULL || buf_len == NULL || doc_arr == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if((writer = bson_writer_new(buf, buf_len, 0, 
					bson_realloc_ctx, NULL)) == NULL) {
		ret = BSON_ERR_WRITER_NEW;
		goto exit;
	}
	if ((ret = bson_buffer_write(writer, doc_arr, arr_size)) < 0) {
		free(*buf); *buf = NULL;
		bson_writer_destroy(writer);
		goto exit;
	}
	if ((tmp = realloc(*buf,
				(*buf_len =
				 bson_writer_get_length(writer)))) == NULL ) {
		 free(*buf); *buf = NULL;
		 bson_writer_destroy(writer);
		 ret = BSON_ERR_MEM_ALLOC;
		 goto exit;
	} else {
		*buf = tmp;
	}
	bson_writer_destroy(writer);
exit:
	return ret;
}

/**@brief Reads a list of bson docs serially from a buffer
 * @param[in] buf Buffer containing the bson docs
 * @param[in] buf_len Length of the buffer
 * @param[out] parsed_len length of the parsed part of buffer
 * @param[out] doc_arr Value at this address points to an array(double ptr)
 *			of bson docs
 * @param[out] arr_len value at this address points to length of array of bson docs
 * @return On Success 0,
 *	   On Error <0, values indicates the error code
 * */

int bson_read_from_buffer(uint8_t *buf, size_t buf_len, off_t *parsed_len, 
                                        bson_t ***doc_arr, size_t *arr_len)
{
	int ret = BSON_SUCCESS;
	bson_reader_t *reader = NULL;
	bool eof = false;
	bson_t **tmp = NULL;
	int i = 0;

	if (buf == NULL || parsed_len == NULL || doc_arr == NULL ||
							arr_len == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if (buf_len < 0) {
		ret = BSON_ERR_INVALID_BUFLEN;
		goto exit;
	}
	if ((reader = bson_reader_new_from_data(buf, buf_len)) == NULL) {
		ret = BSON_ERR_READER_NEW;
		goto exit;
	}
	if ((ret = read_and_alloc_bson_array(reader, &eof, parsed_len, doc_arr, arr_len)) < 0) {
		bson_reader_destroy(reader);
		goto exit;
	}
	if ((ret = get_parsed_len(reader, eof, parsed_len)) < 0) {
		for (i = 0; i < *arr_len; i++) {
			free((*doc_arr)[i]); (*doc_arr)[i] = NULL;
		}
		free(*doc_arr); *doc_arr = NULL;
		bson_reader_destroy(reader);
		goto exit;
	}
	if ((tmp = realloc(*doc_arr, *arr_len * sizeof(**doc_arr))) == NULL 
								&& *arr_len) {
		ret = BSON_ERR_MEM_ALLOC;
		for (i = 0; i < *arr_len; i++) {
			free((*doc_arr)[i]); (*doc_arr)[i] = NULL;
		}
		free(*doc_arr); *doc_arr = NULL;
		bson_reader_destroy(reader);
		goto exit;
	} else {
		*doc_arr = tmp;
	}
	bson_reader_destroy(reader);
exit :
	return ret;
}

/**@brief destroy a bson doc
 * @param[in] bdoc Pointer to a bson doc
 * */
void bson_doc_destroy(bson_t *bdoc)
{
	if (bdoc != NULL) {
		bson_destroy(bdoc);
	}
	return;
}
/**@brief convert a bson doc to an json string
 * @param[in] bson Pointer to a bson doc
 * @param[out] json Value at this address points to json string
 * @param[out] len Value at this address points to length of json string
 * @return On Success 0,
 *	   On Error <0, value indicates error code
 * */
int bson_to_json(bson_t *bson, char **json, size_t *len)
{
	int ret = BSON_SUCCESS;

	if (bson == NULL || json == NULL) {
		ret = BSON_ERR_NULL_PARAMS;
		goto exit;
	}
	if ((*json = bson_as_json(bson, len)) == NULL) {
		ret = BSON_ERR_CONVERT_JSON;
	}
exit: 
	return ret;
}
/**@brief Get error message for an error code
 * @param[in] code error code returned by a bson api
 * @return On Success error msg for the error code,
 *	   On Error NULL
 * */

char *bson_err_msg(int code) 
{
	switch (code) {
	CASE(BSON_ERR_NULL_PARAMS);
	CASE(BSON_ERR_MEM_ALLOC);
	CASE(BSON_ERR_CONTEXT_NEW);
	CASE(BSON_ERR_CREAT_NEW);
	CASE(BSON_ERR_CONVERT_JSON);
	CASE(BSON_ERR_APPEND_VAL);
	CASE(BSON_ERR_WRITER_NEW);
	CASE(BSON_ERR_WRITER_BEGIN);
	CASE(BSON_ERR_INVALID_BUFLEN);
	CASE(BSON_ERR_READER_NEW);
	CASE(BSON_ERR_READER_TELL);
	CASE(BSON_ERR_READER_READ);
	CASE(BSON_ERR_ITER_INIT);
	default:
		return NULL;
	}
}

static int bson_buffer_write(bson_writer_t *writer, bson_t **doc_arr, size_t arr_size)
{
	int i = 0;
	int ret = BSON_SUCCESS;
	bson_t *doc = NULL;

	for (i = 0; i < arr_size; i++) {
		if (!bson_writer_begin(writer, &doc)) {
			ret = BSON_ERR_WRITER_BEGIN;
			goto exit;
		}
		if ((ret = bson_doc_copy_by_iter(doc, doc_arr[i])) < 0) {
			bson_writer_end(writer);
			goto exit;
		}
		bson_writer_end(writer);
	}
exit:
	return ret;
}

static int bson_doc_copy_by_iter(bson_t *dst , bson_t *src)
{
	int ret = BSON_SUCCESS;
	bson_iter_t iter;
	const char *key = NULL;
	const bson_value_t *val = NULL;

	if (!bson_iter_init(&iter, src)) {
		ret = BSON_ERR_ITER_INIT;
		goto exit;
	}
	while (bson_iter_next(&iter)) {
		key = bson_iter_key(&iter);
		val = bson_iter_value(&iter);
		if (!bson_append_value(dst, key, -1, val)) {
			ret = BSON_ERR_APPEND_VAL;
			goto exit;
		}
	}
exit:
	return ret;
}

static int bson_fill_element_array(bson_element_t *earr, bson_t *bdoc)
{
	bson_iter_t iter;	
	int count = 0;
	int ret = BSON_SUCCESS;

	if (!bson_iter_init(&iter, bdoc)) {
		ret = BSON_ERR_ITER_INIT;
		goto exit;
	}
	while (bson_iter_next(&iter)) {
		earr[count].key = strdup(bson_iter_key(&iter));
		bson_value_copy(bson_iter_value(&iter), &earr[count].val);
		count++;
	}
exit:
	return ret;
}

static int read_and_alloc_bson_array(bson_reader_t *reader,
					bool *eof, off_t *parsed_len, 
					bson_t ***doc_arr, size_t *arr_len)
{
	int ret = BSON_SUCCESS;
	const bson_t *doc = NULL;
	int arr_size = BSON_ARRAY_SIZE;
	bson_t **elem;
	bson_t **tmp;
	int i = 0;

	*arr_len = 0;
	if ((*doc_arr = malloc(arr_size * sizeof(**doc_arr))) == NULL ) {
		ret = BSON_ERR_MEM_ALLOC;
		goto exit;
	}
	while ((doc = bson_reader_read(reader, eof)) != NULL) {
		elem = *doc_arr + *arr_len;
		*elem = bson_copy(doc);
		(*arr_len)++;
		if (*arr_len >= arr_size) {
			arr_size  *= 2;
			if ((tmp = 
				realloc(*doc_arr, 
				   arr_size * sizeof(**doc_arr))) == NULL) {
				*doc_arr = tmp;
				for (i = 0; i < *arr_len; i++) {
					free((*doc_arr)[i]);
					(*doc_arr)[i] = NULL;
				}
				free(*doc_arr); *doc_arr = NULL;
				ret = BSON_ERR_MEM_ALLOC;
				goto exit;
			}
			else {
				*doc_arr = tmp;
				elem = *doc_arr + *arr_len;
			}
		}
	}
exit:
	return ret;
}

static int get_parsed_len(bson_reader_t *reader, bool eof, off_t *parsed_len)
{
	int ret = BSON_SUCCESS;
	if (eof) {
		if ((*parsed_len = bson_reader_tell(reader)) < 0) {
			ret = BSON_ERR_READER_TELL;
			goto exit;
		}
	} else {
		ret = BSON_ERR_READER_READ;
	}
exit:
	return ret;
}

