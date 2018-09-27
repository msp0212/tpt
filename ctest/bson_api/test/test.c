#include <stdio.h>
#include <bson_api.h>

#define NUM 2

void test1(void);
void test2(void);
void test3(void);
void test4(void);
void test5(void);
void test6(void);
void print_bson_elements(bson_t *bdoc);
void print_bson_value(bson_value_t *val);

int main()
{
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	return 0;
}

void print_bson_value(bson_value_t *val)
{
	int ret = 0;
	bson_value_t *vals = NULL;
	int vcount = 0;
	int i = 0;
	bson_t *bdoc = NULL;

	switch (val->value_type) {
	case BSON_TYPE_INT32:
		printf("int32 val = %"PRId32"\n", val->value.v_int32);
		break;
	case BSON_TYPE_INT64:
		printf("int64 val = %"PRId64"\n", 
				val->value.v_int64);
		break;
	case BSON_TYPE_DOUBLE:
		printf("double val = %f\n", 
				val->value.v_double);
		break;
	case BSON_TYPE_UTF8:
		printf("str val = %s\n", 
				val->value.v_utf8.str);
		break;
	case BSON_TYPE_ARRAY:
		printf("arr vals : \n");
		if ((ret = bson_value_get_array(val, &vals, &vcount)) < 0) {
			printf("\tError %d %s in bson_value_get_array", 
					ret, bson_err_msg(ret));
		}
		for (i = 0; i < vcount; i++) {
			print_bson_value(vals + i);
			bson_value_free(vals + i);
		}
		free(vals);
		break;
	case BSON_TYPE_DOCUMENT:
		printf("document : \n");
		if ((ret = bson_value_get_doc(val, &bdoc)) < 0) {
			printf("Error %d %s in bson_value_get_doc\n",
					ret, bson_err_msg(ret));
		}
		print_bson_elements(bdoc);
		bson_doc_destroy(bdoc);
	default:
		break;
	}
}

void print_bson_elements(bson_t *bdoc)
{
	bson_element_t *earr = NULL;
	int ecount = 0;
	int ret = 0;
	int i = 0;

	if ((ret =bson_doc_get_elements(bdoc, &earr, &ecount) < 0)) {
		printf("Error %d %s in bson_doc_get_elements",
						ret, bson_err_msg(ret));
		goto exit;
	}
	for (i = 0; i < ecount; i++) {
		printf("%d : ", i);
		printf("key = %s\t", earr[i].key);
		print_bson_value(&earr[i].val);
		free(earr[i].key);
		bson_value_free(&earr[i].val);
	}
	free(earr);
exit:
	return;
}

/*
 * creating a simple bson doc*/
