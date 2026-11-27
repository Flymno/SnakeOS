#ifndef HAL_H_
#define HAL_H_

#include <common/types.h>

#define HAL_DEV_NAME_MAX 32

/* Command Definitions */
#define CMD_PUT 0
#define CMD_WRITE 1
#define CMD_READ 2
#define CMD_INIT 3
#define CMD_IDENT 4
#define CMD_CTRL 5

/* Device Definitions */
#define DEV_CONSOLE 0
#define DEV_MEMMANAGER 1

/* Command Data Defitions */
typedef struct CmdIdentResponse
{
    u32 dev_type;
    char name[HAL_DEV_NAME_MAX];
} CmdIdentResponse_t;


typedef struct HALDevice_t
{
    status_t (*control)(u32 CMD, hal_arg_t in, hal_arg_t out);
} HALDevice_t;

typedef struct
{
    HALDevice_t* active_console;
    HALDevice_t* memory_manager;
} HAL_t;

typedef HALDevice_t* (*hal_registration_callback_t)();
typedef bool (*hal_device_callback_t)(const HALDevice_t *device);

extern HAL_t* HAL;

status_t hal_register_device(hal_registration_callback_t cb);
void hal_foreach_device(hal_device_callback_t cb);
void HAL_init(void);

#endif