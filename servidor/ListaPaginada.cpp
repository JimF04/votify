#include "ListaPaginada.h"
#include <iostream>

ListaPaginada::ListaPaginada(int maxPages, int numSongsPerPage, PlaylistObserver* obs)
        : MAX_PAGES(maxPages), NUM_SONGS_PER_PAGE(numSongsPerPage), observer(obs) {

    // Calcula el tamaño total de cada nodo de canción
    int nodeSize = sizeof(nodo*);

    std::cout << "Tamaño de cada nodo: " << nodeSize << " bytes" << std::endl;

    // Calcula el tamaño total de la página en bytes
    PAGE_SIZE = nodeSize * NUM_SONGS_PER_PAGE;
    paginas = new nodo**[MAX_PAGES];
    for (int i = 0; i < MAX_PAGES; ++i) {
        paginas[i] = new nodo*[NUM_SONGS_PER_PAGE];
        for (int j = 0; j < NUM_SONGS_PER_PAGE; ++j) {
            paginas[i][j] = nullptr;
        }
    }
    lastAccess = new int[MAX_PAGES];
    for (int i = 0; i < MAX_PAGES; ++i) {
        lastAccess[i] = 0;
    }
    currentPageIndex = 0;
    currentSongIndex = 0;
}

void ListaPaginada::clearPlaylistPaginated() {
    for (int i = 0; i < MAX_PAGES; ++i) {
        for (int j = 0; j < NUM_SONGS_PER_PAGE; ++j) {
            paginas[i][j] = nullptr;
        }
    }
    currentPageIndex = 0;
    currentSongIndex = 0;
}


ListaPaginada::~ListaPaginada() {
    clearPlaylistPaginated();
}

void ListaPaginada::paginar(Playlist& playlist) {
    nodo* cancionActual = playlist.getCurrentSong();
    int pageIndex = 0;
    int songIndex = 0;
    int totalSongs = 0;

    while (cancionActual != nullptr && totalSongs < MAX_PAGES * NUM_SONGS_PER_PAGE) {
        paginas[pageIndex][songIndex] = cancionActual;
        songIndex++;
        totalSongs++;

        // Si se llenó la página o se alcanzó el final de la lista de reproducción
        if (songIndex >= NUM_SONGS_PER_PAGE) {
            pageIndex++;
            songIndex = 0;
        }

        // Si se alcanza el límite máximo de páginas, detener la paginación
        if (pageIndex >= MAX_PAGES) {
            break;
        }

        cancionActual = playlist.getNextSong();
    }

    // Actualizar los índices de página y canción actual para reflejar la posición actual
    currentPageIndex = 0;
    currentSongIndex = 0;
}




void ListaPaginada::imprimirPaginaActual() {
    if (currentPageIndex < 0 || currentPageIndex >= MAX_PAGES) {
        std::cout << "Índice de página actual fuera de rango" << std::endl;
        return;
    }

    std::cout << "Contenido de la página actual:" << std::endl;
    for (int i = 0; i < NUM_SONGS_PER_PAGE; ++i) {
        if (paginas[currentPageIndex][i] != nullptr) {
            std::cout << "Canción " << i + 1 << ": " << paginas[currentPageIndex][i]->name << std::endl;
            // Imprimir más información sobre la canción si es necesario
        } else {
            std::cout << "Canción " << i + 1 << ": [Vacío]" << std::endl;
        }
    }
}


nodo* ListaPaginada::obtenerSiguienteCancion() {
    currentSongIndex++;
    if (currentSongIndex >= PAGE_SIZE) {
        currentSongIndex = 0;
        currentPageIndex++;
        if (currentPageIndex >= MAX_PAGES) {
            return nullptr; // Se alcanzó el final de la lista paginada
        }
    }
    return paginas[currentPageIndex][currentSongIndex];
}

nodo* ListaPaginada::obtenerCancionPorIndice(int index) {
    // Calcular el índice de la página y el índice de la canción dentro de esa página
    int pageIndex = index / NUM_SONGS_PER_PAGE;
    int songIndex = index % NUM_SONGS_PER_PAGE;

    // Verificar si la página está dentro de los límites
    if (pageIndex < 0 || pageIndex >= MAX_PAGES) {
        std::cout << "Índice de página fuera de rango" << std::endl;
        return nullptr;
    }

    // Verificar si la canción está dentro de los límites de la página actual
    if (songIndex < 0 || songIndex >= NUM_SONGS_PER_PAGE) {
        std::cout << "Índice de canción fuera de rango en la página actual" << std::endl;
        return nullptr;
    }

    // Acceder y devolver la canción desde la página correspondiente
    return paginas[pageIndex][songIndex];
}


void ListaPaginada::swapOut(int pageIndex) {
    // Implementar intercambio de página con el disco
}

void ListaPaginada::swapIn(int pageIndex) {
    // Implementar intercambio de página desde el disco
}

void ListaPaginada::imprimirCancionPorIndice(int index) {
    nodo* cancion = obtenerCancionPorIndice(index);

    if (cancion != nullptr) {
        std::cout << "ID: " << cancion->id << std::endl;
        std::cout << "Nombre: " << cancion->name << std::endl;
        std::cout << "Artista: " << cancion->artist << std::endl;
        std::cout << "Álbum: " << cancion->album << std::endl;
        std::cout << "Género: " << cancion->genre << std::endl;
        std::cout << "Votos positivos: " << cancion->up_votes << std::endl;
        std::cout << "Votos negativos: " << cancion->down_votes << std::endl;
        std::cout << "Duración: " << cancion->songDuration << " segundos" << std::endl;
        std::cout << "Path: " << cancion->file_path << std::endl;
        // Puedes imprimir más campos si los tienes en tu estructura de datos
    } else {
        std::cout << "No se encontró ninguna canción en el índice especificado." << std::endl;
    }
}

void ListaPaginada::imprimirTodasLasCanciones() {
    std::cout << "Contenido de todas las páginas:" << std::endl;
    for (int pageIndex = 0; pageIndex < MAX_PAGES; ++pageIndex) {
        std::cout << "Página " << pageIndex + 1 << ":" << std::endl;
        for (int songIndex = 0; songIndex < NUM_SONGS_PER_PAGE; ++songIndex) {
            if (paginas[pageIndex][songIndex] != nullptr) {
                std::cout << "Canción " << (pageIndex * NUM_SONGS_PER_PAGE) + songIndex + 1 << ": ";
                std::cout << paginas[pageIndex][songIndex]->name << std::endl;
                // Imprime más detalles si es necesario
            } else {
                std::cout << "Canción " << (pageIndex * NUM_SONGS_PER_PAGE) + songIndex + 1 << ": [Vacío]" << std::endl;
            }
        }
    }
}
