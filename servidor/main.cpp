#include <iostream>
#include <filesystem>
#include "playlist.h"

using namespace std;
namespace fs = std::filesystem;

int main() {
    // Ruta del directorio que contiene las canciones
    string folder_path = "/home/jimmy/Downloads/PlayList";

    // Iterar sobre los archivos dentro del directorio
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file()) { // Verificar si es un archivo regular
            // Llamar a insert_songs() con la ruta del archivo
            insert_songs(entry.path().string());
        }
    }

    cout << "\n";

    display();

    return 0;
}
