#define MONITOR_QUEUE "/MONITOR"
#define NODE_QUEUE "/NODE_"
#include <stdbool.h>
#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

typedef struct mesg {
    bool stable;
    int nodeId;
    float temperature;
    float inteTemp; //added to make it easier to manage the variabe
} MESG;