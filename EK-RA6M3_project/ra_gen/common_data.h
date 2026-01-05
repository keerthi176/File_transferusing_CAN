/* generated common header file - do not edit */
#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_
#include <stdint.h>
#include "bsp_api.h"
#include "r_qspi.h"
#include "r_spi_flash_api.h"
#include "rm_levelx_nor_spi.h"
#include "lx_api.h"
#include "rm_filex_levelx_nor.h"
#include "fx_api.h"
#include "r_ioport.h"
#include "bsp_pin_cfg.h"
FSP_HEADER
extern const spi_flash_instance_t g_qspi0;
extern qspi_instance_ctrl_t g_qspi0_ctrl;
extern const spi_flash_cfg_t g_qspi0_cfg;
/** Access the LevelX NOR SPI instance using these structures when calling functions directly (::p_api is not used). */
extern rm_levelx_nor_spi_instance_ctrl_t g_rm_levelx_nor_spi0_ctrl;
extern rm_levelx_nor_spi_cfg_t g_rm_levelx_nor_spi0_cfg;

#ifndef rm_filex_levelx_nor_spi_callback
void rm_filex_levelx_nor_spi_callback(rm_levelx_nor_spi_callback_args_t *p_args);
#endif

UINT g_rm_levelx_nor_spi0_initialize(LX_NOR_FLASH *p_nor_flash);
fsp_err_t g_rm_levelx_nor_spi0_close();

#define RA_NOT_DEFINED 0xFFFFFFFF
#if (RA_NOT_DEFINED != g_qspi0)
#define G_RM_LEVELX_NOR_SPI0_SECTOR_SIZE (4096)
#elif (RA_NOT_DEFINED != RA_NOT_DEFINED)
#define G_RM_LEVELX_NOR_SPI0_SECTOR_SIZE (RA_NOT_DEFINED)
#else
#define G_RM_LEVELX_NOR_SPI0_SECTOR_SIZE (RA_NOT_DEFINED)
#endif
#undef RA_NOT_DEFINED
extern LX_NOR_FLASH g_lx_nor0;
extern const rm_filex_levelx_nor_instance_t g_rm_filex_levelx_nor_instance;

/** Access the FileX LevelX NOR instance using these structures when calling API functions directly (::p_api is not used). */
extern rm_filex_levelx_nor_instance_ctrl_t g_rm_filex_levelx_nor_ctrl;
extern const rm_filex_levelx_nor_cfg_t g_rm_filex_levelx_nor_cfg;

#ifndef g_rm_filex_levelx_nor_0_callback
void g_rm_filex_levelx_nor_0_callback(rm_filex_levelx_nor_callback_args_t *p_args);
#endif
#define G_FX_MEDIA0_MEDIA_MEMORY_SIZE (512)
#define G_FX_MEDIA0_VOLUME_NAME ("Volume 1")
#define G_FX_MEDIA0_NUMBER_OF_FATS (1)
#define G_FX_MEDIA0_DIRECTORY_ENTRIES (256)
#define G_FX_MEDIA0_HIDDEN_SECTORS (0)
#define G_FX_MEDIA0_TOTAL_SECTORS (57337)
#define G_FX_MEDIA0_BYTES_PER_SECTOR (512)
#define G_FX_MEDIA0_SECTORS_PER_CLUSTER (1)
#define G_FX_MEDIA0_VOLUME_SERIAL_NUMBER (12345)
#define G_FX_MEDIA0_BOUNDARY_UNIT (128)
#define IOPORT_CFG_NAME g_bsp_pin_cfg
#define IOPORT_CFG_OPEN R_IOPORT_Open
#define IOPORT_CFG_CTRL g_ioport_ctrl

/* IOPORT Instance */
extern const ioport_instance_t g_ioport;

/* IOPORT control structure. */
extern ioport_instance_ctrl_t g_ioport_ctrl;
void g_common_init(void);
FSP_FOOTER
#endif /* COMMON_DATA_H_ */
