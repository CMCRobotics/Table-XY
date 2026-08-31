/* generated HAL header file - do not edit */
#ifndef HAL_DATA_H_
#define HAL_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "common_data.h"
#include "r_flash_lp.h"
#include "r_flash_api.h"
FSP_HEADER
/* Flash on Flash LP Instance. */
extern const flash_instance_t DataFlash_access;

/** Access the Flash LP instance using these structures when calling API functions directly (::p_api is not used). */
extern flash_lp_instance_ctrl_t DataFlash_access_ctrl;
extern const flash_cfg_t DataFlash_access_cfg;

#ifndef NULL
void NULL(flash_callback_args_t *p_args);
#endif
void hal_entry(void);
void g_hal_init(void);
FSP_FOOTER
#endif /* HAL_DATA_H_ */
