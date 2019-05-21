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

ketCube_terminal_cmd_t* get_cmd_tree()
{
	return ketCube_terminal_commands;
}

ketCube_cfg_Module_t* get_module_list()
{
	return ketCube_modules_List;
}

size_t get_module_count()
{
	return ketCube_modules_CNT;
}

#ifdef __cplusplus
}
#endif
