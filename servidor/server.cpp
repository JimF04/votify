//
// Created by jimmy on 3/5/24.
//

#include <arpa/inet.h>
#include "server.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "priority_queue.h"
#include <json/json.h>


using namespace std;

server::server(const std::string &ipAddress, int portNum, int bufsize, PriorityQueue &pq)
        : ip_address(ipAddress), port_number(portNum), buffer_size(bufsize), pq(pq) {}

void server::start() {
    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cout << "Error creando conexión..." << std::endl;
        exit(1);
    }
    std::cout << "Servidor: Conexión creada" << std::endl;

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(ip_address.c_str()); // Usar la dirección IP proporcionada
    server_address.sin_port = htons(port_number);

    // Enlazar el socket del servidor
    if (bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        std::cout << "Error enlazando socket..." << std::endl;
        exit(1);
    }
    std::cout << "Servidor: Enlace completado" << std::endl;

    // Escuchar conexiones entrantes
    listen(server_socket, 1);

    // Aceptar una conexión entrante
    socklen_t size = sizeof(server_address);
    client_socket = accept(server_socket, (struct sockaddr *) &server_address, &size);
    if (client_socket < 0) {
        std::cout << "Error aceptando conexión..." << std::endl;
        exit(1);
    }
    std::cout << "Servidor: Conexión aceptada" << std::endl;

    // Comunicación bidireccional
    bool isExit = false;
    while (!isExit) {
        int bytesReceived = recv(client_socket, buffer, buffer_size, 0);
        if(bytesReceived<=0){
            socklen_t size = sizeof(server_address);
            client_socket = accept(server_socket, (struct sockaddr *) &server_address, &size);
            if (client_socket < 0) {
                std::cout << "Error aceptando conexión..." << std::endl;
                exit(1);
            }
            std::cout << "Servidor: Conexión aceptada" << std::endl;

        }
        else{
            recv(client_socket, buffer, buffer_size, 0);

            // Parsear el JSON
            Json::Value root;
            Json::Reader reader;
            bool parsingSuccessful = reader.parse(buffer, root);
            if (!parsingSuccessful) {
                std::cerr << "Error al parsear el JSON: " << reader.getFormattedErrorMessages();
            }

            // Obtener los valores del JSON
            const char* command = root["command"].asCString();
            const char* id = root["id"].asCString();

            // Imprimir los valores obtenidos
            std::cout << "Command: " << command << std::endl;
            std::cout << "ID: " << id << std::endl;

            rapidjson::Document document;
            document.Parse(buffer);

            if (strcmp(command, "Vote-up") == 0){
                cout<<"veaaaaaaaa: "<<id<<endl;
                pq.upVote(id);
                pq.printQueue();
            }


        }

        if (*buffer == '#') {
            isExit = true;
        }


    }

    // Cerrar la conexión del servidor
    close(client_socket);
    close(server_socket);
    std::cout << "Servidor: Conexión terminada..." << std::endl;
}

