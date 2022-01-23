#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <vector>
#include <chrono>
#include <algorithm>
using namespace std;
using namespace std::chrono;

const string INSERT = "INSERT";
const string FIND = "FIND";
const string DELETE = "DELETE";
vector<int> v;

void fun(string filename) {
    ifstream f(filename);
    int operationCount;
    f >> operationCount;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < operationCount; ++i) {
        string operation;
        int value;
        f >> operation >> value;
        if (operation == INSERT) 
            v.push_back(value);
        else if (operation == FIND) 
            auto it = find(v.begin(), v.end(), 10);
        else if (operation == DELETE) {
            auto it = find(v.begin(), v.end(), 10);
            if (it != v.end())
                v.erase(it);
        }
        else {
            cerr << "ERROR, unknown operation";
            return;
        }
    
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << (double) duration.count() << endl;
}


int main(int argc, char **argv) {
    assert(argc == 2);
    fun(argv[1]);
    return 0;
}