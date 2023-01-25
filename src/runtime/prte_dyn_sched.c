/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2007-2020 Cisco Systems, Inc.  All rights reserved
 * Copyright (c) 2012-2016 Los Alamos National Security, LLC.
 *                         All rights reserved
 * Copyright (c) 2015-2020 Intel, Inc.  All rights reserved.
 * Copyright (c) 2017-2018 Research Organization for Information Science
 *                         and Technology (RIST).  All rights reserved.
 * Copyright (c) 2021-2022 Nanook Consulting.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include "prte_config.h"
#include "constants.h"
#include "types.h"

#include <string.h>

#ifdef HAVE_SYS_TIME_H
#    include <sys/time.h>
#endif

#include "src/class/pmix_pointer_array.h"
#include "src/pmix/pmix-internal.h"
#include "src/util/pmix_argv.h"
#include "src/util/output.h"

#include "src/mca/errmgr/errmgr.h"
#include "src/rml/rml.h"
#include "src/runtime/prte_globals.h"
#include "src/runtime/prte_wait.h"
#include "src/util/name_fns.h"

#include "src/runtime/prte_setop_server.h"

static prte_dyn_sched_t dyn_sched = PRTE_DYN_SCHED_NONE;
static pmix_proc_t dyn_sched_peer;
char *dyn_sched_binary = NULL;

void prte_dyn_sched_init(){
    PMIX_PROC_CONSTRUCT(&dyn_sched_peer);
}

void prte_dyn_sched_close(){
    PMIX_PROC_DESTRUCT(&dyn_sched_peer);
    free(dyn_sched_binary);
}

void prte_dyn_sched_set_name(char * cli_input){
    if(0 == strcmp(cli_input, "dmr_slurm")){
        dyn_sched = PRTE_DYN_SCHED_DMR;
    }else if(0 == strcmp(cli_input, "deepsea-slurm")){
        dyn_sched = PRTE_DYN_SCHED_DEEPSEA;
    }else{
        dyn_sched = PRTE_DYN_SCHED_NONE;
    }
}

void prte_dyn_sched_set_proxy_binary(char *path){
    dyn_sched_binary = strdup(path);
}

pmix_status_t prte_dyn_sched_spawn_proxy(){
    char *args[1];
    args[0] = "--proxy";
    if(NULL == dyn_sched_binary){
        printf("Could not spawn dyn sched. No binary set.\n");
        return PMIX_ERR_BAD_PARAM;
    }

    char *myargs [] = { NULL };
    char *myenv [] = { NULL };

    printf ("Parent: Hello, World!\n");

    int pid = fork ();

    if (pid == 0) {

        // I am the child
        execve (dyn_sched_binary, args, NULL);
    }

    return PMIX_SUCCESS;
}

void prte_dyn_sched_set_proxy_peer(pmix_proc_t peer){
    PMIX_PROC_LOAD(&dyn_sched_peer, peer.nspace, peer.rank);
}

//prte_dyn_sched_register_alloc_req_response(alloc_cbfunc);
//pmix_info_cbfunc_t cbfunc

void free_info(void* cbdata){
    pmix_info_t *info = (pmix_info_t *)cbdata;
    PMIX_INFO_FREE(info, 1);
}

pmix_status_t prte_dmr_allocation_request_nb(pmix_alloc_directive_t directive, pmix_info_t *info, size_t ninfo, pmix_info_cbfunc_t cbfunc, void *cbdata){
    size_t n, ncbinfo = 0;
    uint64_t num_procs = 0;
    char hosts[2048];
    int action;
    pmix_status_t rc;
    pmix_info_t *cbinfo = NULL;

    printf("prte_dmr_allocation_request_nb: start\n");

    for(n = 0; n < ninfo; n++){
        if(PMIX_CHECK_KEY(&info[n], PMIX_ALLOC_NUM_CPUS)){
            num_procs = info[n].value.data.uint64;
        }
    }

    printf("prte_dmr_allocation_request_nb: requesting %d processes\n", (int) num_procs);

    if(0 == num_procs){
        return PMIX_ERR_BAD_PARAM;
    }


    printf("prte_dmr_allocation_request_nb: sending request\n");
    rc = dmr_allocation_request((int) num_procs, (int) num_procs, (int) num_procs, &action, hosts);
    printf("prte_dmr_allocation_request_nb: request returned %d\n", rc);
    if(rc == PMIX_SUCCESS){
        PMIX_INFO_CREATE(cbinfo, 1);
        PMIX_INFO_LOAD(&cbinfo[0], PMIX_NODE_LIST, hosts, PMIX_STRING);
        ncbinfo = 1;
    }

    printf("prte_dmr_allocation_request_nb: calling alloc cbfunc\n", rc);
    cbfunc(rc, cbinfo, ncbinfo, cbdata, free_info, (void *)info);
    printf("prte_dmr_allocation_request_nb: finsihed\n", rc);


    return rc;
    
}

void prte_dyn_sched_allocation_request_nb(pmix_alloc_directive_t directive, pmix_info_t *info, size_t ninfo, pmix_info_cbfunc_t cbfunc, void *cbdata){
    switch(dyn_sched){
        case PRTE_DYN_SCHED_DEEPSEA:
            return;
    }
}