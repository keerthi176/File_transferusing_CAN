/* generated common source file - do not edit */
#include "common_data.h"
qspi_instance_ctrl_t g_qspi0_ctrl;

static const spi_flash_erase_command_t g_qspi0_erase_command_list[] =
{
#if 4096 > 0
  { .command = 0x20, .size = 4096 },
#endif
#if 32768 > 0
  { .command = 0x52, .size = 32768 },
#endif
#if 65536 > 0
  { .command = 0xD8, .size = 65536 },
#endif
#if 0xC7 > 0
  { .command = 0xC7, .size = SPI_FLASH_ERASE_SIZE_CHIP_ERASE },
#endif
        };
static const qspi_extended_cfg_t g_qspi0_extended_cfg =
{ .min_qssl_deselect_cycles = QSPI_QSSL_MIN_HIGH_LEVEL_4_QSPCLK, .qspclk_div = QSPI_QSPCLK_DIV_2, };
const spi_flash_cfg_t g_qspi0_cfg =
{ .spi_protocol = SPI_FLASH_PROTOCOL_QPI,
  .read_mode = SPI_FLASH_READ_MODE_FAST_READ_QUAD_IO,
  .address_bytes = SPI_FLASH_ADDRESS_BYTES_4,
  .dummy_clocks = SPI_FLASH_DUMMY_CLOCKS_DEFAULT,
  .page_program_address_lines = SPI_FLASH_DATA_LINES_1,
  .page_size_bytes = 256,
  .page_program_command = 0x02,
  .write_enable_command = 0x06,
  .status_command = 0x05,
  .write_status_bit = 0,
  .xip_enter_command = 0x20,
  .xip_exit_command = 0xFF,
  .p_erase_command_list = &g_qspi0_erase_command_list[0],
  .erase_command_list_length = sizeof(g_qspi0_erase_command_list) / sizeof(g_qspi0_erase_command_list[0]),
  .p_extend = &g_qspi0_extended_cfg, };
/** This structure encompasses everything that is needed to use an instance of this interface. */
const spi_flash_instance_t g_qspi0 =
{ .p_ctrl = &g_qspi0_ctrl, .p_cfg = &g_qspi0_cfg, .p_api = &g_qspi_on_spi_flash, };
rm_levelx_nor_spi_instance_ctrl_t g_rm_levelx_nor_spi0_ctrl;

#define RA_NOT_DEFINED 0xFFFFFFFF
rm_levelx_nor_spi_cfg_t g_rm_levelx_nor_spi0_cfg =
{
#if (RA_NOT_DEFINED != g_qspi0)
  .p_lower_lvl = &g_qspi0,
  .base_address = BSP_FEATURE_QSPI_DEVICE_START_ADDRESS,
#elif (RA_NOT_DEFINED != RA_NOT_DEFINED)
    .p_lower_lvl        = &RA_NOT_DEFINED,
    .base_address       = BSP_FEATURE_OSPI_DEVICE_RA_NOT_DEFINED_START_ADDRESS,
#else
    .p_lower_lvl        = &RA_NOT_DEFINED,
    .base_address       = BSP_FEATURE_OSPI_B_DEVICE_RA_NOT_DEFINED_START_ADDRESS,
#endif
  .address_offset = 0,
  .size = 0x2000000, .poll_status_count = 0xFFFFFFFF, .p_context = &g_rm_filex_levelx_nor_ctrl, .p_callback =
          rm_filex_levelx_nor_spi_callback };
#undef RA_NOT_DEFINED

#ifndef LX_DIRECT_READ
#define FSP_LX_READ_BUFFER_SIZE_WORDS (128U)
ULONG g_rm_levelx_nor_spi0_read_buffer[FSP_LX_READ_BUFFER_SIZE_WORDS] =
{ 0 };
#endif

/** WEAK system error call back */
#if defined(__ICCARM__)
#define g_rm_levelx_nor_spi0_system_error_WEAK_ATTRIBUTE
#pragma weak g_rm_levelx_nor_spi0_system_error  = g_rm_levelx_nor_spi0_system_error_internal
#elif defined(__GNUC__)
#define g_rm_levelx_nor_spi0_system_error_WEAK_ATTRIBUTE   \
        __attribute__ ((weak, alias("g_rm_levelx_nor_spi0_system_error_internal")))
#endif

UINT g_rm_levelx_nor_spi0_system_error(UINT error_code)
g_rm_levelx_nor_spi0_system_error_WEAK_ATTRIBUTE;
/*****************************************************************************************************************//**
 * @brief      This is a weak example initialization error function.  It should be overridden by defining a user  function
 *             with the prototype below.
 *             - void g_rm_levelx_nor_spi0_system_error(UINT error_code)
 *
 * @param[in]  error_code represents the error that occurred.
 **********************************************************************************************************************/
UINT g_rm_levelx_nor_spi0_system_error_internal(UINT error_code);
UINT g_rm_levelx_nor_spi0_system_error_internal(UINT error_code)
{
    FSP_PARAMETER_NOT_USED (error_code);

    /** An error has occurred. Please check function arguments for more information. */
    BSP_CFG_HANDLE_UNRECOVERABLE_ERROR (0);

    return LX_ERROR;
}

