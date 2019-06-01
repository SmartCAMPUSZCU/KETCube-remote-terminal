/**
 * @file    impl_bridge.h
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains implementation bridge (between C and C++ code) interface
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

#ifndef CMD_LOOKUP_H
#define CMD_LOOKUP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "ketCube_terminal.h"
#include "ketCube_common.h"
#include "ketCube_cfg.h"
#include "ketCube_coreCfg.h"
#include "ketCube_modules.h"

	/* command input/output parameters */
	extern ketCube_terminal_paramSet_t commandIOParams;

	/* retrieves command tree (root level) */
	ketCube_terminal_cmd_t* get_cmd_tree();

	/* retrieves module list */
	ketCube_cfg_Module_t* get_module_list();

	/* retrieves module list length */
	size_t get_module_count();

#ifdef __cplusplus
}
#endif

#endif
