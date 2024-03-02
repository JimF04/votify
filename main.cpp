#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;

int main() {
    int client;
    int portNum = 1500;
    int bufsize = 1024;
    char buffer[bufsize];
    const char *ip = "127.0.0.1";

    struct sockaddr_in server_addr;

    // Crear el socket del cliente
    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "Error creando socket..." << endl;
        exit(1);
    }
    cout << "Cliente: Socket creado" << endl;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    // Conectar al servidor
    if (connect(client, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
        cout << "Cliente: Conectando al servidor..." << endl;
    } else {
        cout << "Error conectando al servidor..." << endl;
        exit(1);
    }

    // Comunicación bidireccional
    bool isExit = false;
    while (!isExit) {
        cout << "Cliente: ";
        cin.getline(buffer, bufsize);
        send(client, buffer, bufsize, 0);
        if (*buffer == '#') {
            isExit = true;
        }

        recv(client, buffer, bufsize, 0);
        cout << "Servidor: " << buffer << endl;
        if (*buffer == '#') {
            isExit = true;
        }
    }

    // Cerrar el socket del cliente
    close(client);
    cout << "Cliente: Conexión terminada..." << endl;

    return 0;
}
