#pragma once

/* return error codes */
typedef enum { glb_OK, glb_FAIL, glb_NOMEM, glb_LIMIT,
               glb_IO_FAIL, glb_FORMAT
} glb_err_codes;



#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L

#include <stdbool.h>

#else

#undef bool
#undef true
#undef false

#define bool unsigned char
#define true  1
#define false 0

#endif

