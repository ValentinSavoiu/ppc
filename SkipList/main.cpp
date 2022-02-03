#include <string>
#include "SkipList.cpp"
#include <chrono>
#include <sstream>
#include <fstream>
#define ll long long

using namespace std;
using namespace std::chrono;

int num_threads = 12;
int chunk_size = 1000;
string command;
SkipList skiplist;

vector<int> numbers_insert;
vector<int> numbers_delete;
vector<int> numbers_get;
vector<thread> threads;

void generate_input(int max_number) {
    for(int i = 1; i <= max_number; i++) {
        numbers_insert.push_back(i);
    }
    skiplist = SkipList(numbers_insert.size(), 0.5);
    for(int i = 0; i < numbers_insert.size(); i++) {
        skiplist.add(numbers_insert[i]);
    }
}

void generate_delete() {
    for(int i = 0; i < numbers_insert.size(); i++){
        if(rand() % 3 == 0 ){
            numbers_delete.push_back(numbers_insert[i]);
        }
    }
}

void generate_search() {
    for(int i = 0; i < numbers_insert.size(); i++){
        if( rand() % 5 == 0 ){
            numbers_get.push_back(numbers_insert[i]);
        }
    }
}

void read(istream &f) {
    int N, x;
    f >> command >> N;
    for (int i = 0; i < N; ++i) {
        f >> x;
        if (command == "INSERT") {
            numbers_insert.push_back(x);
        }
        if (command == "FIND") {
            numbers_get.push_back(x);
        }
        if (command == "DELETE") {
            numbers_delete.push_back(x);
        }
    }
}

void skiplist_add(int start, int end) {
    if(end >= numbers_insert.size()) {
        end = numbers_insert.size();
    }
    if(start == end) {
        skiplist.add(numbers_insert[start]);
    }
    for(int i = start; i < end; i++) {
        skiplist.add(numbers_insert[i]);
    }
}

void skiplist_remove(int start, int end) {
    if(end >= numbers_delete.size()) {
        end = numbers_delete.size();
    }
    if(start == end) {
        skiplist.remove(numbers_delete[start]);
    }
    for(int i = start; i < end; i++) {
        skiplist.remove(numbers_delete[i]);
    }
}

void skiplist_search(int start, int end) {
    if(end >= numbers_get.size()) {
        end = numbers_get.size();
    }
    if(start == end) {
        end++;
    }
    for(int i = start; i < end; i++){
        int res = skiplist.search(numbers_get[i]);
        //cout << "Searching for " << numbers_get[i] << " Search value: " << res << endl;
    }
}

void setup() {
    const int N = 1e5;
    generate_input(N);
    generate_search();
    generate_delete();
}

// Testing insert
void insertTest() {
    
    //INSERT TEST
    int chunk_size = ceil(float(numbers_insert.size()) / num_threads);
    for(int i = 0; i < numbers_insert.size(); i = i + chunk_size) {
        threads.push_back(thread(skiplist_add, i, i + chunk_size));
    }
    for (auto &th : threads) {
        th.join();
    }
    threads.clear();
}

// Search test
void searchTest() {
    int chunk_size = ceil(float(numbers_get.size()) / num_threads);
    for(size_t i = 0; i < numbers_get.size(); i = i + chunk_size) {
        threads.push_back(thread(skiplist_search, i, i + chunk_size));
    }
    for (auto &th : threads) {
        th.join();
    }
    threads.clear();
}

// Delete test
void deleteTest() {
    int chunk_size = ceil(float(numbers_delete.size()) / num_threads);
    for(size_t i = 0; i < numbers_delete.size(); i = i + chunk_size){
        threads.push_back(thread(skiplist_remove, i, i + chunk_size));
    }
    for (auto &th : threads) {
        th.join();
    }
    threads.clear();
}


void benchmark(void (*fun)(), string fnName) {
    auto start = high_resolution_clock::now();

    fun();

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Running time for " << fnName << ": " << (double) duration.count() / 1e6 << " seconds" << endl;
}

void benchmark_file(char* filename) {
    ifstream f(filename);
    read(f);
    high_resolution_clock::time_point start;
    if (command == "INSERT" ) {
        skiplist = SkipList(numbers_insert.size(), 0.5);
        start = high_resolution_clock::now();
        insertTest();
    }
    else {
        generate_input((int) 1e5);
        start = high_resolution_clock::now();
        if (command == "FIND") {
            searchTest();
        }
        else {
            deleteTest();
        }
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << (double) duration.count() / 1e6 << "\n";
}

int main(int argc, char **argv) {
    srand(time(NULL));
    const int N = 1e6;
    std::stringstream stream((string) argv[1]);
    while(1) {
        stream >> num_threads;
        if(!stream)
            break;
        benchmark_file(argv[2]);
        numbers_delete.clear();
        numbers_get.clear();
        numbers_insert.clear();
    }
    
    /*
    setup();

    benchmark(insertTest, "INSERT");
    benchmark(searchTest, "SEARCH");
    benchmark(deleteTest, "DELETE");
    */
    return 0;
}