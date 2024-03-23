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
#include <glog/logging.h>
#include "INIReader.h"

using namespace std;
namespace fs_std = std::filesystem;

Playlist myPlaylist;

bool isPlaying = false;

ma_decoder decoder;
ma_device device;
ma_result result;
ma_device_config deviceConfig;

ma_uint64 currentPosition = 0;
int songDuration;

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

GtkWidget *PreviousButton;
GtkWidget *PlayButton;
GtkWidget *StopButton;
GtkWidget *NextButton;
GtkWidget *CPOnButton;
GtkWidget *CPOffButton;
GtkWidget *PaginateButton;
GtkWidget *DeleteButton;
GtkWidget *TimeSlider;
GtkWidget *VolumeSlider;

GtkWidget *artistBox;


typedef struct {
    GtkWidget *button;
    char *artistId;
} ArtistButtonData;


GtkWidget *songArtistBox;


void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data);
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data);
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data);
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data);
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data);
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data);
void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data);
void on_PaginateButton_toggled(GtkToggleButton *button, gpointer user_data);
void on_TimeSlider_value_changed(GtkRange *range, gpointer user_data);
void on_VolumeSlider_value_changed(GtkRange *range, gpointer user_data);
static void change_slider_value(GtkScale *scale, gdouble value);
void on_artist_button_clicked(GtkButton *button, gpointer user_data);

int main(int argc, char *argv[]) {

    google::InitGoogleLogging(argv[0]);

    google::SetLogDestination(google::GLOG_INFO, "server.log");
    google::SetLogDestination(google::GLOG_ERROR, "server.log");
    google::SetLogDestination(google::GLOG_WARNING, "server.log");
    google::SetLogDestination(google::GLOG_FATAL, "server.log");

    string ini_path =  "/home/" + string(getenv("USER")) + "/Documents/GitHub/votify/servidor/config.ini";
    LOG(INFO) << "Ruta de config.ini: " << ini_path << endl;

    INIReader reader(ini_path);

    string songs_path = reader.Get("paths", "songs_path", "");

    // Iterar sobre los archivos dentro del directorio
    for (const auto& entry : fs_std::directory_iterator(songs_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".mp3") {
            // Llamar a insert_songs() con la ruta del archivo
            myPlaylist.insertSong(entry.path().string());
        } else {
            LOG(WARNING) << "El archivo " << entry.path().string() << " no es un archivo de audio" << endl;
        }
    }

    cout << "\n";

    string jsonPath = reader.Get("paths", "json_path", "");
    myPlaylist.saveToJson(jsonPath);

//    string ipAddress = "127.0.0.1";
//    int portNum = 50000;
//    int bufsize = 1024;
//
//    server server(ipAddress, portNum, bufsize);
//    server.start();

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

    TimeSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 1);
    g_signal_connect(TimeSlider, "value-changed", G_CALLBACK(on_TimeSlider_value_changed), NULL);
    g_signal_connect(TimeSlider, "change-value", G_CALLBACK(change_slider_value), NULL);
    gtk_widget_set_size_request(TimeSlider, 400, -1); // Establece un tamaño fijo para el slider
    gtk_grid_attach(GTK_GRID(TimeGrid), TimeSlider, 1, 0, 1, 1);

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

    CPOnButton = GTK_WIDGET(gtk_builder_get_object(builder, "CPOnButton"));
    g_signal_connect(CPOnButton, "clicked", G_CALLBACK(on_CPOn_clicked), NULL);

    CPOffButton = GTK_WIDGET(gtk_builder_get_object(builder, "CPOffButton"));
    g_signal_connect(CPOffButton, "clicked", G_CALLBACK(on_CPOffButton_clicked), NULL);

    DeleteButton = GTK_WIDGET(gtk_builder_get_object(builder, "DeleteButton"));
    g_signal_connect(DeleteButton, "clicked", G_CALLBACK(on_DeleteButton_clicked), NULL);

    PaginateButton = GTK_WIDGET(gtk_builder_get_object(builder, "PaginateButton"));
    g_signal_connect(PaginateButton, "toggled", G_CALLBACK(on_PaginateButton_toggled), NULL);

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

    // Crear labels para cada cancion del artista
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

