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

    myPlaylist.display();

//    string jsonPath = reader.Get("paths", "json_path", "");
//
//    myPlaylist.saveToJson(jsonPath);

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

GSource *timer_source = NULL;

gboolean updating_slider = FALSE;

// Función para actualizar el slider
static gboolean update_slider(gpointer user_data) {
    if (!updating_slider) {
        updating_slider = TRUE;

        // Incrementar el valor actual del slider por 1 segundo
        double value = gtk_range_get_value(GTK_RANGE(TimeSlider));
        value += 1.0;

        // Actualizar el valor del slider
        gtk_range_set_value(GTK_RANGE(TimeSlider), value);

        nodo* currentSong = myPlaylist.getCurrentSong();
        if (value >= currentSong->songDuration) {
            on_NextButton_clicked(GTK_BUTTON(NextButton), NULL);
        }

        updating_slider = FALSE;
    }

    // Retornar TRUE para que el temporizador se mantenga activo
    return TRUE;
}

// Función para iniciar el temporizador
void start_timer() {
    if (timer_source == NULL) {
        // Crear un nuevo temporizador que se ejecute cada segundo
        timer_source = g_timeout_source_new_seconds(1);

        // Conectar la señal para actualizar el slider
        g_source_set_callback(timer_source, (GSourceFunc)update_slider, NULL, NULL);

        // Agregar el temporizador al contexto principal de la aplicación
        g_source_attach(timer_source, NULL);
    }
}

// Función para detener el temporizador
void stop_timer() {
    if (timer_source != NULL) {
        // Detener y liberar el temporizador
        g_source_destroy(timer_source);
        g_source_unref(timer_source);
        timer_source = NULL;
    }
}


// Definición de funciones callback
// Implementación de la función de controlador para el botón PreviousButton
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
    updateSongLabels(prevSong->name, prevSong->artist, prevSong->album, prevSong->genre);

    songDuration = prevSong->songDuration;

    g_print("Duración de la canción: %lu\n", songDuration);
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
}

// Implementación de la función de controlador para el botón PlayButton
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
    updateSongLabels(currentSong->name, currentSong->artist, currentSong->album, currentSong->genre);

    songDuration = currentSong->songDuration;

    g_print("Duración de la canción: %lu\n", songDuration);
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
}

// Implementación de la función de controlador para el botón StopButton
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data) {
    g_print("StopButton clickeado\n");
    
    ma_device_stop(&device);

    currentPosition = getCurrentPositionInSeconds(&decoder);

    isPlaying = false;

    stop_timer();

}


// Implementación de la función de controlador para el botón NextButton
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
    updateSongLabels(nextSong->name, nextSong->artist, nextSong->album, nextSong->genre);

    songDuration = nextSong->songDuration;

    g_print("Duración de la canción: %lu\n", songDuration);
    gtk_range_set_range(GTK_RANGE(TimeSlider), 0, songDuration);

    start_timer();
}

// Implementación de la función de controlador para el botón CPOnButton
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data) {
    g_print("CPOnButton clickeado\n");
}

// Implementación de la función de controlador para el botón CPOffButton
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data) {
    g_print("CPOffButton clickeado\n");
}

// Implementación de la función de controlador para el botón DeleteButton
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

        // Reproduce la próxima canción en un hilo separado
        thread songThread(playAudio, filePath, currentPosition);
        songThread.detach();

        // Actualiza las etiquetas de la canción
        updateSongLabels(nextSong->name, nextSong->artist, nextSong->album, nextSong->genre);

        // Muestra la lista de reproducción
        myPlaylist.display();

        start_timer();
    } else {
        // Si no hay una próxima canción, muestra un mensaje indicando que la lista está vacía
        cout << "La lista de reproducción está vacía." << endl;
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
    // g_print("Slider value: %f\n", value);

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
    g_print("Volume: %f\n", value);

    // Convertir el valor del slider al rango aceptado por miniaudio (de 0 a 1)
    float volume = (float)value / 100.0f;

    // Setear el volumen utilizando miniaudio (la manera específica depende de cómo estés utilizando miniaudio en tu programa)
    // Supongamos que tienes una instancia de `ma_device` llamada `device`:
    ma_device_set_master_volume(&device, volume);
}

