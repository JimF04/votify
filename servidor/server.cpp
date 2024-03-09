//
// Created by jimmy on 3/5/24.
//

#include <arpa/inet.h>
#include "server.h"

using namespace std;

server::server(const std::string& ipAddress, int portNum, int bufsize)
        : ip_address(ipAddress), port_number(portNum), buffer_size(bufsize) {}

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
        recv(client_socket, buffer, buffer_size, 0);
        std::cout << "Cliente: " << buffer << std::endl;
        if (*buffer == '#') {
            isExit = true;
        }

//        std::cout << "Servidor: ";
//        std::cin.getline(buffer, buffer_size);
//        send(client_socket, buffer, buffer_size, 0);
//        if (*buffer == '#') {
//            isExit = true;
//        }
    }

    // Cerrar la conexión del servidor
    close(client_socket);
    close(server_socket);
    std::cout << "Servidor: Conexión terminada..." << std::endl;
}

