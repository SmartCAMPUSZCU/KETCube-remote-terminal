/**
 * @file    impl_bridge.c
 * @author  Martin Ubl
 * @version 0.1
 * @date    2019-05-27
 * @brief   This file contains implementation bridge (between C and C++ code)
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

#ifdef __cplusplus
extern "C"
{
#endif

#include "impl_bridge.h"

#include "dummy_impl.h"

/* dummy macro for remote terminal - as we don't use callbacks in our code,
 * there's no point in them being defined and linked (which would cause errors);
 * therefore, we disable callback setting, thus removing parts of code from compilation */
#define DEF_MODULE(name, descr, moduleId, initFn, sleepEnter, sleepExit, \
                   getSensData, sendData, recvData, processData, cfgStruct) \
                  { \
                      ((char*) &(name)),\
                      ((char*) &(descr)), \
                      moduleId, \
                      (ketCube_cfg_ModInitFn_t)NULL, \
                      (ketCube_cfg_ModVoidFn_t)NULL, \
                      (ketCube_cfg_ModVoidFn_t)NULL, \
                      (ketCube_cfg_ModDataFn_t)NULL, \
                      (ketCube_cfg_ModDataFn_t)NULL, \
                      (ketCube_cfg_ModVoidFn_t)NULL, \
                      (ketCube_cfg_ModDataPtrFn_t)NULL, \
                      (ketCube_cfg_ModuleCfgByte_t *)NULL, \
                      (ketCube_cfg_LenEEPROM_t)0, \
                      (ketCube_cfg_AllocEEPROM_t)0 \
                   }

#include "ketCube_cmdList.c"
#include "ketCube_moduleList.c"

ketCube_terminal_cmd_t* get_cmd_tree() {
	return ketCube_terminal_commands;
}

ketCube_cfg_Module_t* get_module_list() {
	return ketCube_modules_List;
}

size_t get_module_count() {
	return ketCube_modules_CNT;
}

#ifdef __cplusplus
}
#endif
