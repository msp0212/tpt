/**@file 
 * @brief Header file for using conn mgr connection APIs 
 * */
#ifndef CONN_MGR_CONN_H
#define CONN_MGR_CONN_H

int conn_mgr_conn_errno; /**<On error, this variable is set with err code*/

/**@typedef conn_mgr_conn_t
 * @brief Data type for conn
 * */
typedef struct conn_mgr_conn_t conn_mgr_conn_t;

conn_mgr_conn_t *conn_mgr_conn_new(char *ip, int port, int timeout);
void conn_mgr_conn_destroy(conn_mgr_conn_t *conn);
int conn_mgr_conn_get_fd(conn_mgr_conn_t *conn);
#endif
