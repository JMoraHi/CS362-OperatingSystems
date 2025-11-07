//
// Created by aj on 10/24/19.
//

#include "../inc/inverted.h"

PROC *table;

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("USE: <prog name> <memory size> <frame size> <num of processes> <page upper bound>\n");
        exit(EXIT_FAILURE);
    }

    int memSize = strtod(argv[1], NULL);
    int frameSize = strtod(argv[2], NULL);
    int numOfProcesses = strtod(argv[3], NULL);
    int pageNumberUpperBound = strtod(argv[4], NULL);

    int numOfFrames = memSize/frameSize;

    printf("=================================\n");
    printf("SIMULATION PARAMETERS\n");
    printf("=================================\n");
    printf("Memory size: %d\n", memSize);
    printf("Frame size: %d\n", frameSize);
    printf("Number of processes: %d\n", numOfProcesses);
    printf("Upper bound on page numbers: %d\n", pageNumberUpperBound);
    printf("Number of frames (computed): %d\n", numOfFrames);

    // test creation
    initInverted(&table, memSize, frameSize);

    srand(time(NULL));

    // storage for address translations and offsets used to get them (pid and page are in the table)
    long address[numOfFrames];
    int offset[numOfFrames];

    printf("=================================\n");
    printf("filling the table chronologically\n");
    printf("=================================\n");
    struct timespec time;
    for (int i = 0; i < numOfFrames; i++)
    {
        table[i].pid = rand() % (numOfProcesses - 0 + 1) + 0; // 0 is the lower bound; numOfProcesses - upper
        table[i].page = rand() % (pageNumberUpperBound - 0 + 1) + 0; // 0 is the lower bound; pageNumberUpperBound - upper
        offset[i] = rand() % (frameSize - 0 + 1) + 0; // 0 is the lower bound; frameSize - upper
        clock_gettime(CLOCK_REALTIME, &time);
        table[i].timeStamp = time.tv_sec * 1000000000 + time.tv_nsec;
        address[i] = translate(table, table[i].pid, table[i].page, offset[i]);
    }

    printInverted(table);

    printf("==================================\n");
    printf("translate with no page replacement\n");
    printf("==================================\n");
    for (int i = 0; i < numOfFrames; i++)
    {
        int pid = table[i].pid;
        int page = table[i].page;
        long addr = translate(table, pid, page, offset[i]);
        printf("translate(%d, %d, %d) = %ld\n", pid, page, offset[i], addr);
        if (addr == address[i])
            printf("CORRECT\n");
        else
            printf("ERROR: retrieved address %ld != %ld that stored earlier\n", addr, address[i]);
    }

    printInverted(table);

    printf("======================================\n");
    printf("translate with forced page replacement\n");
    printf("======================================\n");
    for (int i = 0; i < numOfFrames; i++)
    {
        int pid = (rand() % ((numOfProcesses*2) - numOfProcesses + 1) + numOfProcesses); // force different processes
        int page = rand() % pageNumberUpperBound;
        int new_offset;
        do
        {
          new_offset = rand() % frameSize;
        } while (new_offset == offset[i]); // force different offset

        long addr = translate(table, pid, page, new_offset);
        printf("translate(%d, %d, %d) = %ld\n", pid, page, new_offset, addr);
        if (addr != address[i])
            printf("CORRECT\n");
        else
            printf("ERROR: slot #%d has not been replaced as expected\n", i);
    }

    // stress test

    printInverted(table);

    printf("====================================\n");
    printf("translate with some page replacement\n");
    printf("====================================\n");
    for (int i = numOfFrames; i < numOfFrames * 2; i++)
    {
        int pid = rand() % (numOfProcesses*2 - numOfProcesses + 1) + numOfProcesses; // force different processes
        int page = rand() % pageNumberUpperBound;
        int offset = rand() % frameSize;
        printf("====================================\n");
        printf("translate(%d, %d, %d) = %ld\n", pid, page, offset, translate(table, pid, page, offset));
        printf("====================================\n");
        printInverted(table);
    }
    
    exit(EXIT_SUCCESS);
}
