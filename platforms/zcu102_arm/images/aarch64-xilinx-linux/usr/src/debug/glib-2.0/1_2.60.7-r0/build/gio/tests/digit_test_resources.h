#ifndef __RESOURCE__digit_test_H__
#define __RESOURCE__digit_test_H__

#include <gio/gio.h>

extern GResource *_digit_test_get_resource (void);

extern void _digit_test_register_resource (void);
extern void _digit_test_unregister_resource (void);

#endif
