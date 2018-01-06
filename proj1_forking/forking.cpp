// forking.cpp
// TE 4348.501 Fall '17 Project 1: Forking
// Author: Stephen-Michael Brooks

/*
This program generates one million forks and measures the differences in time that the parent process regains
control of the processor and the child gaining control. The time at which the child gains control is given to 
the parent via a pipe. The values are compared and and the faster is stored into and array of minimums and the
slower is stored into an array of maximums. With these values, two histograms are created. The histograms will
have 20 bins.

Libraries:
    std::chrono : http://www.cplusplus.com/reference/chrono/ for obtaining the duration of the processes

Output:
    bin_size :  The width of the bins of the histograms
    min      :  The fastest of all processes
    max      :  The slowest of all processes
    minbins  :  The histogram data for the minimums
    maxbins  :  The histogram data for the maximums
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <chrono>


int main(int argc, const char* argv[]) {
    const int one_million = 1000 * 1000;
    
    int pipefd[2];  // file descriptor
    int fstatus;

    int* maximums = new int[one_million];
    int* minimums = new int[one_million];

    pid_t ppid = getpid();
    pid_t cpid;
    
    pipe(pipefd);

    for (int i = 0; i < one_million; i++) {
        using namespace std::chrono;
        
        // create a starting time point for reference
        auto p_start_micro = time_point_cast<nanoseconds>(system_clock::now());
        int fstatus = fork();
        
        switch(fstatus) {
            case -1:    // the child was not created
                perror("Failed to create the child process.");
                exit(1);
            case 0: {   // in the child process
                // record a time point for the child gaining control of the processor
                auto c_end_micro = time_point_cast<nanoseconds>(system_clock::now());
                auto d = c_end_micro - p_start_micro;       // find the duration from call to fork() and now
                
                int c_elapsedtime = d.count();      // counts the number of periods in the duration

                close(pipefd[0]);
                write(pipefd[1], &c_elapsedtime, sizeof(c_elapsedtime));
                close(pipefd[1]);

                _Exit(0);
                break;
            }
            default:    // in the parent process
                // record a time point for the parent gaining control of the processor
                auto p_end_micro = time_point_cast<nanoseconds>(system_clock::now());
                auto d = p_end_micro - p_start_micro;

                int p_elapsedtime = d.count();      // counts the number of periods in the duration
                int c_elapsedtime;
                
                wait(&fstatus);     // wait for child process to terminate
                read(pipefd[0], &c_elapsedtime, sizeof(c_elapsedtime));

                // find which process was faster
                if(c_elapsedtime > p_elapsedtime) {
                    // parent was faster
                    maximums[i] = c_elapsedtime;
                    minimums[i] = p_elapsedtime;
                }
                else {
                    // child was faster
                    minimums[i] = c_elapsedtime;
                    maximums[i] = p_elapsedtime;
                }
                break;
        }
    }

    // find the min and max
    int min = 2000000;
    int max = 0;
    for (int i = 0; i < one_million; i++) {
        if (minimums[i] < min) {
            min = minimums[i];
        }
        if (maximums[i] > max) {
            max = maximums[i];
        }
    }
    
    // determine the size of the histogram bins
    int bin_size = (max - min) / 20;
    printf("bin width: %d\n", bin_size);
    printf("Maxmimum value: %d\nMinimum value: %d\n", max, min);

    /* There are 20 bins, however, the maximum value and any other value very close to it will fall into 
       bin 20 and be out of bounds of the array, so it will be put into index 20 and be added to index 19
       when the rest of the bin assignments are finished  */
    int* minbins = new int[21];
    int* maxbins = new int[21];

    for (int i = 0; i < one_million; i++) {
        int min_assignment = (minimums[i] - min) / bin_size;
        int max_assignment = (maximums[i] - min) / bin_size;
        minbins[min_assignment]++;
        maxbins[max_assignment]++;
    }
    maxbins[19] += maxbins[20];     // to catch the the max value(s)

    // print the mins histogram
    printf("Min bins:  ");
    for (int i = 0; i < 20; i++) {
        printf("%d ", minbins[i]);
    }
    printf("\n");

    // print the maxes histogram
    printf("Max bins:  ");
    for (int i = 0; i < 20; i++) {
        printf("%d ", maxbins[i]);
    }
    printf("\n");

    exit(0);
    // cleanup
    delete [] maximums;
    delete [] minimums;
    delete [] minbins;
    delete [] maxbins;
}