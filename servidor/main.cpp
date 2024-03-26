#define MINIAUDIO_IMPLEMENTATION
#include <iostream>
#include <filesystem>
#include "playlist.h"
#include <gtk/gtk.h>
#include <thread>
#include <cstdlib>
#include "server.h"
#include "miniaudio.h"
#include <stdio.h>
//#include <glog/logging.h>
#include "INIReader.h"
#include <random>
#include <set>
#include <iterator>

using namespace std;
namespace fs_std = std::filesystem;

Playlist myPlaylist;
Playlist randomPlaylist;

bool isPlaying = false;
bool CPisOn = false;

ma_uint64 currentPosition = 0;
int songDuration;

GSource *timer_source = NULL;
gboolean updating_slider = FALSE;

ma_decoder decoder;
ma_device device;
ma_result result;
ma_device_config deviceConfig;

string ini_path =  "/home/" + string(getenv("USER")) + "/Documents/GitHub/votify/servidor/config.ini";
INIReader reader(ini_path);

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

static void playAudio(const char* filePath, ma_uint64 startSeconds)
{
    result = ma_decoder_init_file(filePath, NULL, &decoder);
    if (result != MA_SUCCESS) {
        printf("Could not load file: %s\n", filePath);
        return;
    }

    ma_uint64 startFrame = startSeconds * decoder.outputSampleRate;

    result = ma_decoder_seek_to_pcm_frame(&decoder, startFrame);
    if (result != MA_SUCCESS) {
        printf("Could not seek to the specified position.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    deviceConfig = ma_device_config_init(ma_device_type_playback);
    deviceConfig.playback.format   = decoder.outputFormat;
    deviceConfig.playback.channels = decoder.outputChannels;
    deviceConfig.sampleRate        = decoder.outputSampleRate;
    deviceConfig.dataCallback      = data_callback;
    deviceConfig.pUserData         = &decoder;

    if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
        printf("Failed to open playback device.\n");
        ma_decoder_uninit(&decoder);
        return;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        return;
    }

    isPlaying = true;
}

ma_uint64 getCurrentPositionInSeconds(ma_decoder* pDecoder) {
    ma_uint64 currentFrame;
    ma_result result = ma_decoder_get_cursor_in_pcm_frames(pDecoder, &currentFrame);
    if (result != MA_SUCCESS) {
        printf("Error al obtener la posición actual.\n");
        return 0;
    }

    ma_uint64 sampleRate = pDecoder->outputSampleRate;
    return currentFrame / sampleRate;
}

GtkBuilder *builder;
GtkWidget *main_window;
GtkWidget *NameLabel;
GtkWidget *ArtistLabel;
GtkWidget *AlbumLabel;
GtkWidget *GenreLabel;
GtkWidget *UpVotesLabel;
GtkWidget *DownVotesLabel;
GtkWidget *MemCounterLabel;

GtkWidget *PreviousButton;
GtkWidget *PlayButton;
GtkWidget *StopButton;
GtkWidget *NextButton;
GtkWidget *PaginateButton;
GtkWidget *DeleteButton;
GtkWidget *TimeSlider;
GtkWidget *VolumeSlider;
GtkWidget *startTime;
GtkWidget *endTime;
GtkWidget *artistBox;
GtkWidget *upVoteButton;
GtkWidget *downVoteButton;

GtkWidget *CPButton;

typedef struct {
    GtkWidget *button;
    char *artistId;
} ArtistButtonData;


GtkWidget *songArtistBox;


void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data);
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data);
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data);
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data);
void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data);
void on_PaginateButton_toggled(GtkToggleButton *button, gpointer user_data);
void on_TimeSlider_value_changed(GtkRange *range, gpointer user_data);
void on_VolumeSlider_value_changed(GtkRange *range, gpointer user_data);
static void change_slider_value(GtkScale *scale, gdouble value);
void on_artist_button_clicked(GtkButton *button, gpointer user_data);
void on_upVote_clicked(GtkButton *button, gpointer user_data);
void on_downVote_clicked(GtkButton *button, gpointer user_data);
void on_CP_clicked(GtkButton *button, gpointer user_data);
void updateSongLabels(const string& songName, const string& artistName, const string& albumName, const string& genreName, int upVotes, int downVotes, int songDuration);
static gboolean updateRamUsage(gpointer data);
static double getServerRamUsage(pid_t server_pid);


