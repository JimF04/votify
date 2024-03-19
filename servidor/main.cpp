#include <iostream>
#include <filesystem>
#include "playlist.h"
#include <cstdlib>
#include <iostream>
#include <gtk/gtk.h>

using namespace std;
namespace fs = std::filesystem;

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

void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data);
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data);
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data);
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data);
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data);
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data);
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
    // Inicializa GTK
    gtk_init(&argc, &argv);

    // Carga la interfaz desde el archivo Glade
    builder = gtk_builder_new_from_file("/home/ahenao/Proyecto Playlist Comunitaria/votify/servidor/GUI Server.glade");

    // Obtiene los widgets necesarios de la interfaz
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));


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

// Definición de funciones callback
// Implementación de la función de controlador para el botón PreviousButton
void on_PreviousButton_clicked(GtkButton *PreviousButton, gpointer user_data) {
    g_print("PreviousButton clickeado\n");
}

// Implementación de la función de controlador para el botón PlayButton
void on_PlayButton_clicked(GtkButton *PlayButton, gpointer user_data) {
    g_print("PlayButton clickeado\n");
}

// Implementación de la función de controlador para el botón StopButton
void on_StopButton_clicked(GtkButton *StopButton, gpointer user_data) {
    g_print("StopButton clickeado\n");
}

// Implementación de la función de controlador para el botón NextButton
void on_NextButton_clicked(GtkButton *NextButton, gpointer user_data) {
    g_print("NextButton clickeado\n");
}

// Implementación de la función de controlador para el botón CPOnButton
void on_CPOn_clicked(GtkButton *CPOnButton, gpointer user_data) {
    g_print("CPOnButton clickeado\n");
}

// Implementación de la función de controlador para el botón CPOffButton
void on_CPOffButton_clicked(GtkButton *CPOffButton, gpointer user_data) {
    g_print("CPOffButton clickeado\n");
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