/* LevelX NOR instance "Read Sector" service */
static UINT g_rm_levelx_nor_spi0_read(ULONG *flash_address, ULONG *destination, ULONG words);
static UINT g_rm_levelx_nor_spi0_read(ULONG *flash_address, ULONG *destination, ULONG words)
{
    fsp_err_t err;

    err = RM_LEVELX_NOR_SPI_Read (&g_rm_levelx_nor_spi0_ctrl, flash_address, destination, words);
    if (FSP_SUCCESS != err)
    {
        return LX_ERROR;
    }

    return LX_SUCCESS;
}

/* LevelX NOR instance "Write Sector" service */
static UINT g_rm_levelx_nor_spi0_write(ULONG *flash_address, ULONG *source, ULONG words);
static UINT g_rm_levelx_nor_spi0_write(ULONG *flash_address, ULONG *source, ULONG words)
{
    fsp_err_t err;

    err = RM_LEVELX_NOR_SPI_Write (&g_rm_levelx_nor_spi0_ctrl, flash_address, source, words);
    if (FSP_SUCCESS != err)
    {
        return LX_ERROR;
    }

    return LX_SUCCESS;
}

/* LevelX NOR instance "Block Erase" service */
static UINT g_rm_levelx_nor_spi0_block_erase(ULONG block, ULONG block_erase_count);
static UINT g_rm_levelx_nor_spi0_block_erase(ULONG block, ULONG block_erase_count)
{
    fsp_err_t err;

    err = RM_LEVELX_NOR_SPI_BlockErase (&g_rm_levelx_nor_spi0_ctrl, block, block_erase_count);
    if (FSP_SUCCESS != err)
    {
        return LX_ERROR;
    }

    return LX_SUCCESS;
}

/* LevelX NOR instance "Block Erased Verify" service */
static UINT g_rm_levelx_nor_spi0_block_erased_verify(ULONG block);
static UINT g_rm_levelx_nor_spi0_block_erased_verify(ULONG block)
{
    fsp_err_t err;

    err = RM_LEVELX_NOR_SPI_BlockErasedVerify (&g_rm_levelx_nor_spi0_ctrl, block);
    if (FSP_SUCCESS != err)
    {
        return LX_ERROR;
    }

    return LX_SUCCESS;
}

/* LevelX NOR instance "Driver Initialization" service */
UINT g_rm_levelx_nor_spi0_initialize(LX_NOR_FLASH *p_nor_flash)
{
    fsp_err_t err;

    g_rm_levelx_nor_spi0_cfg.p_lx_nor_flash = p_nor_flash;

    /* Open the rm_levelx_nor_spi driver */
    err = RM_LEVELX_NOR_SPI_Open (&g_rm_levelx_nor_spi0_ctrl, &g_rm_levelx_nor_spi0_cfg);
    if (FSP_SUCCESS != err)
    {
        return LX_ERROR;
    }

#ifndef LX_DIRECT_READ
    /** lx_nor_flash_sector_buffer is used only when LX_DIRECT_READ disabled */
    p_nor_flash->lx_nor_flash_sector_buffer = g_rm_levelx_nor_spi0_ReadBuffer;
#endif

    p_nor_flash->lx_nor_flash_driver_read = g_rm_levelx_nor_spi0_read;
    p_nor_flash->lx_nor_flash_driver_write = g_rm_levelx_nor_spi0_write;
    p_nor_flash->lx_nor_flash_driver_block_erase = g_rm_levelx_nor_spi0_block_erase;
    p_nor_flash->lx_nor_flash_driver_block_erased_verify = g_rm_levelx_nor_spi0_block_erased_verify;
    p_nor_flash->lx_nor_flash_driver_system_error = g_rm_levelx_nor_spi0_system_error;

    return LX_SUCCESS;
}

/* LevelX NOR instance "Driver Close" service */
fsp_err_t g_rm_levelx_nor_spi0_close()
{
    return RM_LEVELX_NOR_SPI_Close (&g_rm_levelx_nor_spi0_ctrl);
}
LX_NOR_FLASH g_lx_nor0;
rm_filex_levelx_nor_instance_ctrl_t g_rm_filex_levelx_nor_ctrl;

const rm_filex_levelx_nor_cfg_t g_rm_filex_levelx_nor_cfg =
{ .close = g_rm_levelx_nor_spi0_close, .nor_driver_initialize = g_rm_levelx_nor_spi0_initialize, .p_nor_flash =
          &g_lx_nor0,
  .p_nor_flash_name = "g_rm_filex_levelx_nor_0", .p_callback = g_rm_filex_levelx_nor_0_callback, .p_context = NULL };

const rm_filex_levelx_nor_instance_t g_rm_filex_levelx_nor_instance =
{ .p_ctrl = &g_rm_filex_levelx_nor_ctrl, .p_cfg = &g_rm_filex_levelx_nor_cfg };
ioport_instance_ctrl_t g_ioport_ctrl;
const ioport_instance_t g_ioport =
{ .p_api = &g_ioport_on_ioport, .p_ctrl = &g_ioport_ctrl, .p_cfg = &g_bsp_pin_cfg, };
void g_common_init(void)
{
}
