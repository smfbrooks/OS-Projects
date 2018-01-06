// processor.cpp
// TE 4348.501 Fall '17 Project 1: Forking
// Author: Stephen-Michael Brooks

/*
This program will read an input file (data.txt) which contains a list of processes/tasks that have three attributes:
PID, Pages Required, Time Required. For all intents and purposes, Time Required is known as "Cycles Required" in this
program. After reading and saving the input file contents into a queue, the program will simulate a processor by
completing the tasks in order from PID 1 to 25. After processing the tasks in order, a custom algorithm will attempt
to reduce the total number of cycles to complete all of the processes. There is no requirement for the order at which
they are processed.


Libraries:
    queue::queue    :   http://www.cplusplus.com/reference/queue/queue/queue/


Output:
    Process.pid     :   Process ID
    Process.pages   :   Number of pages required by the Process
    Process.cycles  :   Number of cycles required by the Process


Structure(s):
    Process :    Describes a task.
        attributes:
            PID
            Pages
            Cycles


Function(s):
    main():
        Reads the input data from a file and loads the data into a queue.
    add_more(queue<Process>* nq, queue<Process>* rq):
        Attepmts to add another Process to the running queue by searching the the Process with the highest
        possible number of pages that can be accomodated by the system memory.
    add_next(queue<Process>* nq, queue<Process>* rq):
        Attempts to add the next Process in the new queue to the running queue if the system memory
        can accomodate the Process's memory requirements.
    fcfs(queue<Process> nq):
        Performs tasks that are in the running queue.
    lsumf(queue<Process> nq):
        Performs tasks that are in the running queue.
*/

#include <stdio.h>
#include <stdexcept>
#include <queue>
#include <fstream>
#include <string>

using namespace std;

int avail_frames = 32;

/*
Structure: Process
    Author(s): Stephen-Michael Brooks
    This structure describes a task. It contains the information needed by the program to complete it.
*/  
struct Process {
    int pid, pages, cycles;
};

/*
Fucntion: add_more
    Author(s): Stephen-Michael Brooks
    Attepmts to add another Process to the running queue by searching the the Process with the highest
    possible number of pages that can be accomodated by the system memory.
    args:
        nq (input)  :   "new queue"
            type: queue<Process>*
            desc: A pointer to the new queue where Processes have not been added to the running queue, yet.
        rq (input)  :   "run queue"
            type: queue<Process>*
            desc: A pointer to the running queue where Processes that are currently running reside.
*/  
void add_more(queue<Process>* nq, queue<Process>* rq) {
    
    int desired_sum = 0;
    for(int j = 32; j > desired_sum+1; j--) {   // outer loop to add nth Process in the queue to the rest in the queue
                                                // until the largest pair of pages is achieved
        for(int i = 0; i < (*nq).size(); i++) { // iterate thorugh all Process(es) in the new queue to look for a pair
            if (avail_frames - (*nq).front().pages == desired_sum) {    // a pair has been found
                avail_frames -= (*nq).front().pages;    // allocate memory
                printf("Add PID %d\tPages: %d\tRemaining frames: %d\n",
                       (*nq).front().pid, (*nq).front().pages, avail_frames);
                (*rq).push((*nq).front());  // move Process from the new queue to the running queue
                (*nq).pop();
                return;
            }
            else {
                (*nq).push((*nq).front());  // move the top Process to the end
                (*nq).pop();
            }
        }
    desired_sum++;  // a pair was not found, so loosen the requirement by 1 and try again
    }
}


/*
Fucntion: add_next
    Author(s): Stephen-Michael Brooks
    Attempts to add the next Process in the new queue to the running queue if the system memory
    can accomodate the Process's memory requirements
    args:
        nq (input)  :   "new queue"
            type: queue<Process>*
            desc: A pointer to the new queue where Processes have not been added to the running queue, yet.
        rq (input)  :   "run queue"
            type: queue<Process>*
            desc: A pointer to the running queue where Processes that are currently running reside.
*/  
void add_next(queue<Process>* nq, queue<Process>* rq) {
    
    while (avail_frames - (*nq).front().pages >= 0 && !(*nq).empty()) { 
        // add all the Processes from the new queue that can fit
        (*rq).push((*nq).front());
        avail_frames -= (*nq).front().pages;
        printf("Add PID %d\tPages: %d\tRemaining frames: %d\n",
               (*nq).front().pid, (*nq).front().pages, avail_frames);
        (*nq).pop();
    }
}


