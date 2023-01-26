#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#include "dmr-prte.h"

#define DURATION 3600
#define STEPS 10

int main()
{
    printf("(sergio): %s(%s,%d)  USAGE\n", __FILE__, __func__, __LINE__);
	
	DMR_slurm_request(1,8,2,4);
   
    return 0;
}
