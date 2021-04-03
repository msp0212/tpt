#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/*For close*/
#include <unistd.h>
/*For socket()*/
#include <sys/socket.h>
/*For inet_aton()*/
#include <arpa/inet.h>
/*For fcntl()*/
#include <fcntl.h>
/*libevent header*/
#include <event2/event.h>

static void inline print_error(char *str);
static void run_server(char *ip, int port);
void srvr_accept_cb(evutil_socket_t fd, short what, void *arg);
void cli_recv_cb(evutil_socket_t fd, short what, void *arg);
void cli_send_cb(evutil_socket_t fd, short what, void *arg);

enum {
	LOG_CRIT,
	LOG_ERR,
	LOG_INFO,
	LOG_DEBUG,
	LOG_LEVEL_MAX
};

char *resp_hdr = 
	"HTTP/1.1 200 OK\r\n"
	"Cache-Control: no-cache\r\n"
	"Expires: -1\r\n"
	"Pragma: no-cache\r\n"
	"Content-Type: application/json; charset=utf-8\r\n"
	"Set-Cookie: .TABTOUCHAUTH=3ABDD86256A14400039B3382FA861396553FD3B195A9FA8DF55842B52233B3E2E8D0C645AF738DABE1DAF5D859102F4FFCB565D80C3F43FD06114FCBCA1D8496A27D71E2CBB56C32D4AA53D8425190912FE59B7BF9622A89F18050E59A84B3F7EC2405E2F3D49B171B7B7BCD07702D2899898B8EE62D0F2D6F9B6A0E7C9EB1EF0EFC6BE9F3E22B8153020D19138BD9554B03C0B839708C34DA28864D5E61CB61BDEAE8E4B2DAC28EF5C01F8C961E0410BB53E369D1399E17A608D5FB68AEB17F88D1D3FA8DD06A3641672BAC998A58D992B23631373892689FD36B56C327D7FE4D226A4363E05667297AF2893968514EFF678950B8BD6C4A66A9D6E3D7BF70BE38E34640B557251FADF029478275FDD3BD573EA595369BB805A9D7867522030808BD01DE119B89021AD4771B1E61A5E811961D6DC17EABC3C65A8557287ABA2B20740208847E0889E220042C8B3FD90F966C88C493152EE6C2A341B548864C4E980C945CE4B50196AC20580CA91B; path=/; secure; httponly\r\n"
	"Set-Cookie: ttac=; expires=Wed, 31-May-2017 00:58:50 GMT; path=/; secure; httponly\r\n"
	"X-Robots-Tag: noindex, nofollow\r\n"
	"Content-Length: 64\r\n"
	"ETag: W/\"40-yhzgSA7NC+PlSVuQtgRQnWAUJ7o\r\n"
	"\r\n";

char *resp_body = "{\"IsSuccessful\":false,\"MustChangePassword\":false,\"OWS132V\":true}";

char *resp = 
	"HTTP/1.1 200 OK\r\n"
	"Cache-Control: no-cache\r\n"
	"Expires: -1\r\n"
	"Pragma: no-cache\r\n"
	"Content-Type: application/json; charset=utf-8\r\n"
	"Set-Cookie: .TABTOUCHAUTH=3ABDD86256A14400039B3382FA861396553FD3B195A9FA8DF55842B52233B3E2E8D0C645AF738DABE1DAF5D859102F4FFCB565D80C3F43FD06114FCBCA1D8496A27D71E2CBB56C32D4AA53D8425190912FE59B7BF9622A89F18050E59A84B3F7EC2405E2F3D49B171B7B7BCD07702D2899898B8EE62D0F2D6F9B6A0E7C9EB1EF0EFC6BE9F3E22B8153020D19138BD9554B03C0B839708C34DA28864D5E61CB61BDEAE8E4B2DAC28EF5C01F8C961E0410BB53E369D1399E17A608D5FB68AEB17F88D1D3FA8DD06A3641672BAC998A58D992B23631373892689FD36B56C327D7FE4D226A4363E05667297AF2893968514EFF678950B8BD6C4A66A9D6E3D7BF70BE38E34640B557251FADF029478275FDD3BD573EA595369BB805A9D7867522030808BD01DE119B89021AD4771B1E61A5E811961D6DC17EABC3C65A8557287ABA2B20740208847E0889E220042C8B3FD90F966C88C493152EE6C2A341B548864C4E980C945CE4B50196AC20580CA91B; path=/; secure; httponly\r\n"
	"Set-Cookie: ttac=; expires=Wed, 31-May-2017 00:58:50 GMT; path=/; secure; httponly\r\n"
	"X-Robots-Tag: noindex, nofollow\r\n"
	"Content-Length: 64\r\n"
	"ETag: W/\"40-yhzgSA7NC+PlSVuQtgRQnWAUJ7o\r\n"
	"\r\n"
	"{\"IsSuccessful\":false,\"MustChangePassword\":false,\"OWS132V\":true}";

