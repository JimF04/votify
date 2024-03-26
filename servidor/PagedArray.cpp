#include "PagedArray.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include "INIReader.h"

PagedArray::PagedArray(const string& configFile) {
    ifstream config(configFile);
    if (!config.is_open()) {
        throw runtime_error("No se pudo abrir el archivo de configuración");
    }
    INIReader reader(configFile);

    pageSize = reader.GetInteger("paged_Array", "page_size", 10);
    totalPages = reader.GetInteger("paged_Array", "total_pages", 10);
    swapPath = reader.Get("paged_Array", "swap_path", "/swapfile");

    // Inicializar el array paginado y la matriz de páginas cargadas
    array = new int[totalPages * pageSize];
    loadedPages = new bool[totalPages]();
    currentPageIndex = -1;
}

PagedArray::~PagedArray() {
    delete[] array;
    delete[] loadedPages;
}

int& PagedArray::operator[](int index) {
    int pageIndex = index / pageSize;
    int localIndex = index % pageSize;

    if (pageIndex < 0 || pageIndex >= totalPages) {
        throw out_of_range("Índice fuera de rango");
    }

    if (!isPageLoaded(pageIndex)) {
        loadPage(pageIndex);
    }

    return array[pageIndex * pageSize + localIndex];
}

bool PagedArray::isPageLoaded(int pageIndex) {
    return loadedPages[pageIndex];
}

void PagedArray::loadPage(int pageIndex) {
    // Implementar el algoritmo LRU para reemplazar las páginas cargadas según sea necesario
    // En esta implementación básica, simplemente cargamos la página del disco al array paginado

    if (currentPageIndex != -1) {
        replacePage(pageIndex);
    } else {
        readPageFromDisk(pageIndex);
    }

    loadedPages[pageIndex] = true;
    currentPageIndex = pageIndex;
}

void PagedArray::replacePage(int pageIndex) {
    // Implementar el algoritmo LRU para reemplazar las páginas cargadas según sea necesario
    // En esta implementación básica, simplemente cargamos la página del disco al array paginado

    swapToDisk(currentPageIndex);
    readPageFromDisk(pageIndex);
    currentPageIndex = pageIndex;
}

void PagedArray::swapToDisk(int pageIndex) {
    // Implementar la lógica para escribir una página de la memoria al disco
    // En esta implementación básica, no se realiza ningún intercambio real y simplemente se imprimen mensajes de registro
    cout << "Página " << pageIndex << " swappeada al disco." << endl;
}

void PagedArray::readPageFromDisk(int pageIndex) {
    // Implementar la lógica para leer una página del disco a la memoria
    // En esta implementación básica, simplemente imprimimos mensajes de registro
    cout << "Página " << pageIndex << " leída del disco." << endl;
}

void PagedArray::writePageToDisk(int pageIndex) {
    // Implementar la lógica para escribir una página de la memoria al disco
    // En esta implementación básica, no se realiza ningún intercambio real y simplemente se imprimen mensajes de registro
    cout << "Página " << pageIndex << " escrita en el disco." << endl;
}

void PagedArray::addSong(nodo* song) {
    // Verificar si hay espacio disponible en la última página
    if (currentPageIndex == -1 || currentPageIndex == totalPages - 1) {
        // No hay espacio disponible en la última página, crea una nueva página
        addNewPage();
    }

    // Obtener el índice del primer espacio vacío en la página actual
    int startIndex = currentPageIndex * pageSize;
    for (int i = startIndex; i < startIndex + pageSize; ++i) {
        // Verificar si el espacio está vacío
        if (array[i].id.empty()) {
            // Almacena la canción en el espacio vacío
            array[i] = *song;
            return;
        }
    }

    // Si no se encontró espacio vacío en la página actual, crea una nueva página
    addNewPage();
    // Agrega la canción a la nueva página
    array[(totalPages - 1) * pageSize] = *song;
}

void PagedArray::addNewPage() {
    if (totalPages < maxTotalPages) {
        totalPages++;
        nodo* newArray = new nodo[totalPages * pageSize];
        bool* newLoadedPages = new bool[totalPages]();
        // Copiar el contenido del arreglo original al nuevo arreglo
        for (int i = 0; i < (totalPages - 1) * pageSize; ++i) {
            newArray[i] = array[i];
        }
        // Liberar la memoria del arreglo original y actualizar los punteros
        delete[] array;
        delete[] loadedPages;
        array = newArray;
        loadedPages = newLoadedPages;
        cout << "Nueva página creada." << endl;
    } else {
        cout << "No se puede agregar una nueva página, se ha alcanzado el límite máximo de páginas." << endl;
    }
}

