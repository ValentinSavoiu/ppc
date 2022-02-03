#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

class Node{
    public:
        // Stores the key and value for the Node
        int value;

        // Stores the reference of the next node until the top level for the node
        vector<Node*> next;

        // Lock to lock the node when modifing it
        mutex node_lock;

        // Atomic variable to be marked if this Node is being deleted
        atomic<bool> marked = {false};

        // Atomic variable to indicate the Node is completely linked to predecessors and successors
        atomic<bool> fully_linked = {false};

        // The Maximum level until which the node is available
        int top_level; 

        Node() {}

        Node(int key, int level) {
            next.resize(level + 1);
            for (size_t i = 0; i < next.size(); i++){
                next[i] = NULL;
            }
            top_level = level;
            value = key;
        }
        ~Node() {}
        int get_value() {
            return value;
        }
        void lock() {
            node_lock.lock();
        }
        void unlock() {
            node_lock.unlock();
        }
};
