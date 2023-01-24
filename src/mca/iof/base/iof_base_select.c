/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2007 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2015      Los Alamos National Security, LLC. All rights
 *                         reserved.
 * Copyright (c) 2019      Intel, Inc.  All rights reserved.
 * Copyright (c) 2020      Cisco Systems, Inc.  All rights reserved
 * Copyright (c) 2021      Nanook Consulting.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "prte_config.h"
#include "constants.h"

#include "src/mca/base/base.h"
#include "src/mca/mca.h"

#include "src/mca/errmgr/errmgr.h"
#include "src/util/proc_info.h"

#include "src/mca/iof/base/base.h"
#include "src/mca/iof/iof.h"

/**
 * Call the query function on all available components to find out if
 * they want to run.  Select the single component with the highest
 * priority.
 */
int prte_iof_base_select(void)
{
    prte_iof_base_component_t *best_component = NULL;
    prte_iof_base_module_t *best_module = NULL;
    int rc;

    /*
     * Select the best component
     */
    if (PRTE_SUCCESS
        != prte_mca_base_select("iof", prte_iof_base_framework.framework_output,
                                &prte_iof_base_framework.framework_components,
                                (prte_mca_base_module_t **) &best_module,
                                (prte_mca_base_component_t **) &best_component, NULL)) {
        /* this is a problem */
        return PRTE_ERR_NOT_FOUND;
    }

    /* Save the winner */
    prte_iof = *best_module;
    /* init it */
    if (NULL != prte_iof.init) {
        if (PRTE_SUCCESS != (rc = prte_iof.init())) {
            PRTE_ERROR_LOG(rc);
            return rc;
        }
    }

    return PRTE_SUCCESS;
}
