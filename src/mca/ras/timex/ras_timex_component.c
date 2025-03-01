/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2012-2015 Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2015-2019 Intel, Inc.  All rights reserved.
 * Copyright (c) 2019      Research Organization for Information Science
 *                         and Technology (RIST).  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * Copyright (c) 2021-2022 Nanook Consulting.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "prte_config.h"
#include "constants.h"

#include "src/include/prte_socket_errno.h"
#include "src/mca/base/base.h"
#include "src/mca/base/prte_mca_base_var.h"
#include "src/util/pmix_net.h"

#include "src/mca/errmgr/errmgr.h"
#include "src/runtime/prte_globals.h"
#include "src/util/name_fns.h"

#include "ras_timex.h"
#include "src/mca/ras/base/ras_private.h"

/*
 * Local functions
 */
static int ras_timex_register(void);
static int ras_timex_open(void);
static int ras_timex_close(void);
static int prte_ras_timex_component_query(prte_mca_base_module_t **module, int *priority);

prte_ras_timex_component_t prte_ras_timex_component = {
    {
        /* First, the prte_mca_base_component_t struct containing meta
           information about the component itself */

        .base_version = {
            PRTE_RAS_BASE_VERSION_2_0_0,

            /* Component name and version */
            .mca_component_name = "timex",
            PRTE_MCA_BASE_MAKE_VERSION(component, PRTE_MAJOR_VERSION, PRTE_MINOR_VERSION,
                                        PMIX_RELEASE_VERSION),

            /* Component open and close functions */
            .mca_open_component = ras_timex_open,
            .mca_close_component = ras_timex_close,
            .mca_query_component = prte_ras_timex_component_query,
            .mca_register_component_params = ras_timex_register
        },
        .base_data = {
            /* The component is checkpoint ready */
            PRTE_MCA_BASE_METADATA_PARAM_CHECKPOINT
        },
    }
};

static int ras_timex_register(void)
{
    prte_mca_base_component_t *component = &prte_ras_timex_component.super.base_version;

    prte_ras_timex_component.timeout = 30;
    (void) prte_mca_base_component_var_register(
        component, "dyn_allocate_timeout", "Number of seconds to wait for timex dynamic allocation",
        PRTE_MCA_BASE_VAR_TYPE_INT, NULL, 0, PRTE_MCA_BASE_VAR_FLAG_NONE, PRTE_INFO_LVL_9,
        PRTE_MCA_BASE_VAR_SCOPE_READONLY, &prte_ras_timex_component.timeout);

    prte_ras_timex_component.dyn_alloc_enabled = false;
    (void) prte_mca_base_component_var_register(component, "enable_dyn_alloc",
                                                "Whether or not dynamic allocations are enabled",
                                                PRTE_MCA_BASE_VAR_TYPE_BOOL, NULL, 0,
                                                PRTE_MCA_BASE_VAR_FLAG_NONE, PRTE_INFO_LVL_9,
                                                PRTE_MCA_BASE_VAR_SCOPE_READONLY,
                                                &prte_ras_timex_component.dyn_alloc_enabled);

    prte_ras_timex_component.config_file = NULL;
    (void) prte_mca_base_component_var_register(component, "config_file",
                                                "Path to timex configuration file",
                                                PRTE_MCA_BASE_VAR_TYPE_STRING, NULL, 0,
                                                PRTE_MCA_BASE_VAR_FLAG_NONE, PRTE_INFO_LVL_9,
                                                PRTE_MCA_BASE_VAR_SCOPE_READONLY,
                                                &prte_ras_timex_component.config_file);

    prte_ras_timex_component.rolling_alloc = false;
    (void) prte_mca_base_component_var_register(component, "enable_rolling_alloc",
                                                "Enable partial dynamic allocations",
                                                PRTE_MCA_BASE_VAR_TYPE_BOOL, NULL, 0,
                                                PRTE_MCA_BASE_VAR_FLAG_NONE, PRTE_INFO_LVL_9,
                                                PRTE_MCA_BASE_VAR_SCOPE_READONLY,
                                                &prte_ras_timex_component.rolling_alloc);

    prte_ras_timex_component.use_all = false;
    (void) prte_mca_base_component_var_register(
        component, "use_entire_allocation",
        "Use entire allocation (not just job step nodes) for this application",
        PRTE_MCA_BASE_VAR_TYPE_BOOL, NULL, 0, PRTE_MCA_BASE_VAR_FLAG_NONE, PRTE_INFO_LVL_5,
        PRTE_MCA_BASE_VAR_SCOPE_READONLY, &prte_ras_timex_component.use_all);

    return PRTE_SUCCESS;
}

static int ras_timex_open(void)
{
    return PRTE_SUCCESS;
}

static int ras_timex_close(void)
{
    return PRTE_SUCCESS;
}

static int prte_ras_timex_component_query(prte_mca_base_module_t **module, int *priority)
{
    /* if I built, then timex support is available. If
     * I am not in a timex allocation, and dynamic alloc
     * is not enabled, then disqualify myself
     */
    if (NULL == getenv("TIMEX") && !prte_ras_timex_component.dyn_alloc_enabled) {
        /* disqualify ourselves */
        *priority = 0;
        *module = NULL;
        return PRTE_ERROR;
    }

    PRTE_OUTPUT_VERBOSE((2, prte_ras_base_framework.framework_output,
                         "%s ras:timex: available for selection",
                         PRTE_NAME_PRINT(PRTE_PROC_MY_NAME)));
    /* since only one RM can exist on a cluster, just set
     * my priority to something - the other components won't
     * be responding anyway
     */
    *priority = 50;
    *module = (prte_mca_base_module_t *) &prte_ras_timex_module;
    return PRTE_SUCCESS;
}
