/* generated thread header file - do not edit */
#ifndef CAN_APP_THREAD_H_
#define CAN_APP_THREAD_H_
#include "bsp_api.h"
#include "tx_api.h"
#include "hal_data.h"
#ifdef __cplusplus
                extern "C" void can_app_thread_entry(void);
                #else
extern void can_app_thread_entry(void);
#endif
#include "r_can.h"
#include "r_can_api.h"
FSP_HEADER
/** CAN on CAN Instance. */
extern const can_instance_t g_can1;
/** Access the CAN instance using these structures when calling API functions directly (::p_api is not used). */
extern can_instance_ctrl_t g_can1_ctrl;
extern const can_cfg_t g_can1_cfg;
extern const can_extended_cfg_t g_can1_extended_cfg;

#ifndef can_callback
void can_callback(can_callback_args_t *p_args);
#endif
#define CAN_NO_OF_MAILBOXES_g_can1 (4)
/** CAN on CAN Instance. */
extern const can_instance_t g_can0;
/** Access the CAN instance using these structures when calling API functions directly (::p_api is not used). */
extern can_instance_ctrl_t g_can0_ctrl;
extern const can_cfg_t g_can0_cfg;
extern const can_extended_cfg_t g_can0_extended_cfg;

#ifndef can_callback
void can_callback(can_callback_args_t *p_args);
#endif
#define CAN_NO_OF_MAILBOXES_g_can0 (4)
FSP_FOOTER
#endif /* CAN_APP_THREAD_H_ */