int main(int argc, char *argv[]) {
//
//    google::InitGoogleLogging(argv[0]);
//
//    google::SetLogDestination(google::GLOG_INFO, "server.log");
//    google::SetLogDestination(google::GLOG_ERROR, "server.log");
//    google::SetLogDestination(google::GLOG_WARNING, "server.log");
//    google::SetLogDestination(google::GLOG_FATAL, "server.log");

    string songs_path = reader.Get("paths", "songs_path", "");

    // Iterar sobre los archivos dentro del directorio
    for (const auto& entry : fs_std::directory_iterator(songs_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            // Llamar a insert_songs() con la ruta del archivo
            myPlaylist.insertSong(entry.path().string());
        } else {
            //LOG(WARNING) << "El archivo " << entry.path().string() << " no es un archivo de audio" << endl;
        }
    }

    cout << "\n";

    string jsonPath = reader.Get("paths", "json_path", "");
    myPlaylist.saveToJson(jsonPath);

    gtk_init(&argc, &argv);

    string gladePath = reader.Get("paths", "glade_path", "");

    // Carga la interfaz desde el archivo Glade
    builder = gtk_builder_new_from_file(gladePath.c_str());

    // Obtiene los widgets necesarios de la interfaz
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *TimeGrid = GTK_WIDGET(gtk_builder_get_object(builder, "TimeGrid"));
    GtkWidget *VolumeBox = GTK_WIDGET(gtk_builder_get_object(builder, "VolumeBox"));


    NameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "NameLabel"));
    ArtistLabel = GTK_WIDGET(gtk_builder_get_object(builder, "ArtistLabel"));
    AlbumLabel = GTK_WIDGET(gtk_builder_get_object(builder, "AlbumLabel"));
    GenreLabel = GTK_WIDGET(gtk_builder_get_object(builder, "GenreLabel"));
    UpVotesLabel = GTK_WIDGET(gtk_builder_get_object(builder, "UpVotesLabel"));
    DownVotesLabel = GTK_WIDGET(gtk_builder_get_object(builder, "DownVotesLabel"));
    MemCounterLabel = GTK_WIDGET(gtk_builder_get_object(builder, "MemCounterLabel"));
    g_timeout_add_seconds(1, updateRamUsage, NULL);

    endTime = GTK_WIDGET(gtk_builder_get_object(builder, "endTime"));
    startTime = GTK_WIDGET(gtk_builder_get_object(builder, "startTime"));
    gtk_grid_attach(GTK_GRID(TimeGrid), endTime, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(TimeGrid), startTime, 2, 0, 1, 1);

    TimeSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    g_signal_connect(TimeSlider, "value-changed", G_CALLBACK(on_TimeSlider_value_changed), NULL);
    g_signal_connect(TimeSlider, "change-value", G_CALLBACK(change_slider_value), NULL);
    gtk_widget_set_size_request(TimeSlider, 400, -1); // Establece un tamaño fijo para el slider
    gtk_grid_attach(GTK_GRID(TimeGrid), TimeSlider, 1, 1, 1, 1);

    VolumeSlider = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL, 0, 100, 1);
    gtk_range_set_inverted(GTK_RANGE(VolumeSlider), TRUE);
    gtk_range_set_value(GTK_RANGE(VolumeSlider), 80);
    g_signal_connect(VolumeSlider, "value-changed", G_CALLBACK(on_VolumeSlider_value_changed), NULL);
    gtk_widget_set_size_request(VolumeSlider, 10, 200);
    gtk_box_pack_end(GTK_BOX(VolumeBox), VolumeSlider, FALSE, FALSE, 0);


    gtk_builder_connect_signals(builder, NULL);

    PreviousButton = GTK_WIDGET(gtk_builder_get_object(builder, "PreviousButton"));
    g_signal_connect(PreviousButton, "clicked", G_CALLBACK(on_PreviousButton_clicked), NULL);

    PlayButton = GTK_WIDGET(gtk_builder_get_object(builder, "PlayButton"));
    g_signal_connect(PlayButton, "clicked", G_CALLBACK(on_PlayButton_clicked), NULL);

    StopButton = GTK_WIDGET(gtk_builder_get_object(builder, "StopButton"));
    g_signal_connect(StopButton, "clicked", G_CALLBACK(on_StopButton_clicked), NULL);

    NextButton = GTK_WIDGET(gtk_builder_get_object(builder, "NextButton"));
    g_signal_connect(NextButton, "clicked", G_CALLBACK(on_NextButton_clicked), NULL);

    CPButton = GTK_WIDGET(gtk_builder_get_object(builder, "CPButton"));
    g_signal_connect(CPButton, "clicked", G_CALLBACK(on_CP_clicked), NULL);

    DeleteButton = GTK_WIDGET(gtk_builder_get_object(builder, "DeleteButton"));
    g_signal_connect(DeleteButton, "clicked", G_CALLBACK(on_DeleteButton_clicked), NULL);

    PaginateButton = GTK_WIDGET(gtk_builder_get_object(builder, "PaginateButton"));
    g_signal_connect(PaginateButton, "toggled", G_CALLBACK(on_PaginateButton_toggled), NULL);

    upVoteButton = GTK_WIDGET(gtk_builder_get_object(builder, "upVote"));
    g_signal_connect(upVoteButton, "clicked", G_CALLBACK(on_upVote_clicked), NULL);

    downVoteButton = GTK_WIDGET(gtk_builder_get_object(builder, "downVote"));
    g_signal_connect(downVoteButton, "clicked", G_CALLBACK(on_downVote_clicked), NULL);

    songArtistBox = GTK_WIDGET(gtk_builder_get_object(builder, "SongsByArtistBx"));

    GtkWidget *label2 = gtk_label_new("Songs by Artist:");
    gtk_box_pack_start(GTK_BOX(songArtistBox), label2, FALSE, FALSE, 0);
    gtk_widget_show(label2);

    artistBox = GTK_WIDGET(gtk_builder_get_object(builder, "ArtistsBox"));
    GtkWidget *label = gtk_label_new("Artists:");
    gtk_box_pack_start(GTK_BOX(artistBox), label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    string *uniqueArtists = myPlaylist.getUniqueArtists();

    int artistCount = 0;
    while (!uniqueArtists[artistCount].empty()) {
        artistCount++;
    }

    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gtk_box_pack_start(GTK_BOX(artistBox), scrolled_window, TRUE, TRUE, 0);
    gtk_widget_show(scrolled_window);

    GtkWidget *artist_button_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrolled_window), artist_button_box);
    gtk_widget_show(artist_button_box);

    // Iterar cada artista para el botonsito
    for (int i = 0; i < artistCount; ++i) {
        const std::string artist = uniqueArtists[i]; // Almacena el artista como std::string

        GtkWidget *button = gtk_button_new_with_label(artist.c_str());
        g_signal_connect(button, "clicked", G_CALLBACK(on_artist_button_clicked), NULL);

        ArtistButtonData *button_data = (ArtistButtonData *)malloc(sizeof(ArtistButtonData));
        button_data->button = button;
        button_data->artistId = strdup(artist.c_str());
        g_object_set_data(G_OBJECT(button), "button_data", button_data);

        gtk_box_pack_start(GTK_BOX(artist_button_box), button, FALSE, FALSE, 0);
        gtk_widget_show(button);
    }
    delete[] uniqueArtists;

    // Info de la primera cancion de la lista
    nodo* currentSong = myPlaylist.getCurrentSong();
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                     currentSong->genre, currentSong->up_votes, currentSong->down_votes, currentSong->songDuration);

    g_object_unref(builder);
    gtk_widget_show_all(main_window);
    gtk_main();

    return EXIT_SUCCESS;
}