void updateSongLabels(const string& songName, const string& artistName, const string& albumName, const string& genreName, int upVotes, int downVotes)  {
    gtk_label_set_text(GTK_LABEL(NameLabel), ("Song: " + songName).c_str());
    gtk_label_set_text(GTK_LABEL(ArtistLabel), ("Artist: " + artistName).c_str());
    gtk_label_set_text(GTK_LABEL(AlbumLabel), ("Album: " + albumName).c_str());
    gtk_label_set_text(GTK_LABEL(GenreLabel), ("Genre: " + genreName).c_str());
    gtk_label_set_text(GTK_LABEL(UpVotesLabel), ("Up Votes: " + to_string(upVotes)).c_str());
    gtk_label_set_text(GTK_LABEL(DownVotesLabel), ("Down Votes: " + to_string(downVotes)).c_str());
}

GSource *timer_source = NULL;
gboolean updating_slider = FALSE;

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

void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data) {
    g_print("PreviousButton clickeado\n");
    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        stop_timer();
    }

    gtk_range_set_value(GTK_RANGE(TimeSlider), 0);

    nodo* prevSong = myPlaylist.getPreviousSong();

    const char* filePath = prevSong->file_path.c_str();

    ma_uint64 startTime = 0;

    thread songThread(playAudio, filePath, startTime);

    songThread.detach();

    isPlaying = true;
    updateSongLabels(prevSong->name, prevSong->artist, prevSong->album,
                     prevSong->genre, prevSong->up_votes, prevSong->down_votes);

    songDuration = prevSong->songDuration;

    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
}

void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data) {
    g_print("PlayButton clickeado\n");
    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
        gtk_range_set_value(GTK_RANGE(TimeSlider), 0);
        stop_timer();
    }

    nodo* currentSong = myPlaylist.getCurrentSong();

    const char* filePath = currentSong->file_path.c_str();

    if (currentPosition == 0){
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();
    } else {
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();
        currentPosition = 0;
    }

    isPlaying = true;
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album,
                     currentSong->genre, currentSong->up_votes, currentSong->down_votes);

    songDuration = currentSong->songDuration;

    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
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
    }

    gtk_range_set_value(GTK_RANGE(TimeSlider), 0);

    nodo* nextSong = myPlaylist.getNextSong();

    const char* filePath = nextSong->file_path.c_str();

    ma_uint64 startTime = 0;

    thread songThread(playAudio, filePath, startTime);

    songThread.detach();

    isPlaying = true;

    updateSongLabels(nextSong->name, nextSong->artist, nextSong->album,
                     nextSong->genre, nextSong->up_votes, nextSong->down_votes);

    songDuration = nextSong->songDuration;

    g_print("Duración de la canción: %lu\n", songDuration);
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
}


void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data) {
    g_print("CPOnButton clickeado\n");
}


void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data) {
    g_print("CPOffButton clickeado\n");
}

void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data) {
    g_print("DeleteButton clickeado\n");

    if (isPlaying) {
        ma_device_stop(&device);
        isPlaying = false;
    }

    gtk_range_set_value(GTK_RANGE(TimeSlider), 0);

    nodo* currentSong = myPlaylist.getCurrentSong();
    string songId = currentSong->id;
    myPlaylist.deleteSong(songId);

    nodo* nextSong = myPlaylist.getNextSong();

    // Verifica si hay una próxima canción
    if (nextSong != nullptr) {
        const char* filePath = nextSong->file_path.c_str();

        // Reproducir la próxima canción
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();

        updateSongLabels(nextSong->name, nextSong->artist, nextSong->album,
                         nextSong->genre, nextSong->up_votes, nextSong->down_votes);

        start_timer();
    } else {
        // Si no hay una próxima canción, muestra un mensaje indicando que la lista está vacía
        LOG(FATAL) << "La lista de reproducción está vacía." << endl;
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

    nodo* currentSong = myPlaylist.getCurrentSong();

    const char* filePath = currentSong->file_path.c_str();

    thread SongThread(playAudio, filePath, value);
    SongThread.detach();

    gtk_range_set_value(GTK_RANGE(TimeSlider), value);

    start_timer();
}

void on_TimeSlider_value_changed(GtkRange *range, gpointer user_data) {
}


void on_VolumeSlider_value_changed(GtkRange *range, gpointer user_data) {
    gdouble value = gtk_range_get_value(range);

    // Convertir el valor del slider al rango aceptado por miniaudio (de 0 a 1)
    float volume = (float)value / 100.0f;

    ma_device_set_master_volume(&device, volume);
}

