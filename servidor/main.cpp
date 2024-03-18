#include <iostream>
#include <filesystem>
#include "playlist.h"
#include <gtk/gtk.h>
#include <thread>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <unistd.h>

using namespace std;
namespace fs = std::filesystem;

bool isPlaying = false;

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

        nodo* previousSong = getPreviousSong();

        thread songThread(playSong, previousSong->file_path);

        songThread.detach();

        isPlaying = true;

        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->updateSongLabels(previousSong->name, previousSong->artist);
    }

    static void on_play_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Play\n");

        nodo* currentSong = getCurrentSong();

        thread songThread(playSong, currentSong->file_path);

        songThread.detach();

        isPlaying = true;

        PCGUI *pc_gui = static_cast<PCGUI *>(data);
        pc_gui->updateSongLabels(currentSong->name, currentSong->artist);
    }

    static void on_stop_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Stop\n");

        system("killall -9 mpg123");
    }

    static void on_next_button_clicked(GtkWidget *widget, gpointer data) {
        g_print("Next\n");

        nodo* nextSong = getNextSong();

        thread songThread(playSong, nextSong->file_path);

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

    static void playSong(const string& filePath) {

        if (isPlaying) {
            system("killall -9 mpg123");
        }

        string command = "mpg123 " + filePath;

        int result = std::system(command.c_str());

        if (result != 0) {
            cerr << "Error reproduciendo la canción" << std::endl;
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
    ServerGUI() : server_pid_(getpid()) {
        std::cout << "PID" << server_pid_ << std::endl;
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

        RamUsageLabel = gtk_label_new("RAM Usage: Calculating...");
        gtk_widget_set_size_request(RamUsageLabel, 250, 50);
        gtk_widget_override_color(RamUsageLabel, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), RamUsageLabel, FALSE, FALSE, 10);

        PangoFontDescription *Boton_desc = pango_font_description_new();
        pango_font_description_set_size(Boton_desc, 15 * PANGO_SCALE);
        gtk_widget_override_font(ReproductorButton, Boton_desc);
        gtk_widget_override_font(PlaylistButton, Boton_desc);

        g_signal_connect(ReproductorButton, "clicked", G_CALLBACK(onReproductorButtonClicked), this);
        g_signal_connect(PlaylistButton, "clicked", G_CALLBACK(onPlaylistButtonClicked), this);

        gtk_widget_show_all(window);

        pc_gui = nullptr;

        g_timeout_add_seconds(1, (GSourceFunc)updateRamUsage, this);
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
    GtkWidget *RamUsageLabel;
    pid_t server_pid_;
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

    static gboolean updateRamUsage(gpointer data) {
        ServerGUI *server_gui = static_cast<ServerGUI *>(data);
        double ramUsage = getServerRamUsage(server_gui->server_pid_);
        std::ostringstream ramUsageStr;
        ramUsageStr << "RAM Usage: " << std::fixed << std::setprecision(2) << ramUsage << " MB";
        gtk_label_set_text(GTK_LABEL(server_gui->RamUsageLabel), ramUsageStr.str().c_str());
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

    gtk_init(&argc, &argv);
    ServerGUI gui;
    gui.run();

    return 0;
}
