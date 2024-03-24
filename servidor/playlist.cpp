//
// Created by jimmy on 3/2/24.
//

#include "playlist.h"
#include <iostream>
#include <filesystem>
#include <uuid/uuid.h>
#include <json/json.h>
#include <fstream>
#include <csignal>
#include <glog/logging.h>

namespace fs = std::filesystem;

Playlist::Playlist() {
    head = nullptr;
    tail = nullptr;
}

Playlist::~Playlist() {
    // Liberar la memoria de todos los nodos
    nodo* current = head;
    while (current != nullptr) {
        nodo* next = current->next;
        delete current;
        current = next;
    }
}

void Playlist::insertSong(const string& file_path) {
    nodo *new_nodo = new nodo;

    // Generar un UUID para el nodo
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    new_nodo->id = string(uuid_str);

    fs::path path(file_path);

    // Ejecutar el comando mpg123 para obtener los metadatos
    string command = "mpg123 -t t \"" + file_path + "\" 2>&1";
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Error al ejecutar el comando mpg123." << endl;
        return;
    }

    // Procesar la salida del comando para extraer los metadatos
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        string line(buffer);

        // Buscar las líneas que contienen los metadatos y extraer los valores correspondientes
        size_t pos;
        if ((pos = line.find("Title:")) != string::npos) {
            new_nodo->name = line.substr(pos + 7);
        } else if ((pos = line.find("Artist:")) != string::npos) {
            new_nodo->artist = line.substr(pos + 8);
        } else if ((pos = line.find("Album:")) != string::npos) {
            new_nodo->album = line.substr(pos + 7);
        } else if ((pos = line.find("Genre:")) != string::npos) {
            new_nodo->genre = line.substr(pos + 7);
        }
    }
    pclose(pipe);

    string command2 = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 " + file_path;

    // Ejecutar el comando y capturar la salida
    FILE *pipe2 = popen(command2.c_str(), "r");
    if (!pipe2) {
        cerr << "Error: No se pudo ejecutar el comando." << endl;
        return;
    }

    char buffer2[128];
    string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);

    // Convertir la salida (que es una cadena) a un double (la duración en segundos)
    int duration = stod(result);

    new_nodo->songDuration = duration;

    // Inicializar votos en 0
    new_nodo->up_votes = 0;
    new_nodo->down_votes = 0;

    // Asignar la ruta del archivo al nodo
    new_nodo->file_path = file_path;
    new_nodo->observer = nullptr; // Inicializar el observador como nullptr

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

void Playlist::deleteSong(const string& songId) {
    nodo* temp = head;
    while (temp != nullptr) {
        if (temp->id == songId) {
            // Eliminar el nodo de la lista
            if (temp == head) {
                head = head->next;
            }
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;

            // Notificar al observador si está configurado
            if (observer != nullptr) {
                observer->onSongDeleted(songId);
            }

            delete temp;
            return;
        }
        temp = temp->next;
        if (temp == head) {
            break; // Se ha recorrido toda la lista
        }
    }
}

nodo* Playlist::getCurrentSong() {
    return head;
}

nodo* Playlist::getNextSong() {
    if (head != nullptr) {
        head = head->next;
        return head;
    } else {
        return nullptr;
    }
}

nodo* Playlist::getPreviousSong() {
    if (head != nullptr) {
        head = head->prev;
        return head;
    } else {
        return nullptr;
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
            cout << "Duracion de la cancion: " << temp->songDuration << endl;
            cout << endl;
            temp = temp->next;
        } while (temp != head);
    } else {
        cout << "PlayList vacio" << endl;
    }
}

void Playlist::saveToJson(const string& jsonFilePath) {
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
        cout << "Se guardo correctamente" << endl;
    } else {
        cerr << "No se pudo guardar" << endl;
    }
}

void Playlist::registerObserver(PlaylistObserver* observer) {
    this->observer = observer;
}

void Playlist::unregisterObserver(PlaylistObserver* observer) {
    if (this->observer == observer) {
        this->observer = nullptr;
    }
}

void Playlist::insertUniqueArtist(const string &artist, string *uniqueArtists, int &artistCount) {
    for (int i = 0; i < artistCount; ++i) {
        // Verificar si el artista ya está en la lista de artistas únicos
        if (uniqueArtists[i] == artist) {
            return; // Si el artista ya está presente, salir sin hacer nada
        }
    }
    // Si el artista no está presente, agregarlo a la lista de artistas únicos
    uniqueArtists[artistCount++] = artist;
}

string* Playlist::getUniqueArtists() {
    // Determinar el tamaño máximo de los arreglos basado en la cantidad de nodos en la lista de reproducción
    int playlistSize = 0;
    nodo *temp = head;
    if (head != nullptr) {
        do {
            playlistSize++;
            temp = temp->next;
        } while (temp != head);
    }

    // Arreglos para almacenar los artistas únicos y la cantidad de artistas únicos
    string* uniqueArtists = new string[playlistSize];
    int artistCount = 0;

    temp = head;
    if (head != nullptr) {
        do {
            // Insertar el artista actual en la lista de artistas únicos
            insertUniqueArtist(temp->artist, uniqueArtists, artistCount);
            temp = temp->next;
        } while (temp != head);
    }

    // Devolver el arreglo de artistas únicos
    return uniqueArtists;
}

void Playlist::upVote(const string& songId) {
    nodo *temp = head;
    if (temp->id == songId) {
        temp->up_votes++;
        return;
    }
}

void Playlist::downVote(const string& songId) {
    nodo* temp = head;
    if (temp->id == songId) {
        temp->down_votes++;
        return;
    }
}