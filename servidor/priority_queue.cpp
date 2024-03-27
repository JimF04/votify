#include "priority_queue.h"
#include <json/json.h>

int PriorityQueue::parent(int index) {
    return (index - 1) / 2;
}

int PriorityQueue::leftChild(int index) {
    return (2 * index) + 1;
}

int PriorityQueue::rightChild(int index) {
    return (2 * index) + 2;
}

void PriorityQueue::heapifyUp(int index) {
    while (index > 0 && heapArray[parent(index)].upVotes < heapArray[index].upVotes) {
        std::swap(heapArray[parent(index)], heapArray[index]);
        index = parent(index);
    }
}

void PriorityQueue::heapifyDown(int index) {
    int maxIndex = index;
    int l = leftChild(index);
    int r = rightChild(index);

    if (l < heapSize && heapArray[l].upVotes > heapArray[maxIndex].upVotes)
        maxIndex = l;

    if (r < heapSize && heapArray[r].upVotes > heapArray[maxIndex].upVotes)
        maxIndex = r;

    if (index != maxIndex) {
        std::swap(heapArray[index], heapArray[maxIndex]);
        heapifyDown(maxIndex);
    }
}


void PriorityQueue::printQueue() const {
    std::cout << "Priority Queue Contents:" << std::endl;
    for (int i = 0; i < heapSize; ++i) {
        std::cout << "Index " << i << ": " << std::endl;
        std::cout << "  Album: " << heapArray[i].album << std::endl;
        std::cout << "  Artist: " << heapArray[i].artist << std::endl;
        std::cout << "  Down Votes: " << heapArray[i].downVotes << std::endl;
        std::cout << "  File Path: " << heapArray[i].filePath << std::endl;
        std::cout << "  Genre: " << heapArray[i].genre << std::endl;
        std::cout << "  ID: " << heapArray[i].id << std::endl;
        std::cout << "  Name: " << heapArray[i].name << std::endl;
        std::cout << "  Up Votes: " << heapArray[i].upVotes << std::endl;
        std::cout << "  Duration: " << heapArray[i].duration << std::endl;
        std::cout << std::endl;
    }
}



PriorityQueue::PriorityQueue(int size) : capacity(size), heapSize(0) {
    heapArray = new Node[size];
}

PriorityQueue::~PriorityQueue() {
    delete[] heapArray;
}

void PriorityQueue::enqueue(const Node& node) {
    if (heapSize == capacity) {
        throw std::overflow_error("Priority queue overflow");
    }

    heapArray[heapSize] = node;
    heapifyUp(heapSize);
    sortByDifference(); // Ordenar según la diferencia de votos
    heapSize++;
}


Node PriorityQueue::dequeue() {
    if (isEmpty()) {
        throw std::underflow_error("Priority queue underflow");
    }

    Node max = heapArray[0];
    heapArray[0] = heapArray[heapSize - 1];
    heapSize--;
    heapifyDown(0);
    return max;
}

bool PriorityQueue::isEmpty() const {
    return heapSize == 0;
}

void PriorityQueue::sortByVotes() {
    std::sort(heapArray, heapArray + heapSize, [](const Node& a, const Node& b) {
        return a.upVotes > b.upVotes;
    });
}

void PriorityQueue::sortByDifference() {
    std::sort(heapArray, heapArray + heapSize, [](const Node& a, const Node& b) {
        int diffA = a.upVotes - a.downVotes;
        int diffB = b.upVotes - b.downVotes;
        return diffA > diffB;
    });
}

Node& PriorityQueue::getNodeAtIndex(int index) {
    if (index < 0 || index >= heapSize) {
        throw std::out_of_range("Index out of range");
    }
    return heapArray[index];
}

void PriorityQueue::upVote(const std::string& id) {
    for (int i = 0; i < heapSize; ++i) {

        std::cout << "heapArray[i].name:" << heapArray[i].id << "x:" << id << std::endl;

        if (heapArray[i].id == id) {
            heapArray[i].upVotes++;
            return;
        }
    }
    throw std::invalid_argument("Node with given ID not found");
}

void PriorityQueue::downVote(const std::string& id) {
    for (int i = 0; i < heapSize; ++i) {
        if (heapArray[i].id == id) {
            heapArray[i].downVotes++;
            return;
        }
    }
    throw std::invalid_argument("Node with given ID not found");
}

// Función para eliminar espacios adicionales al principio y al final de una cadena
std::string cleanString(const std::string& str) {
    std::string result;

    // Eliminar espacios al principio
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start != std::string::npos) {
        // Eliminar espacios al final
        size_t end = str.find_last_not_of(" \t\r\n");
        result = str.substr(start, end - start + 1);
    }

    return result;
}

void PriorityQueue::json_pq() const {
    Json::Value priorityQueueJson(Json::arrayValue);

    for (int i = 0; i < heapSize; ++i) {
        std::string cleanedName = cleanString(heapArray[i].name);

        Json::Value nodeJson;
        nodeJson["id"] = heapArray[i].id;
        nodeJson["name"] = cleanedName; // Utilizar el nombre limpio
        priorityQueueJson.append(nodeJson);
    }

    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "\t";
    std::string jsonString = Json::writeString(writerBuilder, priorityQueueJson);

    std::cout << "Priority Queue JSON:" << std::endl;
    std::cout << jsonString << std::endl;
}




