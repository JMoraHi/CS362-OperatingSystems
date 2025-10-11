/*
 * By Jose Morales Hilario
 * COMP 362 - Operating Systems
 *
 * Centralized temperature monitor system
 */
//argument line begins with like: ./monitor 200 4 25 50 75 100
// for this ex, integrated temp is 200, 4 nodes, 
//node 1's temp is 25, node 1's temp is 25, node 2's temp is 50, node 3's temp is 75, node 4's temp is 100
// for each node, we give it a ID, 
// monitor creates all nodes and all of the inboxes. monitor takes X(4) messages from X(4) nodes. node opens queue(it assumes it is created) and waits for data. Node ends when it receives "Stable Temperature"
// all temps will be floats.
#include <sys/types.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "message.h"

#define oops(ermsg,erno) { perror(ermsg); exit(erno); }

pid_t
Fork(void)
{
    pid_t pid;

    //fork creates a new process by duping the calling one
    pid = fork(); //fork returns a process ID
    if(pid < 0){ //error checking if our fork call didn't return an error, 0 means child process, positive integer means parent process
          ferror("fork error"); //print error
          exit(EXIT_FAILURE);
    
    } 

    return pid;
}

int                           //                                   argv[0],         argv[1],       argv[2],    arg[3],
main(int argc, char** argv){ //argument line should always be: programName, integrated temp, amountOfNodes, node1Temp, node2Temp, ..., nodeXTemp

    mqd_t msqid;
    if (mq_unlink(MONITOR_QUEUE) == 0){ //unlink a message queue if it is up
        printf("Unlinked old queue: %s\n",MONITOR_QUEUE);
    }
    if ((msqid = mq_open(MONITOR_QUEUE, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, NULL)) < 0) { //open and link a message queue for Monitor while error checking 
        oops("Error opening a queue.", errno);
    }

    int nodeCounter = strtol(argv[2], NULL, 10); //amount nodes that we were asked to make, convert to str to long int
    float inteTemp = strtod(argv[1], NULL); //integrated temp for all nodes to reach, convert str to float

    mqd_t nodeIds[nodeCounter]; //acccounts for many nodes there are, this array is used to hold the handles of the message queues of the nodes so you can iterate through them for messages when you have them all.

    //loop to make the current node's name, message queue, then fork and make a new node process with child process
    for(int i = 3; (i - 3) < nodeCounter; i++){
        char qName[12]; //to hold node's message queue name
        char nodeNum[16]; //hold node number ID
        snprintf(nodeNum, 2, "%d",i - 2); //holds nodeNum as a string, EX: 1, 2, ..., 11, 12, ..., XX
        snprintf(qName, 12,"%s%02d", NODE_QUEUE, i - 2); //makes node's message queue name with proper name, NODE_QUEUE's str value is in "message.h", ex: /NODE_01 OR /NODE_02
        if(mq_unlink(qName) == 0){//unlink node message queue of the same name, in case it wasn't unlinked prior to program start. Unlink is successful if we get 0
            printf("Unlinked old queue: %s\n",qName);
        }
        if ((nodeIds[i-3] = mq_open(qName, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG, NULL)) < 0) { //open and link a message queue for current Node while error checking, array holds message queue handles of the nodes. 
        oops("Error opening a queue.", errno);
        }

        pid_t pid = Fork();
        
        if(pid == 0){ //if child program, go make a new node process
            //send the node process: node_iName, node_iTemp, integratedTemp.
            execlp("./node", "node", nodeNum, argv[i], argv[1],  NULL); //arg line: node program, nodeID, current node's temp, integrated temp 
        }
    }

    //PART 2: FEEDBACK LOOP
    double nodeTemps[nodeCounter]; //to hold all previous node's temp
    MESG currNode;
    for(int i = 0; i < nodeCounter; i++)
    {
        if(mq_receive(nodeIds[i], &currNode, (sizeof(MESG) * 1.5) , 1) == -1){
            perror("MONITOR: mq_receive - line 85\n");
            exit(EXIT_FAILURE);
        } //check each node's message queue handle and receive the message, which is a node struct |||| NOT RECEIVING NODE STRUCT INFO FROM NODE, ID HANDLE?
        nodeTemps[i] = currNode.temperature; //get current node's temp and add it to an array for later usage


        //mq_send(nodeIds[i], &currNode, sizeof(MESG), 1); //send modified node struct back to that specific node
        
        
    }
    // go thru each node's handles and receive the node
    //after receiving nodes, send new integrated temperature if they are not equal
    //if they are equal, send a "stable is TRUE" boolean value instead.
    


    //PART 3: TEARDOWN 



    return 0;
}


