#ifndef DRIVER_IOCTL_H
#define DRIVER_IOCTL_H
#include <linux/ioctl.h>

typedef struct test_ioctl {
	int code;
	int val;
} test_ioctl_t;

#define TEST_GET _IOR('q', 1, test_ioctl_t *)
#define TEST_SET _IOR('q', 2, test_ioctl_t *)

#endif
