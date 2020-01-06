/*
 * Uğur Ali Kaplan
 * Student ID: 150170042
 */

#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <vector>

/*
 * Input format : EVENT-NAME START-TIME END-TIME
 * How to read input: Read a list of events from a binary file, file name is supplied as commandline argument
 * Create a MIN-HEAP PRIORITY QUEUE
 * Each event creates two entries in the min-heap -> (Event Start, Key = Start_time) and (Event End, Key = End_time)
 * So, heap nodes should include event name and event type(i.e. start or end)
 *
 * Virtual clock in the system starting from T = 0
 * At each tick, check the root of the min heap. If T = Key value of the root, Call Min-remove and print the name
 *  and type of the event.
 *
 * There might be multiple keys with the same value.
 */

using namespace std;

class Node{
private:
    string name;
    bool start; // If true, this is a "START" node, else this is an "END" node
    unsigned int time;
public:
    Node(string name, bool start, unsigned int time);
    unsigned int getTime() {
        return time;
    }
    void printNode();
};

void Node::printNode() {
    /*
     * Prints the name of the event and type of the event (e.g. EVENT-A STARTED, EVENT-B ENDED ...)
     */

    cout << name << " ";
    if (start) cout << "STARTED";
    else cout << "ENDED";
    cout << endl;
}

Node::Node(string name, bool start, unsigned int time) {
    this->name = name;
    this->start = start;
    this->time = time;
}

class Scheduler{
private:
    Node *heap; // Hold an array of nodes, called heap
    int heap_size; // How many elements do we have?
    unsigned int T; // Timer
public:
    Scheduler(Node *heap, int size);
    unsigned int heapMinimum();
    void heapExtractMin();
    void minHeapify(int index);
    void buildMinHeap();
    void printHeap();
    void tickTock();
};

Scheduler::Scheduler(Node *heap, int size) : T(1){
    this->heap = heap;
    this->heap_size = size;
    buildMinHeap();
}

unsigned int Scheduler::heapMinimum() {
    /*
     * Returns the key of the minimum element
     */
    return heap[0].getTime();
}

void Scheduler::heapExtractMin() {
    /*
     * Prints the minimum element and discards it out of the heap.
     */
    Node value = heap[0];
    Node temp = heap[heap_size - 1];
    heap[heap_size - 1] = heap[0];
    heap[0] = temp;
    heap_size -= 1;
    minHeapify(0);
    value.printNode();
}

void Scheduler::minHeapify(int index) {
    /*
     * Changes the heap so that index it is called on satisfies the min-heap property.
     */
    int left = (2 * (index + 1)) - 1;
    int right = left + 1;
    int min = index;
    if (left <= (heap_size - 1))
    {if (heap[left].getTime() < heap[min].getTime()) min = left;}
    if (right <= (heap_size - 1))
    {if (heap[right].getTime() < heap[min].getTime()) min = right;}
    if(min != index){
        Node temp = heap[index];
        heap[index] = heap[min];
        heap[min] = temp;
        minHeapify(min);
    }
}

void Scheduler::buildMinHeap() {
    /*
     * Builds a min-heap out of the unordered array it is given.
     */
    int start_index = floor(heap_size - 1 / 2.0);
    for(int i = start_index; i >= 0; i--){
        minHeapify(i);
    }
}

void Scheduler::printHeap() {
    /*
     * Prints all elements of the heap, can be used for debugging purposes.
     */
    for(int i = 0; i < heap_size; i++){
        heap[i].printNode();
    }
}

void Scheduler::tickTock() {
    /*
     * Runs the timer, prints the events that are happening.
     * Stops the timer if there are no more events.
     */
    while (heap_size > 0){
        if (T != heapMinimum()) cout << "TIME " << T << ": NO EVENT" << endl;
        while (T == heapMinimum() && heap_size > 0){
            cout << "TIME " << T << ": ";
            heapExtractMin();
        }
        T++;
    }
    T--;
    cout << "TIME " << T << ": NO MORE EVENTS, SCHEDULER EXITS" << endl;
}

int main(int argc, char * argv[]) {
    ifstream in_file(argv[1]); // Open the input file
    int size = 0; // Heap Size

    if(!in_file) { // If were unable to open the file
        cerr << "File could not be read" << endl;
        exit(1);
    }

    vector<Node> nodeList; // Number of events is unknown, therefore we use vectors

    string name; // name will hold the name of the event

    while(in_file >> name){ // Read lines from the file
        int start, end; // Start and end times
        in_file >> start;
        in_file >> end;
        if(start < 1 || end < 1 || start > end){
            cerr << name << " cannot be used, times are incorrect" << endl;
            continue;
        }
        nodeList.push_back(Node(name, true, start)); // Append a start node
        nodeList.push_back(Node(name, false, end)); // Append an end node
        size += 2; // Keep track of how many nodes we have added
    }

    in_file.close(); // Clean after yourself

    if (size == 0) {cerr << "File is empty!" << endl; exit(1);} // If there are no nodes, exit

    Scheduler schedule(&nodeList[0], size); // Initialize the scheduler object

    schedule.tickTock(); // Run the timer

    return 0;
}