// Muestra la lista de canciones por artista
void showSongsByArtist(const std::string& artistName, GtkWidget *songArtistBox) {
    // Eliminar cualquier widget hijo existente en el contenedor de la caja de canciones de artistas
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(songArtistBox));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

     //Crear labels para cada cancion del artista
    GtkWidget *label = gtk_label_new("Songs by Artist:");
    gtk_box_pack_start(GTK_BOX(songArtistBox), label, FALSE, FALSE, 0);
    gtk_widget_show(label);

    GtkWidget *blanck = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(songArtistBox), blanck, FALSE, FALSE, 0);
    gtk_widget_show(blanck);

    nodo *currentSong = myPlaylist.getCurrentSong();
    do {
        if (currentSong->artist == artistName) {
            GtkWidget *song_label = gtk_label_new(currentSong->name.c_str());
            gtk_box_pack_start(GTK_BOX(songArtistBox), song_label, FALSE, FALSE, 0);
            gtk_widget_show(song_label);
        }
        currentSong = currentSong->next;
    } while (currentSong != myPlaylist.getCurrentSong());
}

void on_artist_button_clicked(GtkButton *button, gpointer user_data) {
    ArtistButtonData *button_data = (ArtistButtonData *)g_object_get_data(G_OBJECT(button), "button_data");
    // Mostrar las canciones del artista
    showSongsByArtist(button_data->artistId, songArtistBox);
}

