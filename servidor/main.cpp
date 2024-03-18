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

class PCGUI {
public:
    PCGUI(GtkWidget *parent_window) : parent_window(parent_window) {
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Playlist Comunitaria");
        gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
        gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
        g_signal_connect(window, "destroy", G_CALLBACK(on_window_closed), this);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        backButton = gtk_button_new_with_label("Regresar");
        g_signal_connect(backButton, "clicked", G_CALLBACK(on_back_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(vbox), backButton, FALSE, FALSE, 5);

        musicPlayerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_pack_start(GTK_BOX(vbox), musicPlayerBox, FALSE, FALSE, 5);

        songLabel = gtk_label_new("Canción: ");
        gtk_box_pack_start(GTK_BOX(vbox), songLabel, FALSE, FALSE, 5);

        artistLabel = gtk_label_new("Artista: ");
        gtk_box_pack_start(GTK_BOX(vbox), artistLabel, FALSE, FALSE, 5);

        durationLabel = gtk_label_new("Tiempo de la canción: ");
        gtk_box_pack_start(GTK_BOX(vbox), durationLabel, FALSE, FALSE, 5);

        GtkWidget *spacer = gtk_label_new("");
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), spacer, TRUE, TRUE, 0);

        previousButton = gtk_button_new_with_label("Previous");
        g_signal_connect(previousButton, "clicked", G_CALLBACK(on_previous_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), previousButton, FALSE, FALSE, 5);

        playButton = gtk_button_new_with_label("Play");
        g_signal_connect(playButton, "clicked", G_CALLBACK(on_play_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), playButton, FALSE, FALSE, 5);

        stopButton = gtk_button_new_with_label("Stop");
        g_signal_connect(stopButton, "clicked", G_CALLBACK(on_stop_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), stopButton, FALSE, FALSE, 5);

        nextButton = gtk_button_new_with_label("Next");
        g_signal_connect(nextButton, "clicked", G_CALLBACK(on_next_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), nextButton, FALSE, FALSE, 5);

        GtkWidget *spacer2 = gtk_label_new("");
        gtk_box_pack_end(GTK_BOX(musicPlayerBox), spacer2, TRUE, TRUE, 0);

        timeSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 250, 1);
        gtk_range_set_value(GTK_RANGE(timeSlider), 0);
        gtk_box_pack_start(GTK_BOX(vbox), timeSlider, FALSE, FALSE, 5);

        pageButton = gtk_toggle_button_new_with_label("Modo paginado");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pageButton), FALSE);
        g_signal_connect(pageButton, "toggled", G_CALLBACK(on_page_button_toggled), this);
        gtk_box_pack_start(GTK_BOX(musicPlayerBox), pageButton, FALSE, FALSE, 5);

        gtk_widget_show_all(window);
    }

    void show() {
        gtk_widget_show_all(window);
    }

    void hide() {
        gtk_widget_hide(window);
    }

private:
    GtkWidget *window;
    GtkWidget *backButton;
    GtkWidget *parent_window;
    GtkWidget *musicPlayerBox;
    GtkWidget *playButton;
    GtkWidget *stopButton;
    GtkWidget *previousButton;
    GtkWidget *nextButton;
    GtkWidget *timeSlider;
    GtkWidget *pageButton;
    GtkWidget *songLabel;
    GtkWidget *artistLabel;
    GtkWidget *durationLabel;

    static void on_window_closed(GtkWidget *widget, gpointer data) {
        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->hide();
        gtk_widget_show_all(pc_gui->parent_window);
    }

    static void on_back_button_clicked(GtkWidget *widget, gpointer data) {
        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        gtk_widget_hide(pc_gui->window);
        gtk_widget_show(pc_gui->parent_window);
    }

    static void on_previous_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Previous\n");

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

        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->updateSongLabels(previousSong->name, previousSong->artist);
    }

    static void on_play_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Play\n");

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

        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->updateSongLabels(currentSong->name, currentSong->artist);
    }

    static void on_stop_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Stop\n");

        ma_device_uninit(&device);
        ma_decoder_uninit(&decoder);

        currentPosition = getCurrentPositionInSeconds(&decoder);

        isPlaying = false;

    }

    static void on_next_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Next\n");

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

        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->updateSongLabels(nextSong->name, nextSong->artist);
    }

    static void on_page_button_toggled(GtkWidget *widget, gpointer data) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget))) {
            g_print("Modo paginado activado\n");
        } else {
            g_print("Modo paginado desactivado\n");
        }
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


    void updateSongLabels(const string& songName, const string& artistName) {
        gtk_label_set_text(GTK_LABEL(songLabel), ("Canción: " + songName).c_str());
        gtk_label_set_text(GTK_LABEL(artistLabel), ("Artista: " + artistName).c_str());
    }
};


