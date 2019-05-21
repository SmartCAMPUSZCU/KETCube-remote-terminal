/* this file is included in terminal.c */

/* this file contains dummy implementations and includes to satisfy linkage requirements of
 * symbols we don't really use in remote terminal application */

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_coreCfg.h"
#include "ketCube_modules.h"
#include "ketCube_batMeas.h"

#include "string.h"

static ketCube_terminal_paramSet_t commandIOParams;

static void ketCube_terminal_cmd_about(void) {}
static void ketCube_terminal_cmd_help(void) {}
static void ketCube_terminal_cmd_reload(void) {}
static void ketCube_terminal_cmd_list(void) {}
static void ketCube_terminal_cmd_enable(void) {}
static void ketCube_terminal_cmd_disable(void) {}

void HW_GetUniqueId(uint8_t *id)
{
	memset(id, 0, 8);
}

void ketCube_terminal_UsartPrint(char *format, ...) {}

ketCube_batMeas_battery_t ketCube_batMeas_batList[] = { { NULL, NULL, 0, 0 } };