void updateSongLabels(const string& songName, const string& artistName, const string& albumName, const string& genreName, int upVotes, int downVotes, int songDuration)  {
    gtk_label_set_text(GTK_LABEL(NameLabel), ("Song: " + songName).c_str());
    gtk_label_set_text(GTK_LABEL(ArtistLabel), ("Artist: " + artistName).c_str());
    gtk_label_set_text(GTK_LABEL(AlbumLabel), ("Album: " + albumName).c_str());
    gtk_label_set_text(GTK_LABEL(GenreLabel), ("Genre: " + genreName).c_str());
    gtk_label_set_text(GTK_LABEL(UpVotesLabel), ("Up Votes: " + to_string(upVotes)).c_str());
    gtk_label_set_text(GTK_LABEL(DownVotesLabel), ("Down Votes: " + to_string(downVotes)).c_str());
    gtk_label_set_text(GTK_LABEL(endTime), (to_string(songDuration) + " s").c_str());
}

static gboolean updateRamUsage(gpointer data) {
    double ramUsage = getServerRamUsage(getpid()); // Obtener el uso de RAM del proceso actual
    std::ostringstream ramUsageStr;
    ramUsageStr << "RAM Usage: " << std::fixed << std::setprecision(2) << ramUsage << " MB";
    gtk_label_set_text(GTK_LABEL(MemCounterLabel), ramUsageStr.str().c_str());
    return G_SOURCE_CONTINUE;
}

static double getServerRamUsage(pid_t server_pid) {
    std::ifstream file("/proc/" + std::to_string(server_pid) + "/statm");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open /proc/" << server_pid << "/statm" << std::endl;
        return 0.0;
    }

    int totalMemoryPages;
    int residentSetPages;
    int sharedPages;
    if (!(file >> totalMemoryPages >> residentSetPages >> sharedPages)) {
        std::cerr << "Error: Failed to read memory information from /proc/" << server_pid << "/statm" << std::endl;
        return 0.0;
    }

    const long pageSize = sysconf(_SC_PAGESIZE) / 1024;
    double residentMemoryKb = (residentSetPages - sharedPages) * pageSize;
    return residentMemoryKb / 1024.0;
}

void on_upVote_clicked(GtkButton *button, gpointer user_data) {

    nodo* currentSong = nullptr;

    if (CPisOn){
        currentSong = randomPlaylist.getCurrentSong();
        randomPlaylist.upVote(currentSong->id);
    } else {
        currentSong = myPlaylist.getCurrentSong();
        myPlaylist.upVote(currentSong->id);
    }
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                 currentSong->genre, currentSong->up_votes, currentSong->down_votes, currentSong->songDuration);
}

void on_downVote_clicked(GtkButton *button, gpointer user_data) {

    nodo* currentSong = nullptr;

    if (CPisOn) {
        currentSong = randomPlaylist.getCurrentSong();
        randomPlaylist.downVote(currentSong->id);
    } else {
        currentSong = myPlaylist.getCurrentSong();
        myPlaylist.downVote(currentSong->id);
    }
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                 currentSong->genre, currentSong->up_votes, currentSong->down_votes, currentSong->songDuration);
}

