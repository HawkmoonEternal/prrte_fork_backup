#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "slurm/slurm.h"
#include <sys/types.h>
#include <signal.h>
#define DMR_filename "tmp"
#define STR_SIZE 2048           /**< @brief Standard size for strings. **/

/**
 * @brief Struct to send/receive information with Slurm. 
 */
typedef struct select_jobinfo {
    uint32_t job_id; /**< @brief IN Job identifier. */
    uint32_t action; /**< @brief OUT Action identifier. */
    uint32_t min; /**< @brief IN Minimum number of processes. */
    uint32_t max; /**< @brief IN Maximum number of processes. */
    uint32_t preference; /**< @brief IN Preferred number of processes. */
    uint32_t step; /**< @brief IN Multiple of number of processes. */
    uint32_t currentNodes; /**< @brief IN Current number or processes. */
    uint32_t resultantNodes; /**< @brief OUT Resultant number of processes. */
    char hostlist[STR_SIZE]; /**< @brief IN List of hosts currently allocated. This is used to select the manager host for shrinking. */
} _dmr_slurm_jobinfo_t;

int DMR_slurm_request(int min, int max, int step, int pref);
int dmr_allocation_request(int min, int max, int pref, int *action, char *hosts);