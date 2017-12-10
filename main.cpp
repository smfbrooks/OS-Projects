#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <vector>


const int NUM_SIMS = 150;


bool contains(std::vector<int>* v, const int n) {
    auto front = v->begin();
    for (int i = 0; i < v->size(); i++) {
        if (n == v->at(i))
            return true;
    }
    return false;
}


void get_rands(std::vector<int>* v, const int size) {
    while(v->size() < size) {
        int n = rand() % 199 + 1;
        if (!contains(v, n))
            v->push_back(n);
    }
}


void align_pairs(std::vector<int>* v, const int start) {
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


void short_sort_dec(std::vector<int>* v) {
    int pair_index;

    for(int i = 0; i < v->size(); i++) {
        int distance = std::numeric_limits<int>::max();
        pair_index = i;
        for(int j = i+1; j <= v->size()-1; j++) {
            if (v->at(i) - v->at(j) > 0 && v->at(i) - v->at(j) < distance) {
                distance = v->at(i) - v->at(j);
                pair_index = j;
            }
        }
        v->emplace(v->begin()+i+1, v->at(pair_index));
        v->erase(v->begin()+pair_index+1);
    }
}


void short_sort_asc(std::vector<int>* v) {
    int pair_index;

    for(int i = 0; i < v->size(); i++) {
        int distance = std::numeric_limits<int>::max();
        pair_index = i;
        for(int j = i+1; j <= v->size()-1; j++) {
            if (v->at(i) - v->at(j) < 0 && abs(v->at(i) - v->at(j)) < distance) {
                distance = abs(v->at(i) - v->at(j));
                pair_index = j;
            }
        }
        v->emplace(v->begin()+i+1, v->at(pair_index));
        v->erase(v->begin()+pair_index+1);
    }
}


void full_sort_asc(std::vector<int>* v) {
    std::sort(v->begin()+1, v->end());
}



int fcfs(std::vector<int>* v) {
    int seek_amount = 0;
    for(int i = 1; i < v->size(); i++)
        seek_amount += abs(v->at(i) - v->at(i-1));
    return seek_amount;
}


int sstf(std::vector<int>* v) {
    int seek_amount = 0;
    std::vector<int>* sstf_v = new std::vector<int>(*v);

    for(int i = 0; i < sstf_v->size(); i++) {
        align_pairs(sstf_v, i);
    }

    return fcfs(sstf_v);
}


int scan_asc(std::vector<int>* v) {
    int seek_amount = 0;

    for(int i = 0; i < v->size(); i++) {
        if (v->at(0) - v->at(i) < 0) {
            seek_amount += abs(v->at(0) - v->at(i));
            v->erase(v->begin());
            i = 0;
        }
    }

    return seek_amount;
}


int scan_dec(std::vector<int>* v) {
    int seek_amount = 0;
    for(int i = 0; i < v->size(); i++) {
        if (v->at(0) - v->at(i) > 0) {
            seek_amount += abs(v->at(0) - v->at(i));
            v->erase(v->begin());
            i = 0;
        }
    }

    return seek_amount;
}


int scan(std::vector<int>* vec) {
    std::vector<int>* v = new std::vector<int>(*vec);

    int seek_amount = 0;

    short_sort_asc(v);
    seek_amount += scan_asc(v);
    short_sort_dec(v);
    seek_amount += scan_dec(v);

    return seek_amount;
}


int cscan(std::vector<int>* vec) {
    std::vector<int>* v = new std::vector<int>(*vec);


    int seek_amount = 0;
    short_sort_asc(v);
    seek_amount += scan_asc(v);
    full_sort_asc(v);
    if (v->size() > 1) {
        seek_amount += abs(v->at(0) - v->at(1));
        v->erase(v->begin());
        seek_amount += scan_asc(v);
    }

    return seek_amount;
}


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
            
            // for(int k = 0; k < rands_v->size(); k++)
            //     std::cout << rands_v->at(k) << "\t";

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