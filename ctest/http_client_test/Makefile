.PHONY : clean

all: test_req test_resp http_client

test_req: test_req.c
	gcc -g3 -o test_req -Wall test_req.c -I../../include/ -L ../../libs/ -lhttp_utils
test_resp: test_resp.c
	gcc -g3 -o  test_resp -Wall test_resp.c -I ../../include/ -L ../../libs -lhttp_utils
http_client: http_client.c
	gcc -g3 -o  http_client -Wall http_client.c 
clean:
	-@rm -f test_req test_resp http_client
