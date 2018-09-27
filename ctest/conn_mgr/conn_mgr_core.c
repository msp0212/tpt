/*System Headers*/
#include <stdio.h>
/*Local Headers*/
#include <conn_mgr_core.h>
#include <conn_mgr_errors.h>

/**@brief get error string for an err code
 * @param[in] code err code
 * @return err string for the error code
 * */
char *conn_mgr_err_msg(int code) 
{
	switch (code) {
	CASE(CONN_MGR_SUCCESS);
	CASE(CONN_MGR_ERR_NULL_PARAMS);
	CASE(CONN_MGR_ERR_MEM_ALLOC);
	CASE(CONN_MGR_CONN_ERR_SOCKET);
	CASE(CONN_MGR_CONN_ERR_SETNONBLOCKING);
	CASE(CONN_MGR_CONN_ERR_SETBLOCKING);
	CASE(CONN_MGR_CONN_ERR_CONNECT);
	CASE(CONN_MGR_CONN_ERR_TIMEDOUT);
	CASE(CONN_MGR_CONN_ERR_POLL);
	CASE(CONN_MGR_CONN_ERR_CONNECTING);
	default:
		return NULL;
	}
}

