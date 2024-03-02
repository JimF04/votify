//
// Created by jimmy on 3/2/24.
//

#include "playlist.h"
#include <iostream>
#include <uuid/uuid.h>

using namespace std;

nodo *head = NULL;
nodo *tail = NULL;

void insert_songs() {
    nodo *new_nodo = new nodo;

    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    new_nodo->id = string(uuid_str);

    cout << "Ingrese el nombre de la cancion: ";
    cin >> new_nodo->name;
    cout << "Ingrese el nombre del artista: ";
    cin >> new_nodo->artist;
    cout << "Ingrese el nombre del album: ";
    cin >> new_nodo->album;
    cout << "Ingrese el genero de la cancion: ";
    cin >> new_nodo->genre;

    new_nodo->up_votes = 0;
    new_nodo->down_votes = 0;

    cout << "Ingrese la ruta del archivo: ";
    cin >> new_nodo->file_path;

    if (head == NULL) {
        head = new_nodo;
        head->next = head;
        head->prev = head;
        tail = head;
    } else {
        tail->next = new_nodo;
        new_nodo->next = head;
        new_nodo->prev = tail;
        tail = new_nodo;
        head->prev = tail;
    }
}

void display() {
    nodo *temp = head;
    if (head != NULL) {
        do {
            cout << "Id: " << temp->id << endl;
            cout << "Nombre: " << temp->name << endl;
            cout << "Artista: " << temp->artist << endl;
            cout << "Album: " << temp->album << endl;
            cout << "Genero: " << temp->genre << endl;
            cout << "Votos positivos: " << temp->up_votes << endl;
            cout << "Votos negativos: " << temp->down_votes << endl;
            cout << "Ruta del archivo: " << temp->file_path << endl;
            cout << endl;
            temp = temp->next;
        } while (temp != head);
    } else {
        cout << "PlayList vacio" << endl;
    }
}
