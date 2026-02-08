#include <stddef.h>

#pragma once

#define __print_error_condition(_condition)\
printf("Error at %s:%d, in function %s\r\n", __FILE__, __LINE__, __func__);\
printf("Error type: %s\r\n",#_condition)

#define __return_error_if(_condition)\
if(_condition){\
    __print_error_condition(_condition);\
    return STATUS_ERROR;\
}

#define __return_null_if(_condition)\
if(_condition){\
    __print_error_condition(_condition);\
    return NULL;\
}

#define __goto_cleanup_if(_condition)\
if(_condition){\
   __print_error_condition(_condition);\
    goto __cleanup;\
}



typedef enum{
    STATUS_ERROR = -1,
    STATUS_OK = 0
}error_e;

