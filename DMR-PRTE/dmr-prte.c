#include "dmr-prte.h"

int DMR_slurm_request(int min, int max, int step, int pref)
{
	/*
    int action = 0, nc = 0, counter = 0;
    int dependentJobId;

	printf("(sergio): %s(%s,%d)\n", __FILE__, __func__, __LINE__);

	char *pID = getenv("SLURM_JOBID");
	printf("(sergio): %s(%s,%d)\n", __FILE__, __func__, __LINE__);
	uint32_t procID = atoi(pID);
	printf("(sergio): %s(%s,%d)\n", __FILE__, __func__, __LINE__);
	dependentJobId = procID;  
	job_info_msg_t *MasterInfo;
	printf("(sergio): %s(%s,%d)\n", __FILE__, __func__, __LINE__);
	slurm_load_job(&MasterInfo, procID, 0);
	printf("(sergio): %s(%s,%d)\n", __FILE__, __func__, __LINE__);
	printf("(sergio): %s(%s,%d) slurm nodes %d\n", __FILE__, __func__, __LINE__, MasterInfo->job_array->num_nodes);
	*/
    return 0;
}

int dmr_allocation_request(int min, int max, int pref, int *action, char *hosts){
	printf("(sergio): %s(%s,%d) %d, %d, %d\n", __FILE__, __func__, __LINE__, min, max, pref);
	
	char *pID = getenv("SLURM_JOBID");
	uint32_t procID = atoi(pID);
	int dependentJobId = procID;  
	job_info_msg_t *MasterInfo;
	printf("(sergio): %s(%s,%d) %d, %d, %d\n", __FILE__, __func__, __LINE__, min, max, pref);
	slurm_load_job(&MasterInfo, procID, 0);
	printf("(sergio): %s(%s,%d) %d, %d, %d\n", __FILE__, __func__, __LINE__, min, max, pref);
	sprintf(hosts, "%s", "hi sergio\0");
	printf("(sergio): %s(%s,%d) Job %d has %d nodes\n", __FILE__, __func__, __LINE__, procID, (int)MasterInfo->job_array->num_nodes);

	return 0;
}