#include <common/types.h>
#include <hal/hal.h>
#include <memory/heap.h>

#define container_of(ptr, type, member) ((type *)((u8 *)(ptr) - offsetof(type, member)))

static HAL_t gHAL;
HAL_t* HAL = &gHAL;

typedef struct ListNode {
    struct ListNode *prev;
    struct ListNode *next;
} ListNode_t;

typedef struct HALDevEntry {
    HALDevice_t *dev;
    ListNode_t link;
} HALDevEntry_t;

static ListNode_t hal_dev_head; 
static bool listInited;

static void list_init() {
    hal_dev_head.prev = &hal_dev_head;
    hal_dev_head.next = &hal_dev_head;
}

void HAL_init() {
    if (!listInited) {
        list_init();
        listInited = true;
    }
    
    ListNode_t *cur = hal_dev_head.next;
    while (cur != &hal_dev_head) {
        HALDevEntry_t *entry = container_of(cur, HALDevEntry_t, link);
        HALDevice_t *dev = entry->dev;

        CmdIdentResponse_t ident = {0};
        if (dev->control(CMD_IDENT, (hal_arg_t)0, (hal_arg_t)&ident) == SUCCESS) {
            switch (ident.dev_type)
            {
                case DEV_CONSOLE: {
                    if (!HAL->active_console) {
                        HAL->active_console = dev;
                    }
                    break;
                }

                case DEV_MEMMANAGER: {
                    if (!HAL->memory_manager) {
                        HAL->memory_manager = dev;
                    }
                    break;
                }
                
                default:
                    break;
            }
        }

        cur = cur->next;
    }
}

status_t hal_register_device(hal_registration_callback_t cb) {
    if(!cb) {
        return FAILURE;
    }

    if (listInited == false) {
        list_init();
        listInited = true;
    }

    HALDevice_t *newDevice = cb();
    if (!newDevice) {
        return FAILURE;
    }

    HALDevEntry_t *newHALEntry = kmalloc(sizeof(HALDevEntry_t));
    if (!newHALEntry) {
        return FAILURE;
    }

    newHALEntry->dev = newDevice;

    if (newHALEntry->dev->control(CMD_INIT, 0, 0) == FAILURE) {
        return FAILURE;
    }

    ListNode_t *cur = &hal_dev_head;
    while (cur->next != &hal_dev_head) {
        cur = cur->next;
    }
    
    newHALEntry->link.next = &hal_dev_head;
    newHALEntry->link.prev = cur;
    cur->next = &newHALEntry->link;
    hal_dev_head.prev = &newHALEntry->link;

    return SUCCESS;
}

void hal_foreach_device(hal_device_callback_t cb) {
    if (!cb) {
        return;
    }

    if (!listInited) {
        list_init();
        listInited = true;
    }

    ListNode_t *cur = hal_dev_head.next;
    while (cur != &hal_dev_head) {
        HALDevEntry_t *entry = container_of(cur, HALDevEntry_t, link);

        if (cb(entry->dev) == true) {
            return;
        }

        cur = cur->next;
    }
}