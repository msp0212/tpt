/** @file
 * @brief This header file defines the error codes used by library
 * */
#ifndef CON_MGR_ERRROS_H
#define CONN_MGR_ERRORS_H

#define CONN_MGR_MIN_ERR_CODE -2100

/** @enum conn_mgr_err
 *  @brief List of bson api error codes
 * */
typedef enum conn_mgr_err {
	CONN_MGR_SUCCESS = 0,
	CONN_MGR_ERR_NULL_PARAMS = CONN_MGR_MIN_ERR_CODE,
	CONN_MGR_ERR_MEM_ALLOC,
	CONN_MGR_CONN_ERR_SOCKET,
	CONN_MGR_CONN_ERR_SETNONBLOCKING,
	CONN_MGR_CONN_ERR_SETBLOCKING,
	CONN_MGR_CONN_ERR_CONNECT,
	CONN_MGR_CONN_ERR_TIMEDOUT,
	CONN_MGR_CONN_ERR_POLL,
	CONN_MGR_CONN_ERR_CONNECTING
}conn_mgr_err_t;

#define CONN_MGR_ERR_STR(code)  #code

/** @def CASE(code)
 * Macro to return the error string for an error code
 * */
#define CASE(code) case code : return CONN_MGR_ERR_STR(code)
#endif
