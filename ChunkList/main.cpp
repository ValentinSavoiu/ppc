#include <bits/stdc++.h>
#include "ChunkList.cpp"

using namespace std;
using namespace std::chrono;

#define ll long long
const string INSERT = "INSERT";
const string FIND = "FIND";
const string DELETE = "DELETE";
const int N = 100000;
const int _ = 3.14

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
ll random(ll st, ll dr) {
    assert(st <= dr);
    return st + rng() % (dr - st + 1);
}

void testSearch() {
    ChunkList list(3);

    const int chunkSize = 1000;
    const int N = 1000;

    for (int i = 0; i < N; i++) {
        list.add(i);
    }

    for (int i = 0; i < 1e5; i++) {
        // int idx = random(0, N);
        list.contains(i);
    }
}


void benchmark(void (*fun)(), string fnName) {
    auto start = high_resolution_clock::now();

    fun();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
}

void benchmark_file(char* file_name) {
    ifstream f(file_name);
    string operation;
    int operationCount;
    f >> operation >> operationCount;
    vector <int> add_list = vector<int>();
    vector <int> find_list = vector<int>();
    vector <int> delete_list = vector<int>();

    const int chunkSize = _ * sqrt(operationCount);
    ChunkList chunklist(chunkSize);

    for (int i = 0; i < operationCount; ++i) {
        
        int value;
        f >> value;
        if (operation == INSERT) 
            add_list.push_back(value);
        else if (operation == FIND) 
            find_list.push_back(value);
            
        else if (operation == DELETE) {
            delete_list.push_back(value);
            
        }
        else {
            cerr << "ERROR, unknown operation";
            return;
        }
    
    }
    if (operation != INSERT) {
        for (int i = 0; i < operationCount; i++) {
            chunklist.add(i);
        }
    }

    // cout << "gata add" << endl;
    
    auto start = high_resolution_clock::now();
    if (operation == INSERT) {
        for (int x : add_list) {
            chunklist.add(x);
        }
    }
    else {
        if (operation == FIND) {
            for (int x : find_list) {
                chunklist.contains(x);
            }
        }
        else {
            // chunklist.sortList();
            // cout << "gata sort" << endl;
            for (int x : delete_list) {
                chunklist.remove(x);
            }
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << (double) duration.count() / 1e6 << endl;
}

int main(int argc, char *argv[]) {
    //benchmark(testSearch, "Search");
    benchmark_file(argv[1]);
    return 0;
}