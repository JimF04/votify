#include <iostream>
#include <filesystem>
#include "playlist.h"

using namespace std;
namespace fs = std::filesystem;

int main() {
    // Ruta de las canciones
    string folder_path = "/home/" + string(getenv("USER")) + "/Downloads/PlayList";

    // Iterar sobre los archivos dentro del directorio
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            // Llamar a insert_songs() con la ruta del archivo
            insert_songs(entry.path().string());
        } else {
            cout << "El archivo " << entry.path().string() << " no es un archivo de audio" << endl;
        }
    }

    cout << "\n";

    display();

    savePlaylistToJson("playlist.json");

    return 0;
}
