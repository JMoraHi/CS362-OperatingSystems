/*
 * By Jose Morales Hilario
 * COMP 362 - Operating Systems
 *
 * Individual temperature node reporter
 */
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

int                           //argv[0],argv[1],argv[2],arg[3],
main(int argc, char** argv){  //programName, nodeId, nodeTemp, integrated temp
  
    float nodeTemp = strtod(argv[2], NULL); //converts 2nd arg to float
    float inteTemp = strtod(argv[3], NULL); //converts 3rd arg to float
    int nodeNum = strtol(argv[1], NULL, 10); //converts 1st arg to int
    char mqName[12]; //to hold message queue's name to find later
    MESG node; //create node struct
    
    snprintf(mqName, 12, "%s%02d", NODE_QUEUE, nodeNum); //makes the node message queue name, EX: /NODE_01
    
    //1. Opens the monitor message queue for writing and the node message queue for reading
    mqd_t msqid = mq_open(MONITOR_QUEUE, O_RDWR); //we assume the message queue for MONITOR is made already, we open it in read and write mode
    mqd_t msqidNode = mq_open(mqName, O_RDWR); //we assume the message queue for this node is made already, we open it in read and write mode
   
    float previous_node_temp = nodeTemp;
    float new_node_temp = previous_node_temp;
    float new_integrated_temp = inteTemp;
    node.nodeId = nodeNum; //node is given its ID Number
    node.temperature = nodeTemp; //node is given its temperature
    node.inteTemp = new_integrated_temp; //node is given the current integrated temperature

    //2. Loop until monitor says the system is stable
    while(node.stable != true){
        mq_send(msqid, &node, sizeof(MESG), 1); // a) Send the current node temperature and node's ID
        //mq_send(messageQueue, actualNode, sizeOfTheNode, priority of the message)
        mq_receive(msqid, &node, sizeof(MESG), 1); // b) Receive the new integrated temperature
        //mq_receive(messageQueue, actualNodeChanged, sizeOfTheNode, priority of the message). Should have the new changed values of bool stable OR integratedTemp

        if(node.stable == true){ //c) If the system is stable, go to the next step. Otherwise, continue
            goto done;
        } else {
            previous_node_temp = node.temperature; //set prev temp for formula
            new_integrated_temp = node.inteTemp; //new integrated temp, that we got from monitor, for formuala
            new_node_temp = (previous_node_temp * 3 + 2 * new_integrated_temp); // 5; d) Calculate new temperature

            node.temperature = new_node_temp; //e) Report new temperature to monitor
            //Loops and returns the new node temperature
        }   
        done:

    }
     
return 0; //3. Exit
}
