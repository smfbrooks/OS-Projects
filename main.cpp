// main.cpp
// TE 4348.501 Fall '17 Project 3: Disk Scheduling
// Author: Stephen-Michael Brooks

/*
This program's purpose is to demonstrate the differences between four different disk scheduling alorithms:
    First Come First Serve
    Shortest Service Time First
    SCAN
    CSCAN

The program generates a list of size 25 to 75 of random, unique track numbers between 1 and 199, then uses that list of tracks in the algorithms
listed above. Each size of the list is tested NUM_SIMS of times (more than 100). During the tests, the total seek length to complete the
algorithm is recorded and an average for each list size printed in the output.


Libraries:
    std::vector     :   http://www.cplusplus.com/reference/vector/vector/

Output:
    fcfs_avg        :   Average seek length for FCFS
    sstf_avg        :   Average seek length for SSTF
    scan_avg        :   Average seek length for SCAN
    cscan_avg       :   Average seek length for CSCAN


Function(s):
    main():
        Instantiates all vectors and performs calculations on data returned from algorithms.
    contains(std::vector<int>* v, const int n):
        Detects is n is already present in v.
    get_rands(std::vector<int>* v, const int size):
        Generates random numbers and assigns them to v.
    align_pairs(std::vector<int>* v, const int start):
        Finds pairs of numbers with the smallest difference between them and puts them next to each other.
    short_sort_dec(std::vector<int>* v): 
        Sorts v from the beginning of the list in decending order until min in reached.
    short_sort_asc(std::vector<int>* v):
        Sorts v from the beginning of the list in ascending order until max is reached.
    full_sort_asc(std::vector<int>* v):
        Sorts v in ascending order.
    fcfs(std::vector<int>* v):
        Disk scheduling algorithm that serves requests in order from beginning to end.
    sstf(std::vector<int>* v):
        Disk scheduling algorithm that serves requests in order from shortest to longest seek length.
    scan_asc(std::vector<int>* v):
        Half of the scan disk scheduling algorithm.
    scan_dec(std::vector<int>* v):
        Half of the scan disk scheduling algorithm.
    scan(std::vector<int>* vec):
        Disk scheduling algorithm that serves requests in one direction and then the remaining requests in the opposite direction.
    cscan(std::vector<int>* vec):
        Disk scheduling algorithm that serves requests in only one direction.
*/


#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <vector>


const int NUM_SIMS = 150;

/*
Fucntion: contains
    Author(s): Stephen-Michael Brooks
    Detects is n is already present in v.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints that will be traversed to find if n is present in v.
        n (input)   :   "candidate for insert"
            type: int
            desc: An int that is a candidate for insertion to v.
    output:
        bool    :   true if n is already present in v
                    false if n is not present in v
*/  
bool contains(std::vector<int>* v, const int n) {
    auto front = v->begin();
    for (int i = 0; i < v->size(); i++) {
        if (n == v->at(i))
            return true;    // n is already present in the list
    }
    return false;
}

/*
Fucntion: get_rands
    Author(s): Stephen-Michael Brooks
    Inserts unique, randomly generated ints between 1 and 199 into v.
    args:
        v (input)       :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
        size (input)    :   "size of v"
            type: int
            desc: Sentinal to stop adding ints to v.
*/  
void get_rands(std::vector<int>* v, const int size) {
    while(v->size() < size) {
        int n = rand() % 199 + 1;
        if (!contains(v, n))
            v->push_back(n);
    }
}

/*
Fucntion: align_pairs
    Author(s): Stephen-Michael Brooks
    Finds pairs of ints that have the smallest difference and arranges v to have them in indexes next to each other.
    args:
        v (input)       :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
        start (input)   :   "index to begin"
            type: int
            desc: Determines where to begin searching in v.
*/  
void align_pairs(std::vector<int>* v, const int start) {
    int distance = std::numeric_limits<int>::max();
    int pair_index;
    for(int i = start; i < v->size()-1; i++) {
        if (abs(v->at(start) - v->at(i+1)) < distance) {
            pair_index = i + 1;     // a smaller difference from index start has been found at index i+1
            distance = abs(v->at(start) - v->at(i+1));
        }
    }
    if (start < v->size()-1) {  // checking is at the end of the list
        // move closest pair to start to one index higher that start
        v->emplace(v->begin() + start +1, v->at(pair_index));
        v->erase(v->begin() + pair_index + 1);
    }
}

