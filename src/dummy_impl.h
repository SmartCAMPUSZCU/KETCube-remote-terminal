/**
 * @file    dummy_impl.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains dummy implementations interface
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 University of West Bohemia in Pilsen
 * All rights reserved.</center></h2>
 *
 * Developed by:
 * The SmartCampus Team
 * Department of Technologies and Measurement
 * www.smartcampus.cz | www.zcu.cz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"),
 * to deal with the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimers.
 *
 *    - Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimers in the documentation
 *      and/or other materials provided with the distribution.
 *
 *    - Neither the names of The SmartCampus Team, Department of Technologies and Measurement
 *      and Faculty of Electrical Engineering University of West Bohemia in Pilsen,
 *      nor the names of its contributors may be used to endorse or promote products
 *      derived from this Software without specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
 */

/* this file is included in terminal.c */

/* this file contains dummy implementations and includes to satisfy linkage requirements of
 * symbols we don't really use in remote terminal application */

#ifdef __cplusplus
extern "C"
{
#endif

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_coreCfg.h"
#include "ketCube_modules.h"
#include "ketCube_batMeas.h"

#include "string.h"

ketCube_terminal_paramSet_t commandIOParams;

void ketCube_terminal_cmd_about(void) {}
void ketCube_terminal_cmd_help(void) {}
void ketCube_terminal_cmd_reload(void) {}
void ketCube_terminal_cmd_list(void) {}
void ketCube_terminal_cmd_enable(void) {}
void ketCube_terminal_cmd_disable(void) {}

inline void HW_GetUniqueId(uint8_t *id)
{
	memset(id, 0, 8);
}

void ketCube_terminal_UsartPrint(char *format, ...) {}

void ketCube_terminal_CoreSeverityPrintln(ketCube_severity_t severity, char *format, ...) {}

ketCube_cfg_Error_t ketCube_cfg_SetDefaults(ketCube_cfg_moduleIDs_t id, ketCube_cfg_AllocEEPROM_t target, ketCube_cfg_LenEEPROM_t len) { return KETCUBE_CFG_OK; }

ketCube_batMeas_battery_t ketCube_batMeas_batList[] = { { NULL, NULL, 0, 0 } };

#ifdef __cplusplus
extern "C"
}
#endif