static gboolean update_slider(gpointer user_data) {
    if (!updating_slider) {
        updating_slider = TRUE;

        // Incrementar el valor por 1 segundo
        double value = gtk_range_get_value(GTK_RANGE(TimeSlider));
        value += 1.0;

        gtk_range_set_value(GTK_RANGE(TimeSlider), value);

        nodo* currentSong = myPlaylist.getCurrentSong();
        if (value >= currentSong->songDuration) {
            on_NextButton_clicked(GTK_BUTTON(NextButton), NULL);
        }

        updating_slider = FALSE;
    }
    return TRUE;
}

void start_timer() {
    if (timer_source == NULL) {
        // Crear un nuevo temporizador que se ejecute cada segundo
        timer_source = g_timeout_source_new_seconds(1);
        g_source_set_callback(timer_source, (GSourceFunc)update_slider, NULL, NULL);
        g_source_attach(timer_source, NULL);
    }
}

void stop_timer() {
    if (timer_source != NULL) {
        // Detener y liberar el temporizador
        g_source_destroy(timer_source);
        g_source_unref(timer_source);
        timer_source = NULL;
    }
}

void playSong(const nodo& song) {
    ma_uint64 startTime = 0;
    const char* filePath = song.file_path.c_str();
    thread songThread(playAudio, filePath, startTime);
    songThread.detach();
    updateSongLabels(song.name, song.artist, song.album, song.genre,
                     song.up_votes, song.down_votes, song.songDuration);
    songDuration = song.songDuration;
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);
}

void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data) {
    g_print("PreviousButton clickeado\n");
    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        stop_timer();
        gtk_range_set_value(GTK_RANGE(TimeSlider), 0);
    }

    nodo* prevSong = nullptr;
    if (!CPisOn)
        prevSong = myPlaylist.getPreviousSong();
    else
        prevSong = randomPlaylist.getPreviousSong();

    if (prevSong != nullptr) {
        playSong(*prevSong);
        isPlaying = true;
        start_timer();
    }
}

void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data) {
    g_print("PlayButton clickeado\n");
    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        gtk_range_set_value(GTK_RANGE(TimeSlider), 0);
        stop_timer();
    }

    nodo* currentSong = nullptr;
    if (!CPisOn)
        currentSong = myPlaylist.getCurrentSong();
    else
        currentSong = randomPlaylist.getCurrentSong();

    const char* filePath = currentSong->file_path.c_str();

    if (currentPosition == 0){
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();
    } else {
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();
        currentPosition = 0;
    }
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                     currentSong->genre, currentSong->up_votes, currentSong->down_votes, currentSong->songDuration);
    songDuration = currentSong->songDuration;
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);
    start_timer();
    isPlaying = true;
}

void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data) {
    g_print("StopButton clickeado\n");
    
    ma_device_stop(&device);

    currentPosition = getCurrentPositionInSeconds(&decoder);

    isPlaying = false;

    stop_timer();

}

void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data) {
    g_print("NextButton clickeado\n");

    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        stop_timer();
        gtk_range_set_value(GTK_RANGE(TimeSlider), 0);
    }

    nodo* nextSong = nullptr;
    if (!CPisOn)
        nextSong = myPlaylist.getNextSong();
    else
        nextSong = randomPlaylist.getNextSong();

    if (nextSong != nullptr) {
        playSong(*nextSong);
        isPlaying = true;
        start_timer();
    }
}

void startServer() {

    string ipAddress = reader.Get("server", "ipAdress", "127.0.0.1");
    int portNum = stoi(reader.Get("server", "port", "8080"));
    int bufsize = stoi(reader.Get("server", "bufsize", "1024"));

    server myServer(ipAddress, portNum, bufsize);
    myServer.start();
}

