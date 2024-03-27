//
// Created by jimmy on 3/5/24.
//

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "priority_queue.h"

using namespace std;

class server {
private:
    int server_socket;
    int client_socket;
    int port_number;
    int buffer_size;
    string ip_address;
    struct sockaddr_in server_address;
    char buffer[1024];
    PriorityQueue& pq;

public:
    server(const std::string &ipAddress, int portNum, int bufsize, PriorityQueue &pq);

    void start();
};

#endif


