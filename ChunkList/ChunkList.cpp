#include <bits/stdc++.h>

using namespace std;

struct ChunkList {
    int DEFAULT_SIZE = 1000;
    vector<vector<int> > list;
    int chunkSize;
    mutex mtx;

    int convertIndexToChunk(int index) {
        return index / chunkSize;
    }

    int convertIndexToChunkPos(int index) {
        return index % chunkSize;
    }

    ChunkList() {
        ChunkList(DEFAULT_SIZE);
    }

    ChunkList(int chunkSize) : chunkSize(chunkSize) {
        list = vector<vector<int> >();
    }

    ChunkList(ChunkList &other) {
        list = other.list;
        chunkSize = other.chunkSize;
    }

    void add(int el) {
        for (int i = 0; i < (int) list.size(); i++) {
            if ((int) list[i].size() < chunkSize) {
                list[i].push_back(el);
                return;
            }
        }

        list.push_back(vector<int>());
        list[(int) list.size() - 1].push_back(el);
    }

    void add(int el, bool optimizeSqrtSize) {
        add(el);
        setChunkSize(optimizeSqrtSize);
    }

    void remove(int el) {
        for (int i = 0; i < (int) list.size(); i++) {
            auto idx = upper_bound(list[i].begin(), list[i].end(), el - 1);
            if (idx == list[i].end()) {
                continue;
            }

            //cout << "asdasd " << *idx << endl;
            
            if (*idx != el) {
                continue;
            }

            mtx.lock();
            list[i].erase(idx);
            mtx.unlock();
        }
    }

    void removeAll(int el) {
        for (int i = 0; i < (int) list.size(); i++) {
            for (int j = 0; j < (int) list[i].size(); i++) {
                if (list[i][j] == el) {
                    removeAt(i);
                    i--;
                }
            }
        }
    }

    void removeAll(int el, bool optimizeSqrtSize) {
        removeAll(el);
        setChunkSize(optimizeSqrtSize);
    }

    void removeAt(int index) {
        if (index > size()) {
            return;
        }

        list[convertIndexToChunk(index)].erase(list[convertIndexToChunk(index)].begin() + convertIndexToChunkPos(index));
    }

    void set(int index, int el) {
        if (index > size()) {
            return;
        }
        
        list[convertIndexToChunk(index)][convertIndexToChunkPos(index)] = el;
    }

    int get(int index) {
        if (index > size()) {
            return -1; 
        }

        return list[convertIndexToChunk(index)][convertIndexToChunkPos(index)];
    }

    bool contains(int el) {
        bool found = false;

        std::atomic_int progress_state(0);

        #pragma omp parallel for
        for (int i = 0; i < (int) list.size(); i++) {
            if (progress_state) {
                continue;
            }
            for (int j = 0; j < (int) list[i].size(); j++) {
                if (list[i][j] == el) {
                    progress_state = 1;
                    found = true;
                    break;
                }
            }
        }

        return found;
    }

    vector<int> getList() {
        vector<int> res;
        for(int i = 0; i < (int)list.size(); i++) {
            for(int j = 0; j < (int)list[i].size(); i++) {
                res.push_back(list[i][j]);
            }
        }
        return res;
    }

    int size() {
        int cnt = 0;
        for(int i = 0; i < (int)list.size(); i++) {
            cnt += list[i].size();
        }
        return cnt;
    }
    
    bool isEmpty() {
        return size() == 0;
    }
    
    void clear() {
        list.clear();
    }

    void setChunkSize(int newChunkSize) {
        if(newChunkSize > chunkSize) {
            chunkSize = newChunkSize;
        } else {
            vector<int> res = getList();
            chunkSize = newChunkSize;
            clear();
            for(int i = 0; i < (int)res.size(); i++) {
                add(res[i]);
            }
        }
    }

    void setChunkSize(bool optimizerSqrtSize = false) {
        if(optimizerSqrtSize) {
            setChunkSize((int) sqrt(size()));
        } else {
            setChunkSize((int) (size() * 0.05));
        }
    }

    int getChunkSize() {
        return chunkSize;
    }
    
    int chunkAmount() {
        return (int)list.size();
    }

    void sortList() {
        auto items = getList();
        sort(items.begin(), items.end());
        clear();
        for (auto it : items) {
            add(it);
        }
    }

    void print() {
        for (auto l : list) {
            for (auto it : l) {
                cout << it << ' ';
            }
            cout << endl;
        }
    }
};







































