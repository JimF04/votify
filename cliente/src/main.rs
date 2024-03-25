mod cliente_Server;

use std::fs::File;
use gtk::gdk::keys::constants::{b, CD, lacute};
use gtk::prelude::*;
use gtk::{Application, ApplicationWindow, Button, Label, Box, pango};
use std::sync::{Arc, Mutex};
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::str::{from_utf8, FromStr};
use std::thread;
use async_std::task;
use gio::FileAttributeType::String;
use std::time::Duration;
use fern::Dispatch;
use gtk::pango::ffi::PangoFontDescription;
use log::{info, warn, error, debug, trace};

use serde_json;
use serde_json::{json, Value};
use gtk::ScrolledWindow;

use gtk::gdk::RGBA;
use gtk::glib::bitflags::Flags;
use gtk::prelude::*;
use gtk::traits::*;

fn main() {

    let file = File::create("cliente.log").expect("Failed to create log file");
    let logger_config = Dispatch::new()
        .format(|out, message, record| {
            out.finish(format_args!("[{}] {}", record.level(), message))
        })
        .chain(file);

    // Configurar el logger global para que use fern
    logger_config.apply().expect("Failed to initialize logger");



    let mut songs = vec![
        json!({"nombre": "Baby chino", "cantidad_de_votos": 0}),
        json!({"nombre": "superidol", "cantidad_de_votos": 0}),
        json!({"nombre": "mae nose otra", "cantidad_de_votos": 0}),
        json!({"nombre": "Baby chino", "cantidad_de_votos": 0}),
        json!({"nombre": "superidol", "cantidad_de_votos": 0}),
        json!({"nombre": "mae nose otra", "cantidad_de_votos": 0}),
        json!({"nombre": "Baby chino", "cantidad_de_votos": 0}),
        json!({"nombre": "superidol", "cantidad_de_votos": 0}),
        json!({"nombre": "mae nose otra", "cantidad_de_votos": 0})
    ];




    // Utiliza unwrap_or_else para manejar errores
    let json_string = serde_json::to_string_pretty(&songs)
        .unwrap_or_else(|e| panic!("Error al serializar a JSON: {}", e));



    let application = Application::builder()
        .application_id("com.example.FirstGtkApp")
        .build();

    application.connect_activate(move |app| {
        use gtk::prelude::*;
        use gtk::traits::*;


        let window = ApplicationWindow::builder()
            .application(app)
            .title("Votify")
            .default_width(400)
            .default_height(400)

            .build();



        let container = Box::new(gtk::Orientation::Vertical, 0);
        container.set_spacing(10);


        let scrolled_window = ScrolledWindow::builder()
            .hscrollbar_policy(gtk::PolicyType::Never) // Opcional: desactivar la barra de desplazamiento horizontal
            .min_content_width(300) // Opcional: establecer un ancho mínimo
            .build();

        let songs_box = Box::new(gtk::Orientation::Vertical, 0);
        songs_box.set_spacing(5);


        let frase = Label::new(Some("LET VOTE!!"));
       container.pack_start(&frase, false, false, 15);

        crear_songs(&window, &songs_box, &songs);

        scrolled_window.add(&songs_box);

        container.pack_start(&scrolled_window, true, true, 0);
        window.add(&container);
        window.show_all();
    });

    application.run();
}






fn crear_songs(ventana: &ApplicationWindow, caja: &Box, songs: &Vec<Value>) {

    for song in songs {
        if let Some(nombre) = song.get("nombre").and_then(|n| n.as_str()) {
            let label_text = nombre.to_string(); // Convertimos el &str a String



            let filas = Box::new(gtk::Orientation::Horizontal, 0);

            let label = Label::new(Some(&label_text));
            label.font_map();
            label.set_halign(gtk::Align::Start);
            label.set_size_request(200, 50);





            let like = Button::with_label("😎");

            like.set_size_request(70, 70);


            let dislike = Button::with_label("🤮");
            dislike.set_size_request(70, 70);








            let label_text_like = label_text.clone();
            dislike.connect_clicked(move |_| {
                commando_bajar(&label_text_like);
            });


            let label_text_dislike = label_text.clone();
            // Manejar clics en el botón "like"
            like.connect_clicked(move |_| {
                commando_subida(&label_text_dislike);
            });


            // Agregar iconos a los botones


            filas.pack_start(&label, false, false, 0);
            filas.pack_start(&like, false, false, 0);
            filas.pack_start(&dislike, false, false, 10);
            caja.pack_start(&filas, false, false, 10);
        }
    }
}



fn commando_subida(nombre_cancion: &str){
    let mensaje = format!("vote-up: {}",nombre_cancion);
    println!("vote-up: {}", nombre_cancion);
    abrir_socket_mensaje(&mensaje);

}


fn commando_bajar(nombre_cancion: &str){
    let mensaje = format!("vote-down: {}",nombre_cancion);
    println!("vote-down: {}", nombre_cancion);
    abrir_socket_mensaje(&mensaje);


}

fn abrir_socket_mensaje(mensaje: &str){


    match TcpStream::connect("127.0.0.1:50000") {
        Ok(mut stream) => {
            info!("Successfully connected to server in port 50000");

            let msg = mensaje;
            stream.write(msg.as_ref()).unwrap();
            info!("Sent list, awaiting reply...");
            let mut data = [0 as u8; 6]; // using 6 byte buffer

        }
        Err(e) => {
            warn!("Failed to connect: {}", e);
        }
    }
    info!("Terminated.");

}