//
// Created by jimmy on 3/24/24.
//

#ifndef LISTAPAGINADA_H
#define LISTAPAGINADA_H

#include <string>
#include "playlist.h" // Incluye el archivo Playlist.h donde está definida la clase Playlist

class ListaPaginada {
private:
    nodo*** paginas; // Arreglo tridimensional de punteros a nodos
    const int MAX_PAGES; // Número máximo de páginas permitidas
    int NUM_SONGS_PER_PAGE; // Número de canciones por página
    int PAGE_SIZE; // Tamaño de la página en bytes
    int* lastAccess; // Arreglo para mantener el último acceso a cada página
    int currentPageIndex; // Índice de la página actual
    int currentSongIndex; // Índice de la canción actual en la página actual
    PlaylistObserver* observer; // Observador de la lista paginada
    const string swapFilePath; // Ruta del archivo de intercambio

public:
    ListaPaginada(int maxPages, int numSongsPerPage, PlaylistObserver* obs, const string& swapFilePaTH); // Constructor
    ~ListaPaginada(); // Destructor
    void paginar(Playlist& playlist); // Método para paginar la lista de canciones
    void imprimirPaginaActual(); // Método para imprimir la página actual
    nodo* obtenerSiguienteCancion(); // Método para obtener la siguiente canción
    nodo* obtenerCancionPorIndice(int index); // Método para obtener una canción por índice
    void swapOut(int pageIndex); // Método para intercambiar una página con el disco
    void swapIn(int pageIndex); // Método para intercambiar una página desde el disco
    void imprimirCancionPorIndice(int index);
    void imprimirTodasLasCanciones();
    void clearPlaylistPaginated(); // Método para limpiar la lista paginada
};

#endif


