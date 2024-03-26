#include <iostream>
#include <stdexcept>

class PriorityQueue {
private:
    int *heapArray;
    int capacity;
    int heapSize;

    int parent(int index) {
        return (index - 1) / 2;
    }

    int leftChild(int index) {
        return (2 * index) + 1;
    }

    int rightChild(int index) {
        return (2 * index) + 2;
    }

    void heapifyUp(int index) {
        while (index > 0 && heapArray[parent(index)] < heapArray[index]) {
            std::swap(heapArray[parent(index)], heapArray[index]);
            index = parent(index);
        }
    }

    void heapifyDown(int index) {
        int maxIndex = index;
        int l = leftChild(index);
        int r = rightChild(index);

        if (l < heapSize && heapArray[l] > heapArray[maxIndex])
            maxIndex = l;

        if (r < heapSize && heapArray[r] > heapArray[maxIndex])
            maxIndex = r;

        if (index != maxIndex) {
            std::swap(heapArray[index], heapArray[maxIndex]);
            heapifyDown(maxIndex);
        }
    }

public:
    explicit PriorityQueue(int size) : capacity(size), heapSize(0) {
        heapArray = new int[size];
    }

    ~PriorityQueue() {
        delete[] heapArray;
    }

    void enqueue(int key) {
        if (heapSize == capacity) {
            throw std::overflow_error("Priority queue overflow");
        }

        heapArray[heapSize] = key;
        heapifyUp(heapSize);
        heapSize++;
    }

    int dequeue() {
        if (isEmpty()) {
            throw std::underflow_error("Priority queue underflow");
        }

        int max = heapArray[0];
        heapArray[0] = heapArray[heapSize - 1];
        heapSize--;
        heapifyDown(0);
        return max;
    }

    bool isEmpty() const {
        return heapSize == 0;
    }
};

int main() {
    PriorityQueue pq(10);

    pq.enqueue(3);
    pq.enqueue(2);
    pq.enqueue(5);
    pq.enqueue(1);
    pq.enqueue(6);

    while (!pq.isEmpty()) {
        std::cout << pq.dequeue() << " ";
    }
    std::cout << std::endl;

    return 0;
}
