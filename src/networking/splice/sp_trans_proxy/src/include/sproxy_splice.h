#ifndef SPROXY_SPLICE_H
#define SPROXY_SPLICE_H

int make_splice(int fd_in, int fd_out, int *pipefd_arr, size_t len);
#endif