#define CONN_BUF_SIZE 8192
struct conn_info {
	uint8_t buf[CONN_BUF_SIZE];
	struct event *ev_read;
	struct event *ev_write;
};

int g_sev; 

void print_log(int sev, char *fmt, ...)
{
	va_list ap;

	if (sev > g_sev) 
		goto ret;
	switch(sev) {
	case LOG_CRIT:
		printf("CRIT:: ");
		break;
	case LOG_ERR: 
		printf("ERR:: ");
		break;
	case LOG_INFO:
		printf("INFO:: ");
		break;
	case LOG_DEBUG:
		printf("DEBUG:: ");
		break;
	default:
		goto ret;
	}
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
ret:
	return;
}

static void inline print_error(char *str)
{
	print_log(LOG_ERR, "%s: err [%d] [%s]\n", str, errno, strerror(errno));
}

struct conn_info *conn_info_new(struct event_base *ev_base, int fd)
{
	struct conn_info *cinfo = NULL;

	cinfo = malloc(sizeof(*cinfo));
	if (!cinfo) {
		print_log(LOG_CRIT, "Are we under memory pressure?\n");
		goto err;
	}
	cinfo->ev_read = event_new(ev_base, fd, EV_READ | EV_PERSIST, 
													cli_recv_cb, cinfo);
	if (!cinfo->ev_read) {
		print_error("event_new\n");
		free(cinfo);
		goto err;
	}

	cinfo->ev_write = event_new(ev_base, fd, EV_WRITE | EV_PERSIST, 
													cli_send_cb, cinfo);
	if (!cinfo->ev_write) {
		print_error("event_new\n");
		event_free(cinfo->ev_read);
		free(cinfo);
		goto err;
	}
	return cinfo;
err:
	print_log(LOG_CRIT, "Cannot get new conn info!\n");
	return NULL;
}

void conn_info_free(struct conn_info *cinfo) 
{
	event_free(cinfo->ev_read);
	event_free(cinfo->ev_write);
	free(cinfo);
}

void cli_send_cb(evutil_socket_t fd, short what, void *arg)
{
	struct conn_info *cinfo = arg;
	int bytes = 0;
	print_log(LOG_DEBUG, "%s\n", __func__);
#if 1
	send(fd, resp, strlen(resp), 0);
#else
	send(fd, resp_hdr, strlen(resp_hdr), 0);
	send(fd, resp_body, strlen(resp_body), 0);
#endif	
	event_del(cinfo->ev_write);
}

