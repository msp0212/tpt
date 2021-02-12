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
	struct http_request_info *http_request_info = NULL;

	ret = http_parse_request("", 0, &http_request_info);
	if (ret < 0) {
		printf("Error [%d] [%s] in http_parse_request\n",
				ret, 
				http_get_error_string(ret));
	} else if (ret == 0) {
		printf("ret %d\n",ret);
	}
	else {
		printf("ret %d\n",ret);
	}
	return 0;
	buf = (char *)calloc(tot_size, sizeof(char));
	while ((bytes_read = read(0, buf+offset, tot_size - offset)) > 0) {
		offset += bytes_read;
		if ((tot_size - offset) <= 256)	{
			tot_size = 2*tot_size;
			buf = (char*)realloc(buf, tot_size);
		}
		ret = http_parse_request(buf, offset, &http_request_info);
		if (ret < 0) {
			printf("Error [%d] [%s] in http_parse_request\n",
						ret, 
						http_get_error_string(ret));
			break;
		} else if (ret == 0) {
			printf("method [%s] URI [%s] version [%s]\n", 
				http_method_string[http_request_info->method],
				http_request_info->uri,
				http_version_string[http_request_info->version]);
			printf("\nHTTP Header Length [%d]\n", http_request_info->header_length);
                        printf("\nHTTP body [%.*s]\n", offset - http_request_info->header_length, buf + http_request_info->header_length);
			http_print_headers(http_request_info->headers_list);
		}
		else {
			printf("ret %d\n",ret);
		}
	}
	printf("\nbuffer :\n%s", buf);
	ret = http_request_info_free(&http_request_info);
	if (ret < 0) {
		printf("Error [%d] [%s] in http_request_info_free\n", ret, http_get_error_string(ret));
	}
	free(buf);buf=NULL;
	return 0;
}
