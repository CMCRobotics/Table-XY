/* generated HAL source file - do not edit */
#include "hal_data.h"
flash_lp_instance_ctrl_t DataFlash_access_ctrl;
const flash_cfg_t DataFlash_access_cfg =
{ .data_flash_bgo = false, .p_callback = NULL, .p_context = NULL, .ipl = (BSP_IRQ_DISABLED),
#if defined(VECTOR_NUMBER_FCU_FRDYI)
    .irq                 = VECTOR_NUMBER_FCU_FRDYI,
#else
  .irq = FSP_INVALID_VECTOR,
#endif
        };
/* Instance structure to use this module. */
const flash_instance_t DataFlash_access =
{ .p_ctrl = &DataFlash_access_ctrl, .p_cfg = &DataFlash_access_cfg, .p_api = &g_flash_on_flash_lp };
void g_hal_init(void)
{
    g_common_init ();
}
