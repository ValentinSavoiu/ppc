#include <bits/stdc++.h>
#include "ChunkList.cpp"

using namespace std;
using namespace std::chrono;

#define ll long long
const string INSERT = "INSERT";
const string FIND = "FIND";
const string DELETE = "DELETE";

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

void testFile(char* file_name) {
    ChunkList list(3);

    const int chunkSize = 1000;
    const int N = 1000;

    ifstream f(file_name);
    int operationCount;
    f >> operationCount;
    for (int i = 0; i < operationCount; ++i) {
        string operation;
        int value;
        f >> operation >> value;
        if (operation == INSERT) 
            list.add(value);
        else if (operation == FIND) 
            list.contains(value);
        else if (operation == DELETE) {
            list.remove(value);
        }
        else {
            cerr << "ERROR, unknown operation";
            return;
        }
    
    }
}

void benchmark(void (*fun)(), string fnName) {
    auto start = high_resolution_clock::now();

    fun();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() << endl;
}

void benchmark_file(void (*fun)(char*), char* file_name) {
    auto start = high_resolution_clock::now();

    fun(file_name);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << (double) duration.count() / 1e6 << endl;
}

int main(int argc, char *argv[]) {
    //benchmark(testSearch, "Search");
    benchmark_file(testFile, argv[1]);
    return 0;
}