/*
Fucntion: short_sort_dec
    Author(s): Stephen-Michael Brooks
    Sorts v in decending order starting from the front of v until min is found.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
*/
void short_sort_dec(std::vector<int>* v) {
    int pair_index;

    for(int i = 0; i < v->size(); i++) {
        int distance = std::numeric_limits<int>::max();
        pair_index = i;
        for(int j = i+1; j <= v->size()-1; j++) {
            if (v->at(i) - v->at(j) > 0 && v->at(i) - v->at(j) < distance) {
                distance = v->at(i) - v->at(j);     // difference between i and j is smaller than previous
                pair_index = j;                     // and is positive
            }
        }
        // move closest pair with i to one index higher than i
        v->emplace(v->begin()+i+1, v->at(pair_index));
        v->erase(v->begin()+pair_index+1);
    }
}

/*
Fucntion: short_sort_asc
    Author(s): Stephen-Michael Brooks
    Sorts v in ascending order starting from the front of v until max is found.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
*/
void short_sort_asc(std::vector<int>* v) {
    int pair_index;

    for(int i = 0; i < v->size(); i++) {
        int distance = std::numeric_limits<int>::max();
        pair_index = i;
        for(int j = i+1; j <= v->size()-1; j++) {
            if (v->at(i) - v->at(j) < 0 && abs(v->at(i) - v->at(j)) < distance) {
                distance = abs(v->at(i) - v->at(j));    // difference between i and j is smaller than previous
                pair_index = j;                         // and is negative
            }
        }
        // move closest pair with i to one index higher than i
        v->emplace(v->begin()+i+1, v->at(pair_index));
        v->erase(v->begin()+pair_index+1);
    }
}

/*
Fucntion: short_sort_dec
    Author(s): Stephen-Michael Brooks
    Sorts v in decending order.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
*/ 
void full_sort_asc(std::vector<int>* v) {
    std::sort(v->begin()+1, v->end());
}


/*
Fucntion: fcfs
    Author(s): Stephen-Michael Brooks
    Disk scheduling algorithm that serves requests in order from beginning to end.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
*/ 
int fcfs(std::vector<int>* v) {
    int seek_amount = 0;
    for(int i = 1; i < v->size(); i++)
        seek_amount += abs(v->at(i) - v->at(i-1));
    return seek_amount;
}

/*
Fucntion: sstf
    Author(s): Stephen-Michael Brooks
    Disk scheduling algorithm that serves requests in order from shortest to longest seek length.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
    output:
        fcfs(sstf_v):   "Function call to fcfs disk scheduling algorithm"
            type: int
            desc: sstf_v is sorted so that closest pairs are together
*/ 
int sstf(std::vector<int>* v) {
    int seek_amount = 0;
    std::vector<int>* sstf_v = new std::vector<int>(*v);

    for(int i = 0; i < sstf_v->size(); i++) {
        align_pairs(sstf_v, i);
    }

    return fcfs(sstf_v);    // the list is sorted in a way that satisfies sstf standard
}

/*
Fucntion: scan_asc
    Author(s): Stephen-Michael Brooks
    Half of the scan disk scheduling algorithm which serves requests in ascending order.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
    output:
        seek_amount :
            type: int
            desc: Total seek length for the ascending half of the scan.
*/ 
int scan_asc(std::vector<int>* v) {
    int seek_amount = 0;

    for(int i = 0; i < v->size(); i++) {
        if (v->at(0) - v->at(i) < 0) {
            seek_amount += abs(v->at(0) - v->at(i));
            v->erase(v->begin());   // remove the served request so that the next one is at index 0
            i = 0;  // start the loop over again so that the next request is served immediately
        }
    }

    return seek_amount;
}