class ServerGUI {
public:
    ServerGUI() {
        gtk_init(NULL, NULL);

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Votify");
        gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

        GdkRGBA BackgroundColor;
        gdk_rgba_parse(&BackgroundColor, "#f0f2f2");
        gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &BackgroundColor);

        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        VotLabel = gtk_label_new("Votify");
        gtk_widget_set_size_request(VotLabel, 100, 50);
        PangoFontDescription *VotLabel_desc = pango_font_description_new();
        pango_font_description_set_size(VotLabel_desc, 40 * PANGO_SCALE);
        gtk_widget_override_font(VotLabel, VotLabel_desc);
        GdkRGBA color;
        gdk_rgba_parse(&color, "#49ada5");
        gtk_widget_override_color(VotLabel, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), VotLabel, FALSE, FALSE, 10);

        ReproductorButton = gtk_button_new_with_label("Reproductor de Música");
        gtk_widget_set_size_request(ReproductorButton, 250, 50);
        gtk_widget_override_color(ReproductorButton, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), ReproductorButton, FALSE, FALSE, 10);
        gtk_widget_set_halign(ReproductorButton, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(ReproductorButton, GTK_ALIGN_CENTER);

        PlaylistButton = gtk_button_new_with_label("Playlist Comunitaria");
        gtk_widget_set_size_request(PlaylistButton, 250, 50);
        gtk_widget_override_color(PlaylistButton, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), PlaylistButton, FALSE, FALSE, 10);
        gtk_widget_set_halign(PlaylistButton, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(PlaylistButton, GTK_ALIGN_CENTER);

        PangoFontDescription *Boton_desc = pango_font_description_new();
        pango_font_description_set_size(Boton_desc, 15 * PANGO_SCALE);
        gtk_widget_override_font(ReproductorButton, Boton_desc);
        gtk_widget_override_font(PlaylistButton, Boton_desc);

        g_signal_connect(ReproductorButton, "clicked", G_CALLBACK(onReproductorButtonClicked), this);
        g_signal_connect(PlaylistButton, "clicked", G_CALLBACK(onPlaylistButtonClicked), this);

        gtk_widget_show_all(window);

        pc_gui = nullptr;
    }

    ~ServerGUI() {
        delete pc_gui;
    }

    void run() {
        gtk_main();
    }

private:
    GtkWidget *window;
    GtkWidget *VotLabel;
    GtkWidget *ReproductorButton;
    GtkWidget *PlaylistButton;
    PCGUI *pc_gui;

    static void onReproductorButtonClicked(GtkWidget *widget, gpointer data) {
        g_print("Boton de reproduccion \n");
    }

    static void onPlaylistButtonClicked(GtkWidget *widget, gpointer data) {
        g_print("Boton de playlist\n");
        ServerGUI *server_gui = static_cast<ServerGUI *>(data);
        gtk_widget_hide(server_gui->window);
        if (!server_gui->pc_gui) {
            server_gui->pc_gui = new PCGUI(server_gui->window);
        }
        server_gui->pc_gui->show();
    }
};


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
    ServerGUI gui;
    gui.run();

    return 0;
}
