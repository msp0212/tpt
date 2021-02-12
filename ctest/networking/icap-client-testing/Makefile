#icap_client Testing
#Mohit Singh 22 Aug 2013

MESSAGE="icap_client test utilities built successfully"

CC  = gcc -m32 -g3
LIB_PATH = /root/data/tpt/libs
CFLAGS  += -I./include -L $(LIB_PATH) -licap_client -lpthread

all :
	$(CC) respMod.c -o resp_mod $(CFLAGS)
#	$(CC) respMod_nullBody.c -o resp_mod_null_body $(CFLAGS)
#	$(CC) respMod_preview.c -o preview $(CFLAGS)
#	$(CC) getReqMod.c -o get_req_mod $(CFLAGS)
	$(CC) my.c -o my $(CFLAGS)
#	$(CC) postReqMod.c -o post_req_mod $(CFLAGS)
	$(CC) optionsReq.c -o options_req $(CFLAGS)
#	$(CC) connPool.c -o conn_pool $(CFLAGS)
	@echo "*******************************************************************"
	@echo $(MESSAGE)
	@echo "*******************************************************************"
#gcc -m32 -g3 connPool.c -o conn_pool -I./include -L/root/data/tpt/libs -licap_client -lpthread
clean:
	-@rm resp_mod resp_mod_null_body get_req_mod  post_req_mod conn_pool preview options_req
