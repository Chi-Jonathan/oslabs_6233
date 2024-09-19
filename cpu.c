//
// Created by junot on 9/18/2024.
//
#include <locale.h>

#include "oslabs.h"
#include <stdio.h>


// Null PCB
const struct PCB NULLPCB = {0, 0, 0, 0, 0, 0, 0};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Helper functions

// Helper function


// Function to print out the PCB info
void print(struct PCB pcb) {
    printf("PID: %d\n arrival: %d\n total_bursttime: %d\n execution_starttime: %d\n execution_endtime: %d\n remaining_bursttime: %d\n process_priority: %d\n",
        pcb.process_id, pcb.arrival_timestamp, pcb.total_bursttime, pcb.execution_starttime, pcb.execution_endtime, pcb.remaining_bursttime, pcb.process_priority);
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Priority-based Preemptive Scheduling Implementation

struct PCB handle_process_arrival_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp) {
    struct PCB temp;

    // If a process isn't currently running make the new process the current process
    if(current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.remaining_bursttime;
        current_process = new_process;
    }

    // If the new process is of higher priority make it the current process and put the old current process into the queue
    else if(current_process.process_priority > new_process.process_priority && *queue_cnt < QUEUEMAX) {

        // Change the current process' EET and RBT then add it to the queue
        current_process.remaining_bursttime = current_process.execution_endtime - timestamp;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;

        //Change the new process' EST and EET then make it the current process
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.remaining_bursttime;
        current_process = new_process;
        *queue_cnt += 1;
    }

    // Put the new process in the queue
    else if(*queue_cnt < QUEUEMAX) {
        ready_queue[*queue_cnt] = new_process;
        *queue_cnt += 1;
    }

    return current_process;
}


struct PCB handle_process_completion_pp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {

    // If there is nothing in the queue just return the null pcb
    if(*queue_cnt == 0) return NULLPCB;

    // Initialize the variables for the next process and the index of the next process
    struct PCB next_process = ready_queue[0];
    int next_process_index = 0;

    // Search for the next process to run
    for (int i = 1; i < *queue_cnt; i++) {
        if(next_process.process_priority > ready_queue[i].process_priority) {
            next_process = ready_queue[i];
            next_process_index = i;
        }
    }

    // Shorten the queue
    if (next_process_index == *queue_cnt - 1) {
        *queue_cnt -= 1;
    }
    else {
        for (int i = next_process_index; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        *queue_cnt -= 1;
    }

    // The process' EST and EET are input
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Shortest-Remaining-Time-Next Preemptive Scheduling

struct PCB handle_process_arrival_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int time_stamp) {

    int rbt = current_process.execution_endtime - time_stamp;

    // If a process isn't currently running make the new process the current process
    if(current_process.process_id == 0) {
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.remaining_bursttime;
        current_process = new_process;
    }

    // If the new process has a lower remaining bursttime make it the current process and put the old current process into the queue
    else if(rbt > new_process.remaining_bursttime) {

        // Change the current process' EET and RBT then add it to the queue
        current_process.remaining_bursttime = current_process.execution_endtime - time_stamp;
        current_process.execution_starttime = 0;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;

        //Change the new process' EST and EET then make it the current process
        new_process.execution_starttime = time_stamp;
        new_process.execution_endtime = time_stamp + new_process.remaining_bursttime;
        current_process = new_process;
        *queue_cnt += 1;
    }

    // Put the new process in the queue
    else if(*queue_cnt < QUEUEMAX) {
        ready_queue[*queue_cnt] = new_process;
        *queue_cnt += 1;
    }

    return current_process;
}


struct PCB handle_process_completion_srtp(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp) {
    // If there is nothing in the queue just return the null pcb
    if(*queue_cnt == 0) return NULLPCB;

    // Initialize the variables for the next process and the index of the next process
    struct PCB next_process = ready_queue[0];
    int next_process_index = 0;

    // Search for the next process to run
    for (int i = 1; i < *queue_cnt; i++) {
        if(next_process.remaining_bursttime > ready_queue[i].remaining_bursttime) {
            next_process = ready_queue[i];
            next_process_index = i;
        }
    }

    // Shorten the queue
    if (next_process_index == *queue_cnt - 1) {
        *queue_cnt -= 1;
    }
    else {
        for (int i = next_process_index; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        *queue_cnt -= 1;
    }

    // The process' EST and EET are input
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Round-Robin Scheduling


struct PCB handle_process_arrival_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, struct PCB current_process, struct PCB new_process, int timestamp, int time_quantum) {

    // If a process isn't currently running make the new process the current process
    if(current_process.process_id == 0) {
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.remaining_bursttime;
        current_process = new_process;
    }

    // If the current process has gone through it's allotted quanta by the time the new process enters it the current process goes into the queue and the new process becomes the current process
    else if(timestamp - current_process.execution_starttime > time_quantum && *queue_cnt == 0) {
        current_process.remaining_bursttime = current_process.execution_endtime - timestamp;
        current_process.execution_endtime = 0;
        ready_queue[*queue_cnt] = current_process;
        *queue_cnt += 1;
        new_process.execution_starttime = timestamp;
        new_process.execution_endtime = timestamp + new_process.remaining_bursttime;
        current_process = new_process;
    }

    // Put the new process in the queue
    else if(*queue_cnt < QUEUEMAX) {
        ready_queue[*queue_cnt] = new_process;
        *queue_cnt += 1;
    }
    return current_process;
}


struct PCB handle_process_completion_rr(struct PCB ready_queue[QUEUEMAX], int *queue_cnt, int timestamp, int time_quantum) {
    // If there is nothing in the queue just return the null pcb
    if(*queue_cnt == 0) return NULLPCB;

    // Initialize the variables for the next process and the index of the next process
    struct PCB next_process = ready_queue[0];
    int next_process_index = 0;

    // Search for the next process to run
    for (int i = 1; i < *queue_cnt; i++) {
        if(next_process.arrival_timestamp > ready_queue[i].arrival_timestamp) {
            next_process = ready_queue[i];
            next_process_index = i;
        }
    }

    // Shorten the queue
    if (next_process_index == *queue_cnt - 1) {
        *queue_cnt -= 1;
    }
    else {
        for (int i = next_process_index; i < *queue_cnt - 1; i++) {
            ready_queue[i] = ready_queue[i + 1];
        }
        *queue_cnt -= 1;
    }

    // The process' EST and EET are input
    next_process.execution_starttime = timestamp;
    next_process.execution_endtime = timestamp + next_process.remaining_bursttime;

    return next_process;
}



