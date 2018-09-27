/** @file bson_api.h
 *  @brief Header file to be included by user of the library.
 */
#ifndef BSON_API_H
#define BSON_API_H
#include <bson.h>

/** @struct bson_element
 * @brief structure describing a bson element with 
 *			key and value for the element
 * @details bson_value_t is defined in bson.h.For structure details refer 
 *		online documentation of libbson.
 * */
typedef struct bson_element {
	char *key; /**<bson element key*/
	bson_value_t val; /**<bson element value*/
}bson_element_t;


int bson_value_set_int32(bson_value_t *val, uint32_t i32);
int bson_value_set_int64(bson_value_t *val, uint64_t i64);
int bson_value_set_double(bson_value_t *val, double d);
int bson_value_set_str(bson_value_t *val, char *str);
int bson_value_set_array(bson_value_t *val, 
                                bson_value_t *arr_vals, int arr_len);
int bson_value_set_doc(bson_value_t *val, bson_t *doc);

int bson_value_get_array(bson_value_t *val, 
				bson_value_t **arr_vals, int *arr_len);
int bson_value_get_doc(bson_value_t *val, bson_t **bdoc);

void bson_value_free(bson_value_t *val);
int bson_doc_create(bson_element_t *earr, int ecount, bson_t **bdoc);
int bson_doc_get_elements(bson_t *bdoc, bson_element_t **earr, int *ecount);
int bson_write_to_buffer(uint8_t **buf, size_t *buf_len,
					bson_t **doc_arr, size_t arr_size);
int bson_read_from_buffer(uint8_t *buf, size_t buf_len, off_t *parsed_len, 
					bson_t ***doc_arr, size_t *arr_len);
void bson_doc_destroy(bson_t *bdoc);
char *bson_err_msg(int code);
int bson_to_json(bson_t *bson, char **json, size_t *len);
#endif
