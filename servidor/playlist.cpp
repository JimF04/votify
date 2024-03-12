//
// Created by jimmy on 3/2/24.
//

#include "playlist.h"
#include <iostream>
#include <filesystem>
#include <uuid/uuid.h>
#include <json/json.h>
#include <fstream>
#include <glog/logging.h>

using namespace std;
namespace fs = std::filesystem;

Playlist::Playlist() {
    head = nullptr;
    tail = nullptr;
}

nodo* Playlist::getHead() const {
    return head;
}

nodo* Playlist::getTail() const {
    return tail;
}

void Playlist::setHead(nodo* newHead) {
    head = newHead;
}

void Playlist::setTail(nodo* newTail) {
    tail = newTail;
}

void Playlist::insert_songs(const string& file_path) {
    nodo *new_nodo = new nodo;

    // Generar un UUID para el nodo
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    new_nodo->id = string(uuid_str);

    // Falta por agregar por medio de metadata
    fs::path path(file_path);
    new_nodo->name = path.filename().string();

    new_nodo->artist = "";
    new_nodo->album = "";
    new_nodo->genre = "pop";

    // Inicializar votos en 0
    new_nodo->up_votes = 0;
    new_nodo->down_votes = 0;

    // Asignar la ruta del archivo al nodo
    new_nodo->file_path = file_path;

    // Insertar el nuevo nodo en la lista
    if (head == nullptr) {
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

void Playlist::display() {
    nodo *temp = head;
    if (head != nullptr) {
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
        LOG(FATAL) << "PlayList vacio";
    }
}

void Playlist::savePlaylistToJson(const string& jsonFilePath) {
    Json::Value playlistJson(Json::arrayValue); // Array JSON para almacenar los nodos

    nodo* temp = head;
    if (head != nullptr) {
        do {
            // Crear un objeto JSON para el nodo actual
            Json::Value nodeJson;
            nodeJson["id"] = temp->id;
            nodeJson["name"] = temp->name;
            nodeJson["artist"] = temp->artist;
            nodeJson["album"] = temp->album;
            nodeJson["genre"] = temp->genre;
            nodeJson["up_votes"] = temp->up_votes;
            nodeJson["down_votes"] = temp->down_votes;
            nodeJson["file_path"] = temp->file_path;

            // Agregar el objeto JSON del nodo al array JSON de la playlist
            playlistJson.append(nodeJson);

            temp = temp->next;
        } while (temp != head);
    }

    // Guardar el Json en un archivo
    ofstream outputFile(jsonFilePath);
    if (outputFile.is_open()) {
        outputFile << playlistJson;
        outputFile.close();
        LOG(INFO) << "Se guardo playlist en json correctamente";
    } else {
        LOG(ERROR) << "No se pudo guardar playlist en json";
    }
}
