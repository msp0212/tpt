#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<http_utils.h>

#define BUF_SIZE 1024
int main()
{
	int tot_size = BUF_SIZE;
	int offset = 0;
	int bytes_read = 0;
	int ret = 0;
	char *buf = NULL;
	struct http_response_info *http_response_info = NULL;
	struct value_list *values  = NULL;

	buf = (char *)calloc(tot_size, sizeof(char));
	while ((bytes_read = read(0, buf+offset, tot_size - offset)) > 0) {
		offset += bytes_read;
		if ((tot_size - offset) <= 256)	{
			tot_size = 2*tot_size;
			buf = (char*)realloc(buf, tot_size);
		}
		ret = http_parse_response(buf, offset, &http_response_info);
		if (ret < 0) {
			printf("Error [%d] [%s] in http_parse_response\n",
						ret, 
						http_get_error_string(ret));
			break;
		} else if (ret == 0) {
			printf("\nversion [%s] status_code [%s] "
				"reason_phrase [%s]\n", 
				http_version_string[http_response_info->version],
				http_response_info->status_code,
				http_response_info->reason_phrase);
			printf("\nHTTP Header Length [%d]\n", http_response_info->header_length);
			printf("\nHTTP body [%.*s]\n", offset - http_response_info->header_length, buf + http_response_info->header_length);
			http_print_headers(http_response_info->headers_list);
		}
		else {
			printf("ret %d\n",ret);
		}
	}
	printf("\nbuffer :\n%s", buf);

	ret = http_get_header_value(http_response_info->headers_list, 
					"Server", 
					&values);
	if (ret < 0) {
		printf("Error [%d] [%s] in http_get_header_value\n", ret, http_get_error_string(ret));
	} else {
		printf("HTTP Header Value [%s]\n", values->value);
	}
	ret = http_response_info_free(&http_response_info);
	if (ret < 0) {
		printf("Error [%d] [%s] in http_response_info_free\n", ret, http_get_error_string(ret));
	}
	free(buf);buf=NULL;
	return 0;
}
