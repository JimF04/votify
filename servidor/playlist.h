//
// Created by jimmy on 3/2/24.
//

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>

using namespace std;

struct nodo {
    string id;
    string name;
    string artist;
    string album;
    string genre;
    int up_votes;
    int down_votes;
    string file_path;
    nodo* next;
    nodo* prev;
};

nodo* getCurrentSong();
nodo* getNextSong();
nodo* getPreviousSong();

void insert_songs(const string& file_path);
void display();
void savePlaylistToJson(const string& jsonFilePath);

#endif