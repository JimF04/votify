// playlist.h

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>

struct nodo {
    std::string id;
    std::string name;
    std::string artist;
    std::string album;
    std::string genre;
    int up_votes;
    int down_votes;
    std::string file_path;
    nodo* next;
    nodo* prev;
};

void insert_songs(const std::string& file_path);
nodo* getCurrentSong();
nodo* getNextSong();
nodo* getPreviousSong();
void display();
void savePlaylistToJson(const std::string& jsonFilePath);

#endif  // PLAYLIST_H
