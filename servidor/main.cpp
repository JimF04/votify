#include <iostream>
#include <filesystem>
#include "playlist.h"
#include <glog/logging.h>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    Playlist p;

    google::SetLogDestination(google::GLOG_INFO, "server.log");
    google::SetLogDestination(google::GLOG_ERROR, "server.log");
    google::SetLogDestination(google::GLOG_WARNING, "server.log");
    google::SetLogDestination(google::GLOG_FATAL, "server.log");

    // Ruta de las canciones
    string folder_path = "/home/" + string(getenv("USER")) + "/Downloads/PlayList";
    LOG(INFO) << "Folder Path:" <<  folder_path;

    // Iterar sobre los archivos dentro del directorio
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            // Llamar a insert_songs() con la ruta del archivo
            p.insert_songs(entry.path().string());
        } else {
            LOG(WARNING) << "El archivo " << entry.path().string() << " no es un archivo de audio";
        }
    }

    cout << "\n";

    p.display();

    p.savePlaylistToJson("playlist.json");

    return 0;

}
