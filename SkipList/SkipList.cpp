#include <map>
#include <math.h>
#include <limits.h>
#include "node.cpp"
#include <iostream>

using namespace std;

static int max_level;

class SkipList{
    private:
        // Head and Tail of the Skiplist
        Node *head;
        Node *tail;
    public:
        SkipList() {}
        SkipList(int max_elements, float prob) {
            max_level = (int) round(log(max_elements) / log(1/prob)) - 1;
            head = new Node(INT_MIN, max_level);
            tail = new Node(INT_MAX, max_level);

            for (size_t i = 0; i < head->next.size(); i++) {
                head->next[i] = tail;
            }
        }
        ~SkipList() {}
        int get_random_level() {
            int l = 0;
            while(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) <= 0.5){
                l++;
            }
            return l > max_level ? max_level : l;
        }

        // Supported operations
        int find(int key, vector<Node*> &predecessors, vector<Node*> &successors) {
            int found = -1;
            Node *prev = head; 
            for (int level = max_level; level >= 0; level--){
                Node *curr = prev->next[level];
                while (key > curr->get_value()){
                    prev = curr;
                    curr = prev->next[level];
                }
                if(found == -1 && key == curr->get_value()){
                    found = level;
                }

                predecessors[level] = prev;
                successors[level] = curr;
            }
            return found;
        }


        bool add(int value) {
            // Get the level until which the new node must be available
            int top_level = get_random_level();
            
            // Initialization of references of the predecessors and successors
            vector<Node*> preds(max_level + 1); 
            vector<Node*> succs(max_level + 1);

            for (size_t i = 0; i < preds.size(); i++){
                preds[i] = NULL;
                succs[i] = NULL;
            }

            // Keep trying to insert the element into the list. In case predecessors and successors are changed,
            // this loop helps to try the insert again
            while(true){
                // Find the predecessors and successors of where the key must be inserted
                int found = find(value, preds, succs);

                // If found and marked, wait and continue insert
                // If found and unmarked, wait until it is fully_linked and return. No insert needed
                // If not found, go ahead with insert
                if(found != -1) {
                    Node* node_found = succs[found];
                    
                    if(!node_found->marked){
                        while(! node_found->fully_linked){}

                        return false;
                    }
                    continue;
                }

                // Store all the Nodes which lock we acquire in a map
                // Map used so that we don't try to acquire lock to a Node we have already acquired
                // This may happen when we have the same predecessor at different levels
                map<Node*, int> locked_nodes;

                // Traverse the skip list and try to acquire the lock of predecessor at every level
                try{
                    Node* pred;
                    Node* succ;

                    // Used to check if the predecessor and successors are same from when we tried to read them before
                    bool valid = true;

                    for (int level = 0; valid && (level <= top_level); level++){
                        pred = preds[level];
                        succ = succs[level];

                        // If not already acquired lock, then acquire the lock 
                        if(!(locked_nodes.count( pred ))){
                            pred->lock();
                            locked_nodes.insert(make_pair(pred, 1));
                        }

                        // If predecessor marked or if the predecessor and successors change, then abort and try again
                        valid = !(pred->marked.load(std::memory_order_seq_cst)) && !(succ->marked.load(std::memory_order_seq_cst)) && pred->next[level]==succ;                
                    }

                    // Conditons are not met, release locks, abort and try again.
                    if(!valid){
                        for (auto const& x : locked_nodes){
                            x.first->unlock();
                        }
                        continue;
                    }

                    // All conditions satisfied, create the Node and insert it as we have all the required locks
                    Node* new_node = new Node(value, top_level);

                    // Update the predecessor and successors
                    for (int level = 0; level <= top_level; level++){
                        new_node->next[level] = succs[level];
                    }

                    for (int level = 0; level <= top_level; level++){
                        preds[level]->next[level] = new_node;
                    }

                    // Mark the node as completely linked.
                    new_node->fully_linked = true;
                    
                    // Release lock of all the nodes held once insert is complete
                    for (auto const& x : locked_nodes){
                        x.first->unlock();
                    }
                    
                    return true;
                }catch(const std::exception& e){
                    // If any exception occurs during the above insert, release locks of the held nodes and try again.
                    cerr << e.what() << '\n';
                    for (auto const& x : locked_nodes){
                            x.first->unlock();
                    }
                }
            }
        }
        int search(int key) {
            // Finds the predecessor and successors 
            vector<Node*> preds(max_level + 1); 
            vector<Node*> succs(max_level + 1);

            for (size_t i = 0; i < preds.size(); i++){
                preds[i] = NULL;
                succs[i] = NULL;
            }

            int found = find(key, preds, succs);

            // If not found return empty.
            if(found == -1){
                return -1;
            }

            Node *curr = head; 

            for (int level = max_level; level >= 0; level--){
                while (curr->next[level] != NULL && key > curr->next[level]->get_value()){
                    curr = curr->next[level];
                }
            }

            curr = curr->next[0];
            
            // If found, unmarked and fully linked, then return value. Else return empty.
            if ((curr != NULL) && (curr->get_value() == key) && succs[found]->fully_linked && !succs[found]->marked){
                return curr->get_value();
            }else {
                return -1;
            }
        }

        bool remove(int key) {
            // Initialization
            Node* victim = NULL;
            bool is_marked = false;
            int top_level = -1;

            // Initialization of references of the predecessors and successors
            vector<Node*> preds(max_level + 1); 
            vector<Node*> succs(max_level + 1);

            for (size_t i = 0; i < preds.size(); i++){
                preds[i] = NULL;
                succs[i] = NULL;
            }

            // Keep trying to delete the element from the list. In case predecessors and successors are changed,
            // this loop helps to try the delete again
            while(true){
                
                // Find the predecessors and successors of where the key to be deleted
                int found = find(key, preds, succs);

                // If found, select the node to delete. else return
                if(found != -1){
                    victim = succs[found];
                }

                // If node not found and the node to be deleted is fully linked and not marked return
                if(is_marked | 
                        (found != -1 &&
                        (victim->fully_linked && victim->top_level == found && !(victim->marked))
                        )
                    ){
                        // If not marked, the we lock the node and mark the node to delete
                        if(!is_marked){
                            top_level = victim->top_level;
                            victim->lock();
                            if(victim->marked){
                                victim->unlock();
                                return false;
                            }
                            victim->marked = true;
                            is_marked = true;
                        }

                        // Store all the Nodes which lock we acquire in a map
                        // Map used so that we don't try to acquire lock to a Node we have already acquired
                        // This may happen when we have the same predecessor at different levels
                        map<Node*, int> locked_nodes;

                        // Traverse the skip list and try to acquire the lock of predecessor at every level
                        try{
                            Node* pred;
                            //Node* succ;

                            // Used to check if the predecessors are not marked for delete and if the predecessor next is the node we are trying to delete or if it is changed.
                            bool valid = true;

                            for(int level = 0; valid && (level <= top_level); level++){
                                pred = preds[level];
                                
                                // If not already acquired lock, then acquire the lock 
                                if(!(locked_nodes.count( pred ))){
                                    pred->lock();
                                    locked_nodes.insert(make_pair(pred, 1));
                                }
                                
                                // If predecessor marked or if the predecessor's next has changed, then abort and try again
                                valid = !(pred->marked) && pred->next[level] == victim;
                            }

                            // Conditons are not met, release locks, abort and try again.
                            if(!valid){
                                for (auto const& x : locked_nodes){
                                    x.first->unlock();
                                }
                                continue;
                            }

                            // All conditions satisfied, delete the Node and link them to the successors appropriately
                            for(int level = top_level; level >= 0; level--){
                                preds[level]->next[level] = victim->next[level];
                            }

                            victim->unlock();

                            // delete victim;

                            // Delete is completed, release the locks held.
                            for (auto const& x : locked_nodes){
                                x.first->unlock();
                            }

                            return true;
                        }catch(const std::exception& e){
                            // If any exception occurs during the above delete, release locks of the held nodes and try again.
                            for (auto const& x : locked_nodes){
                                x.first->unlock();
                            }
                        }

                    }else{
                        return false;
                    }
            }
        }
        void display() {
            for (int i = 0; i <= max_level; i++) {
                Node *temp = head;
                int count = 0;
                if(!(temp->get_value() == INT_MIN && temp->next[i]->get_value() == INT_MAX)){
                    printf("Level %d  ", i);
                    while (temp != NULL){
                        printf("%d -> ", temp->get_value());
                        temp = temp->next[i];
                        count++;
                    }
                    cout<<endl;
                }
                if(count == 3) break;
            }
            cout << "---------- Display done! ----------\n\n";
        }
};