void createRandomPlayList() {
    int totalSongs = 0;
    nodo* temp = myPlaylist.getCurrentSong();
    if (temp != nullptr) {
        do {
            totalSongs++;
            temp = temp->next;
        } while (temp != myPlaylist.getCurrentSong());
    }

    // Crear un conjunto para almacenar las rutas de archivo ya seleccionadas
    std::set<std::string> selectedFilePaths;

    // Crear la lista aleatoria evitando duplicados
    while (selectedFilePaths.size() < 10 && selectedFilePaths.size() < totalSongs) {
        // Generar un índice aleatorio único
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, totalSongs - 1);
        int randomIndex = dis(gen);

        // Avanzar hasta el nodo correspondiente al índice aleatorio
        temp = myPlaylist.getCurrentSong();
        for (int i = 0; i < randomIndex; ++i) {
            temp = temp->next;
        }

        // Verificar si la ruta de archivo ya ha sido seleccionada
        if (selectedFilePaths.find(temp->file_path) == selectedFilePaths.end()) {
            // Si la ruta de archivo no está en el conjunto, insertarla en la lista aleatoria
            randomPlaylist.insertSong(temp->file_path);
            // Agregar la ruta de archivo al conjunto de rutas seleccionadas
            selectedFilePaths.insert(temp->file_path);
        }
    }
}


void on_CP_clicked(GtkButton *CPButton, gpointer user_data) {
    CPisOn = !CPisOn;
    const char *newLabel = CPisOn ? "ON" : "OFF";
    gtk_button_set_label(CPButton, newLabel);

    if (CPisOn) {
        // Si el botón está encendido
        g_print("CPButton encendido.\n");

        createRandomPlayList();

        nodo* currentSong = randomPlaylist.getCurrentSong();
        updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                         currentSong->genre, currentSong->up_votes, currentSong->down_votes, currentSong->songDuration);

        thread serverThread(startServer);
        serverThread.detach();

    } else {
        // Si el botón está apagado
        g_print("CPButton apagado.\n");

    }

}

void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data) {
    g_print("DeleteButton clickeado\n");

    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        gtk_range_set_value(GTK_RANGE(TimeSlider), 0);
    }

    if (CPisOn) {
        nodo* currentSong = randomPlaylist.getCurrentSong();
        string songId = currentSong->id;
        randomPlaylist.deleteSong(songId);
    } else {
        nodo* currentSong = myPlaylist.getCurrentSong();
        string songId = currentSong->id;
        myPlaylist.deleteSong(songId);
    }

    nodo* currentAfterDel = nullptr;

    if (CPisOn) {
        currentAfterDel = randomPlaylist.getCurrentSong();
        if (currentAfterDel == nullptr) {
            on_CP_clicked(GTK_BUTTON(CPButton), NULL);
        }
        on_PlayButton_clicked(GTK_BUTTON(PlayButton), NULL);
    } else {
        currentAfterDel = myPlaylist.getCurrentSong();
        if (currentAfterDel == nullptr) {
            //LOG(FATAL) << "No hay canciones en la lista" << endl;
        } else {
            on_PlayButton_clicked(GTK_BUTTON(PlayButton), NULL);
        }
    }
}

void on_PaginateButton_toggled(GtkToggleButton *PaginateButton, gpointer user_data) {
    gboolean active = gtk_toggle_button_get_active(PaginateButton);
    if (active) {
        g_print("PaginateButton activado\n");
        // Realizar acciones cuando el botón se activa
    } else {
        g_print("PaginateButton desactivado\n");
        // Realizar acciones cuando el botón se desactiva
    }
}

static void change_slider_value(GtkScale *scale, gdouble value) {
    // Actualizar el valor del slider
    gtk_range_set_value(GTK_RANGE(TimeSlider), value);

    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        stop_timer();
    }

    if (CPisOn) {
        nodo* currentSong = randomPlaylist.getCurrentSong();
        const char* filePath = currentSong->file_path.c_str();
        thread SongThread(playAudio, filePath, value);
        SongThread.detach();
        gtk_range_set_value(GTK_RANGE(TimeSlider), value);
        start_timer();
    } else {
        nodo* currentSong = myPlaylist.getCurrentSong();
        const char* filePath = currentSong->file_path.c_str();
        thread SongThread(playAudio, filePath, value);
        SongThread.detach();
        gtk_range_set_value(GTK_RANGE(TimeSlider), value);
        start_timer();
    }
}

void on_TimeSlider_value_changed(GtkRange *range, gpointer user_data) {
}


void on_VolumeSlider_value_changed(GtkRange *range, gpointer user_data) {
    gdouble value = gtk_range_get_value(range);

    // Convertir el valor del slider al rango aceptado por miniaudio (de 0 a 1)
    float volume = (float)value / 100.0f;

    ma_device_set_master_volume(&device, volume);
}

