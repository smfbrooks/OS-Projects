#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <vector>


const int NUM_SIMS = 100;


bool contains(std::vector<int>* v, const int n) {
    auto front = v->begin();
    for (int i = 0; i < v->size(); i++) {
        if (n == v->at(i))
            return true;
    }
    return false;
}


void get_rands(std::vector<int>* v) {
    int num_ops = rand() % 51 + 25;
    while(v->size() < num_ops) {
        int n = rand() % 200 + 1;
        if (!contains(v, n))
            v->push_back(n);
    }
}


void align_pair(std::vector<int>* v, const int start) {
    int distance = std::numeric_limits<int>::max();
    int pair_index;
    for(int i = start; i < v->size()-1; i++) {
        if (abs(v->at(start) - v->at(i+1)) < distance) {
            pair_index = i + 1;
            distance = abs(v->at(start) - v->at(i+1));
        }
    }
    if (start < v->size()-1) {
        v->emplace(v->begin() + start +1, v->at(pair_index));
        v->erase(v->begin() + pair_index + 1);
    }
}


int fcfs(std::vector<int>* v) {
    int seek_amount = 0;
    for(int i = 1; i < v->size(); i++)
        seek_amount += abs(v->at(i) - v->at(i-1));
    return seek_amount;
}


int sstf(std::vector<int>* v) {
    int seek_amount = 0;
    std::vector<int>* sstf_v = new std::vector<int>();

    for(int i = 0; i < v->size(); i++) {
        sstf_v->push_back(v->at(i));
    }

    for(int i = 0; i < sstf_v->size(); i++) {
        align_pair(sstf_v, i);
    }

    return fcfs(sstf_v);
}


int scan(std::vector<int>* v) {
    std::vector<int>* scan_v = new std::vector<int>();
    int seek_amount = 0;
    int pair_index;

    for(int i = 0; i < v->size(); i++)
        scan_v->push_back(v->at(i));

    while(scan_v->size() > 1) {
        for(int i = 1; !scan_v->empty() && i < scan_v->size(); i++) {
            if (scan_v->at(0) - scan_v->at(i) < 0) {
                seek_amount += abs(scan_v->at(0) - scan_v->at(i));
                scan_v->erase(scan_v->begin());
                scan_v->emplace(scan_v->begin(), scan_v->at(i-1));
                scan_v->erase(scan_v->begin()+i);
                i = 0;
            }
        }

        for(int i = 1; !scan_v->empty() && i < scan_v->size(); i++) {
            if (scan_v->at(0) - scan_v->at(i) > 0) {
                seek_amount += abs(scan_v->at(0) - scan_v->at(i));
                scan_v->erase(scan_v->begin());
                scan_v->emplace(scan_v->begin(), scan_v->at(i-1));
                scan_v->erase(scan_v->begin()+i);
                i = 0;
            }
        }

    }
    return seek_amount;
}


int cscan(std::vector<int>* v) {
    std::vector<int>* cscan_v = new std::vector<int>();
    int seek_amount = 0;

    for(int i = 0; i < v->size(); i++)
        cscan_v->push_back(v->at(i));

    int k = 0;
    while(cscan_v->size() > 1) {
        for(int i = 1; !cscan_v->empty() && i < cscan_v->size(); i++) {
            if (cscan_v->at(0) - cscan_v->at(i) > 0) {
                seek_amount += abs(cscan_v->at(0) - cscan_v->at(i));
                cscan_v->erase(cscan_v->begin());
                cscan_v->emplace(cscan_v->begin(), cscan_v->at(i-1));
                cscan_v->erase(cscan_v->begin()+i);
                i = 0;
            }
        }
        if (cscan_v->size() > 1) {
            seek_amount += abs(cscan_v->at(0) - cscan_v->at(1));
            cscan_v->erase(cscan_v->begin());
        }
    }

    return seek_amount;
}


int main () {
    std::vector<int>* rands_v = new std::vector<int>();
    std::srand((unsigned)time(NULL));

    for(int i = 0; i < 10; i++) {
        rands_v->clear();
        get_rands(rands_v);

        // for(int j = 0; j < rands_v->size(); j++)
        //     std::cout << rands_v->at(j) << "\t";
        // std::cout << std::endl;

        std::cout << "Seeks for fcfs: " << fcfs(rands_v) << std::endl;
        std::cout << "Seeks for sstf: " << sstf(rands_v) << std::endl;
        std::cout << "Seeks for scan: " << scan(rands_v) << std::endl;
        std::cout << "Seeks for cscan: " << cscan(rands_v) << std::endl;
    }
}