void cli_recv_cb(evutil_socket_t fd, short what, void *arg)
{
	struct conn_info *cinfo = arg;
	int bytes = 0;
	print_log(LOG_DEBUG, "%s\n", __func__);
	
	while (1) {
		bytes = recv(fd, cinfo->buf, sizeof(cinfo->buf), 0);
		if (bytes <= 0)
			break;
		print_log(LOG_DEBUG, "recvd %d bytes from client\n", bytes);
		
		if (event_add(cinfo->ev_write, NULL) != 0) {
			print_error("event_add");
			goto err;
		}
	}
	if (bytes == 0) {
		print_log(LOG_DEBUG, "conn closed by client\n");
		conn_info_free(cinfo);
		close(fd);
	}
	if (bytes < 0) {
		if (errno == EAGAIN) {
			print_log(LOG_INFO, "Nothing more to read!\n");
		} else {
			print_error("recv");
		}
	}
	return;
err:
	conn_info_free(cinfo); 
	close(fd);

}

void srvr_accept_cb(evutil_socket_t fd, short what, void *arg)
{
	struct sockaddr_in cli_addr;
	int cli_sock;
	socklen_t len = sizeof(cli_sock);
	struct event_base *ev_base = arg;
	struct conn_info *cinfo = NULL;

	print_log(LOG_DEBUG, "%s\n", __func__);
	
	if ((cli_sock = accept(fd, (struct sockaddr *)&cli_sock, &len)) < 0) {
		print_error("accept");
		print_log(LOG_CRIT, "Couldn't accept client conn!");
	} else {
		print_log(LOG_DEBUG, "Accepted client conn [%s : %d]\n", 
				inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		
		if (fcntl(cli_sock, F_SETFL, O_NONBLOCK) < 0) {
			print_error("fcntl");
			goto err;
		}
	
		cinfo = conn_info_new(ev_base, cli_sock);
		if (!cinfo) 
			goto err;
		
		if (event_add(cinfo->ev_read, NULL) != 0) {
			print_error("event_add");
			goto err;
		}
	}
	return;
err:
	print_log(LOG_ERR, "client conn not processed!\n");
	close(cli_sock);
}

void run_server(char *ip, int port)
{
	struct event_base *ev_base = NULL;
	struct event *ev_srvr_sock = NULL;
	struct sockaddr_in srvr_addr;
	int srvr_sock; 

	ev_base = event_base_new();
	if (ev_base == NULL) {
		print_error("event_base_new\n");
		exit(1);
	}
	print_log(LOG_DEBUG, "event base backend method [%s]\n", 
												event_base_get_method(ev_base));
	
	if ((srvr_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		print_error("socket");
		goto err;
	}

	{
		int one = 1;
		setsockopt(srvr_sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	}
	
	if (fcntl(srvr_sock, F_SETFL, O_NONBLOCK) < 0) {
		print_error("fcntl");
		goto err;
	}

	srvr_addr.sin_family = AF_INET;
	srvr_addr.sin_port = htons(port);
	inet_aton(ip, &srvr_addr.sin_addr);
	if (bind(srvr_sock, (struct sockaddr *)&srvr_addr, sizeof(srvr_addr)) 
																		< 0) {
		print_error("bind");
		goto err;
	}

	if (listen(srvr_sock, 128) < 0) {
		print_error("listen");
		goto err;
	}

	ev_srvr_sock = event_new(ev_base, srvr_sock, EV_READ | EV_PERSIST, 
													srvr_accept_cb, ev_base);
	if (!ev_srvr_sock) {
		print_error("event_new");
		goto err;
	}
	if (event_add(ev_srvr_sock, NULL) < 0) {
		print_error("event_add");
		goto err;
	}

	if (event_base_dispatch(ev_base) < 0) {
		print_error("event_base_dispatch\n");
		goto err;
	}
	return;
err:
	print_log(LOG_CRIT, "Cannot start the server!");
	exit(1);
}

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	if (argc < 4) {
		fprintf(stderr, "Usage:\n%s <log_level> <srvr_ip> <srvr_port>\n", 
																	argv[0]);
		exit(1);
	}
	g_sev = atoi(argv[1]);
	if (g_sev > LOG_LEVEL_MAX) 
		g_sev = LOG_LEVEL_MAX - 1;
	run_server(argv[2], atoi(argv[3]));

	return 0;
}
