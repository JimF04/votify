#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <jsoncpp/json/value.h>


using namespace std;

int main() {
    int server, client;
    int portNum = 1500;
    int bufsize = 1024;
    char buffer[bufsize];
    

    struct sockaddr_in server_addr;

    // Crear el socket del servidor
    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        cout << "Error creando conexión..." << endl;
        exit(1);
    }
    cout << "Servidor: Conexión creada" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(portNum);

    // Enlazar el socket del servidor
    if (bind(server, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        cout << "Error enlazando socket..." << endl;
        exit(1);
    }
    cout << "Servidor: Enlace completado" << endl;

    // Escuchar conexiones entrantes
    listen(server, 1);

    // Aceptar una conexión entrante
    socklen_t size = sizeof(server_addr);
    client = accept(server, (struct sockaddr *) &server_addr, &size);
    if (client < 0) {
        cout << "Error aceptando conexión..." << endl;
        exit(1);
    }
    cout << "Servidor: Conexión aceptada" << endl;

    // Comunicación bidireccional
    bool isExit = false;
    while (!isExit) {
        recv(client, buffer, bufsize, 0);
        cout << "Cliente: " << buffer << endl;
        if (*buffer == '#') {
            isExit = true;
        }

        cout << "Servidor: ";
        cin.getline(buffer, bufsize);
        send(client, buffer, bufsize, 0);
        if (*buffer == '#') {
            isExit = true;
        }
    }

    // Cerrar la conexión del servidor
    close(client);
    close(server);
    cout << "Servidor: Conexión terminada..." << endl;

    return 0;
}