void test1(void)
{
	bson_element_t earr[4];
	bson_t *bdoc = NULL;
	char *json = NULL;
	int ret = 0;

	/* Doc to be creted is :
	 * {
	 *  "key_int32" : 111,
	 *  "key_int64" : 222, 
	 *  "key_double" : 3.141590, 
	 *  "key_string" : "Mohit Singh"
	 * } 
	 * */

	earr[0].key = "key_int32";
	bson_value_set_int32(&earr[0].val, 111);
	earr[1].key = "key_int64";
	bson_value_set_int64(&earr[1].val, 222);
	earr[2].key = "key_double";
	bson_value_set_double(&earr[2].val, 3.141590);
	earr[3].key = "key_string";
	bson_value_set_str(&earr[3].val, "Mohit Singh");

	if ((ret = bson_doc_create(earr, 4, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test1 FAILED !\n");
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test1 FAILED !\n");
		bson_doc_destroy(bdoc);
		goto exit;
	}
	bson_value_free(&earr[3].val);
	printf("json : %s\n", json);
	free(json); json = NULL;

	print_bson_elements(bdoc);

	printf("Test1 PASSED\n\n");

	bson_doc_destroy(bdoc);
exit:
	return;
}
/*
 * Creating a bson doc with a nested array
 * */

void test2(void)
{
	bson_element_t earr[2];
	bson_value_t nested_values[4];
	bson_t *bdoc = NULL;
	char *json = NULL;
	int ret = 0;

	/* Doc to be creted is :
	 * { 
	 *  "key_string" : "Mohit Singh", 
	 *  "key_array" : [ 
	 *			64,
	 *			3.141590,
	 *			"this is 3rd element",
	 *			"this is 4th element"
	 *		   ] 
	 * }
	 * */
	bson_value_set_int32(&nested_values[0], 64);
	bson_value_set_double(&nested_values[1], 3.141590);
	bson_value_set_str(&nested_values[2], "this is 3rd element");
	bson_value_set_str(&nested_values[3], "this is 4th element");

	earr[0].key = "key_string";
	bson_value_set_str(&earr[0].val, "Mohit Singh");

	earr[1].key = "key_array";
	bson_value_set_array(&earr[1].val, nested_values, 4);

	if ((ret = bson_doc_create(earr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test2 FAILED !\n");
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test2 FAILED !\n");
		bson_doc_destroy(bdoc);
		goto exit;
	}
	bson_value_free(&nested_values[2]);
	bson_value_free(&nested_values[3]);
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);
	printf("json : %s\n", json);
	print_bson_elements(bdoc);
	printf("Test2 PASSED\n\n");

	free(json); json = NULL;
	bson_doc_destroy(bdoc);
exit:
	return;
}

/*
 * Creating a bson doc with a nested doc
 * */

void test3(void)
{
	bson_element_t earr[2];
	bson_t *nested_doc = NULL;
	bson_t *bdoc = NULL;
	char *json = NULL;
	int ret = 0;

	/* Doc to be creted is :
	 * { 
	 *  "key_string" : "Mohit Singh",
	 *  "key_doc" : { 
	 *  		"nested_key_string" : "nested doc's string",
	 *		"nested_key_int" : 1234
	 *		}
	 * }
	 * */
	
	/*create doc to be nested*/
	earr[0].key = "nested_key_string";
	bson_value_set_str(&earr[0].val, "nested doc's string");

	earr[1].key = "key_int";
	bson_value_set_int32(&earr[1].val, 1234);

	if ((ret = bson_doc_create(earr, 2, &nested_doc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(nested_doc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(nested_doc);
		goto exit;
	}
	printf("nested json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);
			
	/*create the outer doc*/
	earr[0].key = "key_string";
        bson_value_set_str(&earr[0].val, "Mohit Singh");
	earr[1].key = "key_doc";
	bson_value_set_doc(&earr[1].val, nested_doc);

	bson_doc_destroy(nested_doc);

	if ((ret = bson_doc_create(earr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test3 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);
	print_bson_elements(bdoc);
	bson_doc_destroy(bdoc);
	printf("Test3 PASSED\n\n");
exit:
	return;
}

/*
 * Creating a bson doc with a nested array. Nested array contains a nested doc. 
 * */
void test4(void)
{
	bson_element_t earr[2];
	bson_value_t nested_values[4];
	bson_t *nested_doc = NULL;
	bson_t *bdoc = NULL;
	char *json = NULL;
	int ret = 0;

	/* Doc to be creted is :
	 * { "key_string" : "Mohit Singh",
	 *   "key_array" :[ 64, 
	 *		    { "nested_key_string" : "nested doc's string", 
	 *		       "nested_key_int" : 1234 
	 *		    },
	 *		    "this is 3rd element",
	 *		    "this is 4th element"
	 *		  ]
	 * }
	 * */
	
	/*create doc to be nested*/
	earr[0].key = "nested_key_string";
	bson_value_set_str(&earr[0].val, "nested doc's string");

	earr[1].key = "key_int";
	bson_value_set_int32(&earr[1].val, 1234);

	if ((ret = bson_doc_create(earr, 2, &nested_doc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(nested_doc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(nested_doc);
		goto exit;
	}
	printf("nested json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);

	/*create nested array*/
	bson_value_set_int32(&nested_values[0], 64);
	bson_value_set_doc(&nested_values[1], nested_doc);
	bson_value_set_str(&nested_values[2], "this is 3rd element");
	bson_value_set_str(&nested_values[3], "this is 4th element");

	bson_doc_destroy(nested_doc);
	/*create outer doc*/
	earr[0].key = "key_string";
	bson_value_set_str(&earr[0].val, "Mohit Singh");

	earr[1].key = "key_array";
	bson_value_set_array(&earr[1].val, nested_values, 4);
	
	bson_value_free(&nested_values[0]);
	bson_value_free(&nested_values[1]);
	bson_value_free(&nested_values[2]);
	bson_value_free(&nested_values[3]);

	if ((ret = bson_doc_create(earr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test4 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);
	print_bson_elements(bdoc);
	bson_doc_destroy(bdoc);
	printf("Test4 PASSED\n\n");
exit:
	return;
}

/*
 * Creating a bson doc with a nested doc. Nested doc contains a nested array. 
 * */
void test5(void)
{
	bson_element_t earr[2];
	bson_value_t nested_values[4];
	bson_t *nested_doc = NULL;
	bson_t *bdoc = NULL;
	char *json = NULL;
	int ret = 0;

	/* Doc to be creted is :
	 * { 
	 * "key_string" : "Mohit Singh", 
	 * "key_doc" : { 
	 *		"nested_key_string" : "nested doc's string",
	 *		"nested_key_array" : [ 
	 *					64, 
	 *					3.150000,
	 *					"this is 3rd element",
	 *					"this is 4th element"
	 *				     ] 
	 *		}
	 * }
	 * */
	
	/*create nested array*/
	bson_value_set_int32(&nested_values[0], 64);
	bson_value_set_double(&nested_values[1], 3.150000);
	bson_value_set_str(&nested_values[2], "this is 3rd element");
	bson_value_set_str(&nested_values[3], "this is 4th element");

	/*create doc to be nested*/
	earr[0].key = "nested_key_string";
	bson_value_set_str(&earr[0].val, "nested doc's string");

	earr[1].key = "nested_key_array";
	bson_value_set_array(&earr[1].val, nested_values, 4);

	if ((ret = bson_doc_create(earr, 2, &nested_doc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		bson_value_free(&nested_values[0]);
		bson_value_free(&nested_values[1]);
		bson_value_free(&nested_values[2]);
		bson_value_free(&nested_values[3]);
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(nested_doc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		bson_value_free(&nested_values[0]);
		bson_value_free(&nested_values[1]);
		bson_value_free(&nested_values[2]);
		bson_value_free(&nested_values[3]);
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(nested_doc);
		goto exit;
	}
	printf("nested json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&nested_values[0]);
	bson_value_free(&nested_values[1]);
	bson_value_free(&nested_values[2]);
	bson_value_free(&nested_values[3]);
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);

	/*create outer doc*/
	earr[0].key = "key_string";
	bson_value_set_str(&earr[0].val, "Mohit Singh");

	earr[1].key = "key_doc";
	bson_value_set_doc(&earr[1].val, nested_doc);

	bson_doc_destroy(nested_doc);

	if ((ret = bson_doc_create(earr, 2, &bdoc)) < 0) {
		printf("Error [%d] [%s] in bson_create_doc !\n", 
						ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		goto exit;
	}
	if ((ret = bson_to_json(bdoc, &json, NULL )) < 0) {
		printf("Error [%d] [%s] in bson_to_json !\n", 
						ret, bson_err_msg(ret));
		printf("Test5 FAILED !\n");
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_doc_destroy(bdoc);
		goto exit;
	}
	printf("json : %s\n", json);
	free(json); json = NULL;
	bson_value_free(&earr[0].val);
	bson_value_free(&earr[1].val);
	print_bson_elements(bdoc);
	bson_doc_destroy(bdoc);
	printf("Test5 PASSED\n\n");
exit:
	return;

}

/** Function to test creation of bson doc, writing a list of bson doc to a buf
 * and reading a list of bson documents from a buffer
 * */
void test6(void)
{
	bson_element_t earr[4];
	bson_t *bdoc[NUM] = {NULL, };
	char *json = NULL;
	int ret = 0;
	int i = 0;
	uint8_t *buf = NULL;
	size_t buf_len = 0;
	bson_t **docs = NULL;
	size_t count = 0;
	off_t parsed_len = 0;

	/* Doc to be creted is :
	 * {
	 *  "key_int32" : 1,
	 *  "key_string" : "Mohit Singh"
	 * } 
	 * */
	for (i = 0; i < NUM; i++) {
		earr[0].key = "key_int32";
		bson_value_set_int32(&earr[0].val, i);
		earr[1].key = "key_int64";
		bson_value_set_int64(&earr[1].val, i);
		earr[2].key = "key_double";
		bson_value_set_double(&earr[2].val, 3.141590);
		earr[3].key = "key_string";
		bson_value_set_str(&earr[3].val, "Mohit Singh");


		if ((ret = bson_doc_create(earr, 4, &bdoc[i])) < 0) {
			printf("Error %d %s in bson_doc_create\n", 
					ret,  bson_err_msg(ret));
			bson_value_free(&earr[0].val);
			bson_value_free(&earr[1].val);
			bson_value_free(&earr[2].val);
			bson_value_free(&earr[3].val);
			for (i = 0; i < NUM; i++) {
				bson_doc_destroy(bdoc[i]);
			}
			goto exit;
		}
		bson_value_free(&earr[0].val);
		bson_value_free(&earr[1].val);
		bson_value_free(&earr[2].val);
		bson_value_free(&earr[3].val);
		bson_to_json(bdoc[i], &json, NULL);
		printf("json : %s\n", json);
		free(json); json = NULL;
	}
	if ((ret = bson_write_to_buffer(&buf, &buf_len,bdoc, NUM)) < 0) {
		printf("Error %d %s in bson_write_to_buffer\n", ret , bson_err_msg(ret));
		for (i = 0; i < NUM; i++) {
			bson_doc_destroy(bdoc[i]);
		}
		goto exit;
	}
	printf("buf_len %lu\n", buf_len);
	FILE *fp = fopen("./datafile", "w");
	fwrite(buf, buf_len, 1, fp);
	fclose(fp);
	if ((ret = bson_read_from_buffer(buf, buf_len, &parsed_len, &docs, &count)) < 0) {
		printf("Error %d %s in bson_read_from_buffer\n", ret , bson_err_msg(ret));
		for (i = 0; i < NUM; i++) {
			bson_doc_destroy(bdoc[i]);
		}
		free(buf); buf = NULL;
		goto exit;
	}
	printf("parsed_len %lu count %lu\n", parsed_len, count);
	for (i = 0; i < count; i++) {
		printf("json %s\n", (json = bson_as_json(docs[i], NULL)));
		free(json); json = NULL;
	}
	for (i = 0; i < count; i++) {
		bson_doc_destroy(docs[i]);
	}
	free(docs); docs = NULL;
	free(buf); buf = NULL;
	for (i = 0; i < NUM; i++) {
		bson_doc_destroy(bdoc[i]);
	}
exit:
	return;
}
