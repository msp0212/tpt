/**@file icap_client.h
 * @brief Header file to be included by user of the library.
 */
# ifndef _ICAP_CLIENT_H_
# define _ICAP_CLIENT_H_

/**@enum eModificationType
 * Type of the modification to be requested i.e REQMOD or RESPMOD
 */
typedef enum modificationType
{
	ICAP_MOD_REQ = 1,/**<Request Modification*/
	ICAP_MOD_RESP = 2/**<Response Modification*/
}eModificationType;

/**@enum eIcapBodyType
 * Type of the body received in the ICAP message.
 */
typedef enum eIcapBodyType
{
	ICAP_REQ_BODY = 1,/**<ICAP message contains request body*/
	ICAP_RES_BODY = 2,/**<ICAP message contains response body*/
	ICAP_NULL_BODY = 3/**<ICAP message does not contain any body*/
}eIcapBodyType;


#define ICAP_SUCCESS 0 
/*ICAP Status Codes*/
#define ICAP_OK 200 
#define ICAP_CONTINUE 100
#define ICAP_UNMODIFIED 204

/*Network Errors*/
#define ICAP_NET_ERR_SOCKET -1001
#define ICAP_NET_ERR_CONNECTING -1002
#define ICAP_NET_ERR_SEND -1003
#define ICAP_NET_ERR_RECV -1004
#define ICAP_NET_ERR_SOCK_OPTIONS -1005
#define ICAP_NET_ERR_SERVER_CLOSED_CONN -1006
/*General Errors*/
#define ICAP_ERR_INVALID_PARAMS -2001
#define ICAP_ERR_TOO_MANY_CONNECTIONS -2002
#define ICAP_ERR_MEM_ALLOC -2003
#define ICAP_ERR_CALLBACK -2004
#define ICAP_ERR_POLL_TIMEOUT -2005
#define ICAP_ERR_POLL_FAILED -2006
/*Protocol Errors*/
#define ICAP_PROTO_ERR_INVALID_METHOD -3001
#define ICAP_PROTO_ERR_PARSING_HEADER -3002
#define ICAP_PROTO_ERR_NO_ENCAPSULATED_HEADER -3003
#define ICAP_PROTO_ERR_PARSING_CHUNK -3004
#define ICAP_PROTO_ERR_CHUNK_SIZE_EXCEEDED -3005
/**@struct values
 * @brief To store header values
*/
typedef struct values
{
	char value[256]; /**<value of the header*/
	struct values *next;/**<link to next value*/
}values; 

/**@struct nameValues
 * @brief To store header name-value pairs
 */
typedef struct nameValues
{
	char name[128];/**<name of the header*/
	values *valueHead;/**<head of linked list of values*/
	struct nameValues *next;/**<link to next header node*/
}nameValues;
/**@struct icapResponseInfo
 * @brief To store the parsed ICAP response information*/
typedef struct icapResponseInfo
{
	char versionMajor;/**<ICAP major version*/
	char versionMinor;/**<ICAP minor version*/
	int statusCode;   /**<status code returned by ICAP server*/
	char statusString[128];/**<string corresponding to the status code*/
	nameValues *headersHead; /**<head of linked list of headers*/
	int reqHdrOffset;/**<offset of HTTP request headers relative to start of ICAP body*/
	int resHdrOffset;/**<offset of HTTP response headers relatibe to start of ICAP body*/
	eIcapBodyType bodyType;/**<type of HTTP body encapsulated inside ICAP body*/
	int bodyOffset;/**<offset of HTTP body relative to start of ICAP body*/
}icapResponseInfo;	
/**@typedef pHandleIcapData
 * @brief function pointer for callback function
 */
typedef int (*pHandleIcapData)(const char *, int , int);
typedef struct icapContext icapContext;
char* icapGetErrorString(int errnum);
int icapInitContextPool(int poolSize);
int icapFreeContextPool(void);
int icapConnect(char *icapServerIP, int icapServerPort, icapContext **context);
int icapDisconnect(icapContext **context);
int icapSendOptionsRequest(icapContext *context, char *icapService);
int icapSendHeaders(icapContext *context, char *icapService, char *icapQuery, eModificationType eModType, char *httpRequestHeaders, char *httpResponseHeaders, 
													int bodyPresent, int allow204, int previewBytes);
int icapSendData(icapContext *context , void *httpBody, size_t httpBodySize);
int icapReceiveData(icapContext *context, int withCallback, char **buffer, int *bufLen, pHandleIcapData handleIcapData, int clientSocket);
int icapSendPreviewAndWait(icapContext *context, char *httpBody, size_t httpBodySize, char **buffer, int *bufLen);
#endif
