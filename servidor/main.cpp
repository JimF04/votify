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


using namespace std;
namespace fs = std::filesystem;

bool isPlaying = false;

ma_decoder decoder;
ma_device device;
ma_result result;
ma_device_config deviceConfig;

ma_uint64 currentPosition = 0;

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
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

    isPlaying = false;
}



GtkBuilder *builder;
GtkWidget *main_window;
GtkWidget *NameLabel;
GtkWidget *ArtistLabel;
GtkWidget *AlbumLabel;
GtkWidget *GenreLabel;

GtkWidget *PreviousButton;
GtkWidget *PlayButton;
GtkWidget *StopButton;
GtkWidget *NextButton;
GtkWidget *CPOnButton;
GtkWidget *CPOffButton;
GtkWidget *PaginateButton;
GtkWidget *DeleteButton;



void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data);
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data);
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data);
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data);
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data);
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data);
void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data);
void on_PaginateButton_toggled(GtkToggleButton *button, gpointer user_data);

int main(int argc, char *argv[]) {

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

//    string ipAddress = "127.0.0.1";
//    int portNum = 50000;
//    int bufsize = 1024;
//
//    server server(ipAddress, portNum, bufsize);
//    server.start();

    gtk_init(&argc, &argv);

    // Carga la interfaz desde el archivo Glade
    builder = gtk_builder_new_from_file("/home/jimmy/Documents/GitHub/votify/servidor/GUI Server.glade");

    // Obtiene los widgets necesarios de la interfaz
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));


    NameLabel = GTK_WIDGET(gtk_builder_get_object(builder, "NameLabel"));
    ArtistLabel = GTK_WIDGET(gtk_builder_get_object(builder, "ArtistLabel"));
    AlbumLabel = GTK_WIDGET(gtk_builder_get_object(builder, "AlbumLabel"));
    GenreLabel = GTK_WIDGET(gtk_builder_get_object(builder, "GenreLabel"));


    // Conecta las señales de los widgets a las funciones callback
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

    // Libera el builder
    g_object_unref(builder);

    // Muestra la ventana principal
    gtk_widget_show_all(main_window);

    // Ejecuta el bucle principal de eventos
    gtk_main();

    return EXIT_SUCCESS;
}

void updateSongLabels(const string& songName, const string& artistName, const string& albumName, const string& genreName )  {
    gtk_label_set_text(GTK_LABEL(NameLabel), ("Song: " + songName).c_str());
    gtk_label_set_text(GTK_LABEL(ArtistLabel), ("Artist: " + artistName).c_str());
    gtk_label_set_text(GTK_LABEL(AlbumLabel), ("Album: " + albumName).c_str());
    gtk_label_set_text(GTK_LABEL(GenreLabel), ("Genre: " + genreName).c_str());
}

// Definición de funciones callback
// Implementación de la función de controlador para el botón PreviousButton
void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data) {
    g_print("PreviousButton clickeado\n");
    if (isPlaying) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        isPlaying = false;
    }

    nodo* previousSong = getPreviousSong();

    const char* filePath = previousSong->file_path.c_str();

    ma_uint64 startTime = 0;

    thread songThread(playAudio, filePath, startTime);

    songThread.detach();

    isPlaying = true;
    updateSongLabels(previousSong->name, previousSong->artist, previousSong->album, previousSong->genre);
}

// Implementación de la función de controlador para el botón PlayButton
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data) {
    g_print("PlayButton clickeado\n");
    if (isPlaying) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        isPlaying = false;
    }

    nodo* currentSong = getCurrentSong();

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
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album, currentSong->genre);

}

// Implementación de la función de controlador para el botón StopButton
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data) {
    g_print("StopButton clickeado\n");
    ma_device_uninit(&device);
    ma_decoder_uninit(&decoder);

    currentPosition = getCurrentPositionInSeconds(&decoder);

    isPlaying = false;

}


// Implementación de la función de controlador para el botón NextButton
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data) {
    g_print("NextButton clickeado\n");
    if (isPlaying) {
        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);
        isPlaying = false;
    }

    nodo* nextSong = getNextSong();

    const char* filePath = nextSong->file_path.c_str();

    ma_uint64 startTime = 0;

    thread songThread(playAudio, filePath, startTime);

    songThread.detach();

    isPlaying = true;
    updateSongLabels(nextSong->name, nextSong->artist, nextSong->album, nextSong->genre);
}

// Implementación de la función de controlador para el botón CPOnButton
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data) {
    g_print("CPOnButton clickeado\n");
}

// Implementación de la función de controlador para el botón CPOffButton
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data) {
    g_print("CPOffButton clickeado\n");
}

void on_DeleteButton_clicked(GtkButton *DeleteButton, gpointer user_data) {
    g_print("DeleteButton clickeado\n");
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


