#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include <stdexcept>
#include <algorithm> // for std::swap

struct Node {
    std::string album;
    std::string artist;
    int downVotes;
    std::string filePath;
    std::string genre;
    std::string id;
    std::string name;
    int upVotes;
    int duration;
};

class PriorityQueue {
private:
    Node* heapArray;
    int capacity;
    int heapSize;

    int parent(int index);
    int leftChild(int index);
    int rightChild(int index);
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    explicit PriorityQueue(int size);
    ~PriorityQueue();
    void enqueue(const Node& node);
    Node dequeue();
    bool isEmpty() const;
    void sortByVotes();
    void sortByDifference();
    void printQueue() const;
    Node& getNodeAtIndex(int index);
    void upVote(const std::string& id);
    void downVote(const std::string& id);
};

#endif // PRIORITY_QUEUE_H
