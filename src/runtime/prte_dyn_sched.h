/*
 * Copyright (c) 2004-2008 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007      Sun Microsystems, Inc.  All rights reserved.
 * Copyright (c) 2007-2020 Cisco Systems, Inc.  All rights reserved
 * Copyright (c) 2015-2020 Intel, Inc.  All rights reserved.
 * Copyright (c) 2021-2022 Nanook Consulting.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

/**
 * @file
 *
 * Data server for PRTE
 */
#ifndef PRTE_DYN_SCHED_H
#define PRTE_DYN_SCHED_H

#include "prte_config.h"
#include "types.h"

#include "src/rml/rml_types.h"
#include "src/pmix/pmix-internal.h"
#include "src/prted/pmix/pmix_server_internal.h"
#include "src/prted/pmix/pmix_server.h"

void prte_dyn_sched_init();
void prte_dyn_sched_close();
void prte_dyn_sched_set_name(char * cli_input);
void prte_dyn_sched_set_proxy_binary(char *path);
void prte_dyn_sched_set_proxy_peer(pmix_proc_t peer);
pmix_status_t prte_dyn_sched_spawn_proxy();
void prte_dyn_sched_register_job(char * jobid);
void prte_dyn_sched_register_app(char * appid);
void prte_dyn_sched_allocation_request_nb(pmix_alloc_directive_t directive, pmix_info_t *info, size_t ninfo, pmix_info_cbfunc_t cbfunc, void *cbdata);

pmix_status_t prte_dmr_allocation_request_nb(pmix_alloc_directive_t directive, pmix_info_t *info, size_t ninfo, pmix_info_cbfunc_t cbfunc, void *cbdata);
 

#endif /* PRTE_DYN_SCHED_H */
