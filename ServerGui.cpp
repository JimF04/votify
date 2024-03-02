#include <gtk/gtk.h>

class ServerGUI {
public:
    ServerGUI() {
        gtk_init(NULL, NULL);

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Votify");
        gtk_window_set_default_size(GTK_WINDOW(window), 600, 600);
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        // Cambiar color de la ventana
        GdkRGBA BackgroundColor;
        gdk_rgba_parse(&BackgroundColor, "#f0f2f2"); // Color negro
        gtk_widget_override_background_color(window, GTK_STATE_FLAG_NORMAL, &BackgroundColor);


        // Crear un contenedor
        GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 50);
        gtk_container_add(GTK_CONTAINER(window), vbox);


        // Crear un Label
        VotLabel = gtk_label_new("Votify");

        // Cambiar tamaño del label
        gtk_widget_set_size_request(VotLabel, 100, 50);

        // Cambiar el tamaño de la letra y crear fonts
        PangoFontDescription *VotLabel_desc = pango_font_description_new();
        pango_font_description_set_size(VotLabel_desc, 40 * PANGO_SCALE);
        gtk_widget_override_font(VotLabel, VotLabel_desc);

        // Cambiar color de widgets
        GdkRGBA color;
        gdk_rgba_parse(&color, "#49ada5");
        gtk_widget_override_color(VotLabel, GTK_STATE_FLAG_NORMAL, &color);

        // Agregar label al contenedor
        gtk_box_pack_start(GTK_BOX(vbox), VotLabel, FALSE, FALSE, 10);



        // Crear botones
        ReproductorButton = gtk_button_new_with_label("Reproductor de Música");
        gtk_widget_set_size_request(ReproductorButton, 250, 50);
        gtk_widget_override_color(ReproductorButton, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), ReproductorButton, FALSE, FALSE, 10);

        //Pone las cosas en el centro del contenedor
        gtk_widget_set_halign(ReproductorButton, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(ReproductorButton, GTK_ALIGN_CENTER);

        PlaylistButton = gtk_button_new_with_label("Playlist Comunitaria");
        gtk_widget_set_size_request(PlaylistButton, 250, 50);
        gtk_widget_override_color(PlaylistButton, GTK_STATE_FLAG_NORMAL, &color);
        gtk_box_pack_start(GTK_BOX(vbox), PlaylistButton, FALSE, FALSE, 10);

        gtk_widget_set_halign(PlaylistButton, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(PlaylistButton, GTK_ALIGN_CENTER);

        // Crear fonts de los botones
        PangoFontDescription *Boton_desc = pango_font_description_new();
        pango_font_description_set_size(Boton_desc, 15 * PANGO_SCALE);
        gtk_widget_override_font(ReproductorButton, Boton_desc);
        gtk_widget_override_font(PlaylistButton, Boton_desc);

        // senales de los botones para los action listeners
        g_signal_connect(ReproductorButton, "clicked", G_CALLBACK(onReproductorButtonClicked), this);
        g_signal_connect(PlaylistButton, "clicked", G_CALLBACK(onPlaylistButtonClicked), this);

        gtk_widget_show_all(window);
    }

    void run() {
        gtk_main();
    }

private:
    GtkWidget *window;
    GtkWidget *VotLabel;
    GtkWidget *ReproductorButton;
    GtkWidget *PlaylistButton;

    static void onReproductorButtonClicked(GtkWidget *widget, gpointer data) {
        g_print("Boton de reproduccion \n");
        // Aquí puedes realizar acciones relacionadas con el botón del reproductor de música
    }

    static void onPlaylistButtonClicked(GtkWidget *widget, gpointer data) {
        g_print("Boton de playlist\n");
        // Aquí puedes realizar acciones relacionadas con el botón de la playlist comunitaria
    }
};


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    ServerGUI gui;
    gui.run();
    return 0;
}
