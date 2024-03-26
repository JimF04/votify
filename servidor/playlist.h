//
// Created by jimmy on 3/2/24.
//

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <uuid/uuid.h>
#include <json/json.h>

using namespace std;

// Declaración de la clase PlaylistObserver
class PlaylistObserver {
public:
    virtual void onSongDeleted(const string& songId) = 0;
};

// Declaración del tipo nodo
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
    PlaylistObserver* observer;
    int songDuration;
};

// Declaración de la clase Playlist
class Playlist {
public:
    Playlist();
    ~Playlist();

    void insertSong(const string& file_path);
    void deleteSong(const string& songId);
    nodo* getCurrentSong();
    nodo* getNextSong();
    nodo* getPreviousSong();
    void display();
    void saveToJson(const string& jsonFilePath);
    void registerObserver(PlaylistObserver* observer);
    void unregisterObserver(PlaylistObserver* observer);
    static void insertUniqueArtist(const string& artist, string uniqueArtists[], int& artistCount);
    string* getUniqueArtists();
    void upVote(const string& songId);
    void downVote(const string& songId);
    void clearPlaylist();

private:
    nodo *head;
    nodo *tail;
    PlaylistObserver* observer;
};

#endif