/*
Fucntion: scan_dec
    Author(s): Stephen-Michael Brooks
    Half of the scan disk scheduling algorithm which serves requests in decending order.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
    output:
        seek_amount :
            type: int
            desc: Total seek length for the decending half of the scan.
*/ 
int scan_dec(std::vector<int>* v) {
    int seek_amount = 0;
    for(int i = 0; i < v->size(); i++) {
        if (v->at(0) - v->at(i) > 0) {
            seek_amount += abs(v->at(0) - v->at(i));
            v->erase(v->begin());   // remove the served request so that the next one is at index 0
            i = 0;  // start the loop over again so that the next request is served immediately
        }
    }

    return seek_amount;
}

/*
Fucntion: scan
    Author(s): Stephen-Michael Brooks
    Disk scheduling algorithm that serves requests in one direction and then the remaining requests in the opposite direction.
    args:
        vec (input) :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
    output:
        seek_amount :
            type: int
            desc: Total seek length for the scan algorithm.
*/ 
int scan(std::vector<int>* vec) {
    std::vector<int>* v = new std::vector<int>(*vec);

    int seek_amount = 0;

    short_sort_asc(v);  // call the first half
    seek_amount += scan_asc(v);
    short_sort_dec(v);  // call the second half
    seek_amount += scan_dec(v);

    return seek_amount;
}

/*
Fucntion: cscan
    Author(s): Stephen-Michael Brooks
    Disk scheduling algorithm that serves requests in only one direction.
    args:
        v (input)   :   "vector"
            type: vector<int>*
            desc: A pointer to a vector of ints.
    output:
        seek_amount :
            type: int
            desc: Total seek length for the cscan algorithm.
*/ 
int cscan(std::vector<int>* vec) {
    std::vector<int>* v = new std::vector<int>(*vec);

    int seek_amount = 0;
    short_sort_asc(v);  // call the first half
    seek_amount += scan_asc(v);
    full_sort_asc(v);   // sort
    if (v->size() > 1) {    // make sure all requests are not already served
        // prep by serving the top request, becuase the max track is left at the top after the first half
        seek_amount += abs(v->at(0) - v->at(1));
        v->erase(v->begin());
        seek_amount += scan_asc(v); // call the second half
    }

    return seek_amount;
}

/*
Fucntion: main
    Author(s): Stephen-Michael Brooks
    Instantiates all vectors needed to perform the various disk scheduling algorithm tests, calculations, and comparisson.
    output:
        STDOUT  :
            desc: A grid that shows the average seek length for each disk scheduing algorithm for every length of requests queue.
*/ 
int main () {
    std::vector<int>* rands_v = new std::vector<int>();
    std::srand((unsigned)time(NULL));
    std::vector<int> fcfs_totals(51);
    std::vector<int> sstf_totals(51);
    std::vector<int> scan_totals(51);
    std::vector<int> cscan_totals(51);

    for(int i = 0; i < NUM_SIMS; i++) {
        for(int j = 25; j < 76; j++) {
            rands_v->clear();
            get_rands(rands_v, j);

            fcfs_totals[j-25] += fcfs(rands_v);
            sstf_totals[j-25] += sstf(rands_v);
            scan_totals[j-25] += scan(rands_v);
            cscan_totals[j-25] += cscan(rands_v);

        }
    }

    float fcfs_avg, sstf_avg, scan_avg, cscan_avg;

    std::cout << "fcfs\tsstf\tscan\tcscan" << std::endl;

    for(int i = 0; i < 51; i++) {
        std::cout << std::endl;
        fcfs_avg = fcfs_totals[i] / float(((i + 25) * NUM_SIMS));
        sstf_avg = sstf_totals[i] / float(((i + 25) * NUM_SIMS));
        scan_avg = scan_totals[i] / float(((i + 25) * NUM_SIMS));
        cscan_avg = cscan_totals[i] / float(((i + 25) * NUM_SIMS));
        std::cout << fcfs_avg << "\t"
                  << sstf_avg << "\t"
                  << scan_avg << "\t"
                  << cscan_avg << std::endl;
    }
}