/*
Fucntion: fcfs
    Author(s): Stephen-Michael Brooks
    "First Come First Serve" performs tasks that are in the running queue. It only serves Processes in order that they are orginally
    given in the new queue. Also provides output to STDOUT for the user to monitor the function's activity.
    args:
        nq (input)  :   "new queue"
            type: queue<Process>*
            desc: A pointer to the new queue where Processes have not been added to the running queue, yet.
*/  
void fcfs(queue<Process> nq) {
    int cycles = 0;

    queue<Process> rq;
    
    add_next(&nq, &rq); // add Process(es) to the running queue
    
    while (true) {
        if (rq.empty() && nq.empty()) { break; }
        cycles++;
        
        for (int i = 0; i < rq.size(); i++) {
            // decrement all running Processes
            if (rq.front().cycles == 1) {
                // a Process has finished. free the memory and add it to the exit queue
                avail_frames += rq.front().pages;   // free memory
                rq.pop();
                add_next(&nq, &rq); // attempt to add the next Process in the new queue to the running queue
            }
            else {
                rq.front().cycles--;    // decrememnt the cycles required of the currently served Process(es)
                rq.push(rq.front());    // move the top Process to the end
                rq.pop();
            }
        }
    }
    printf("It took %d cycles (time units) to finish all Processes in the queue using the ", cycles);
    printf("First Come First Serve algorithm.\n\n");
}


/*
Fucntion: lsumf
    Author(s): Stephen-Michael Brooks
    "Largest Sum First" performs tasks that are in the running queue. It serves processes that are in the running
     queue. It is paired with the function "add_more" to try to serve pairs of Processes that are close to the
     maximum memory in the system. Also provides output to STDOUT for the user to monitor the function's activity.
    args:
        nq (input)  :   "new queue"
            type: queue<Process>*
            desc: A pointer to the new queue where Processes have not been added to the running queue, yet.
*/ 
void lsumf(queue<Process> nq) {
    queue<Process> rq;
    int cycles = 0;     // tracks the number of iterations the program must take to finish all Processes

    rq.push(nq.front());    //  add the first Process to the running queue
    avail_frames -= nq.front().pages;   // allocate memory for a process
    printf("Add PID %d\tPages: %d\tRemaining frames: %d\n",
           nq.front().pid, nq.front().pages, avail_frames);
    nq.pop();   // remove the Process from the new queue
    add_more(&nq, &rq);     // attempt to add another Process to the running queue

    while(true) {
        if (nq.empty() && rq.empty()) { break; }    // break the loop if all Processes are finished
        cycles++;

        for(int k = 0; k < rq.size(); k++) {
            if (rq.front().cycles == 1) {   // if the front Process is finished, remove it from the running queue
                avail_frames += rq.front().pages;   // free memory
                rq.pop();
                add_more(&nq, &rq); // attempt to add another Process to the running queue
            }
            else {
                rq.front().cycles--;    // decrement the cycles of the currently served Process(es)
                rq.push(rq.front());    // move the top process to the end
                rq.pop();
            }
        }
    }
    printf("It took %d cycles (time units) to finish all Processes in the queue using the ", cycles);
    printf("Largest Sum First algorithm.\n\n");
}


/*
Fucntion: main
    Author(s): Stephen-Michael Brooks
    Reads the input data from a file and loads the data into a queue. If the function cannot read the input file,
    it will exit with error code 1.
*/ 
int main() {
    queue<Process> new_q, run_q, suspend_q, exit_q;
    Process p;

    ifstream infile("data.txt");
    
    if (!infile.is_open()) {    // if the file cannot be read, exit with code 1
        printf("Error opening data.txt. Exiting...");
        return 1;
    }

    while (infile.is_open()) {
        try {
            string pid_s, pages_s, cycles_s;

            getline(infile, pid_s, '\t');   // save "pid" to pid_s
            getline(infile, pages_s, '\t'); // save "pages" req to pages_s
            getline(infile, cycles_s);      // save "time required" to cycles_s

            p.pid = stoi(pid_s);        // convert pid_s to int and save to Process p.pid
            p.pages = stoi(pages_s);    // convert pages_s to int and save to Process p.pages
            p.cycles = stoi(cycles_s);  // convert cycles_s to int and save to Process p.cycles
            new_q.push(p);  // push new process onto the new queue

        }
        catch (invalid_argument) { break; } // when the EOF has been detected, break the read loop
    }

    infile.close();

    fcfs(new_q);    // begin processing the new queue with the first come first serve algorithm
    lsumf(new_q);   // begin processing the new queue with the largest sum first algorithm
    
    return 0;
}