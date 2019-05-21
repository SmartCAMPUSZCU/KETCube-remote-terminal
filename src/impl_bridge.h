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
