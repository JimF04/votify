mod cliente_Server;

use std::fs::File;
use gtk::gdk::keys::constants::{b, CD, lacute};
use gtk::prelude::*;
use gtk::{Application, ApplicationWindow, Button, Label, Box, pango};
use std::sync::{Arc, Mutex};
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::process::id;
use std::str::{from_utf8, FromStr};
use std::thread;
use async_std::task;
use gio::FileAttributeType::String;
use std::time::Duration;
use fern::Dispatch;
use gio::ffi::g_dbus_property_info_ref;
use gtk::pango::ffi::PangoFontDescription;
use log::{info, warn, error, debug, trace};

use serde_json;
use serde_json::{json, Value};
use gtk::ScrolledWindow;

use gtk::gdk::RGBA;
use gtk::glib::bitflags::Flags;
use gtk::prelude::*;
use gtk::traits::*;
use serde::{Serialize, Deserialize};





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
        json!({"id": "6fff1d7b-61cc-4f5c-b853-6c963e6b4ba9", "name": "Without the Communist Party"}),
        json!({"id": "0ee42edc-e0bc-4ed1-abde-69467f499b06", "name": "Eine Kleine"}),
        json!({"id": "10b53a87-ef9e-4305-b54c-2748a254a2dd", "name": "海阔天空"}),
        json!({"id": "ec772044-84ec-4af0-87db-ebac21d07f02", "name": "Red Sun in the Sky"}),
        json!({"id": "0153f356-0ba2-4585-bac7-feeaa81c2bc2", "name": "Ye Hua Xiang"}),
        json!({"id": "58e488d9-b8ac-41fe-8fb7-b107da6e6642", "name": "Likey"}),
        json!({"id": "ee16a253-2584-42f2-a840-74c447c69510", "name": "歌词版"}),
        json!({"id": "8150ee7a-bb7d-42b5-a4c6-d04e6541cd02", "name": "Orion"}),
        json!({"id": "68ffdffa-951d-4596-9f9a-f40afcb388d9", "name": "Xue Hua Piao",}),
        json!({"id": "7b2b8299-efd0-4416-bccc-d82a960ecbef", "name": "Super Idol"}),
        json!({"id": "e7ee9a48-24c6-4a66-b7ac-c5216a7c3262", "name": "Lemon"}),
        json!({"id": "1f9bcdff-63f0-4385-af1c-71ea60a501d4", "name": "Yoru ni Kakeru"}),
        json!({"id": "2a4b0a20-efca-4a90-abaf-ee40883fac06", "name": "Tong Hua",}),
        json!({"id": "a8249a04-ea82-4b6b-9611-92802479407e", "name": "アイドル"}),
        json!({"id": "60684364-c020-465a-bdaa-94a3ea400fec", "name":"Fancy"}),
        json!({"id": "9cc7ac02-c126-4e42-96cb-70bd4ccd4077", "name": "Yes or Yes"}),
        json!({"id": "fdf0aff2-5c28-4e9c-b871-1c88f6bd191d", "name": "Flamingo"}),
        json!({"id": "4be38bb8-453c-48c4-9a6a-bd7a2cfa7a9c", "name": "ラフレター"}),
        json!({"id": "c574f860-ce7d-478f-ba3f-bf0a1750e23d", "name": "Cry for me"}),
        json!({"id": "5b905406-32c1-4689-acaa-a209fc4313eb", "name": "Baby Chinese"})
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
        if let Some((nombre, id)) = song.get("name").and_then(|n| n.as_str()).zip(song.get("id").and_then(|i| i.as_str())) {
            let label_text = nombre.to_string(); // Convertimos el &str a String
            let id_text = id.to_string(); // Convertimos el &str a String


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
            let id_text_like = id_text.clone();
            dislike.connect_clicked(move |_| {
                commando_bajar(&label_text_like,&id_text_like);
            });


            let label_text_dislike = label_text.clone();
            let id_text_dislike = id_text.clone();
            // Manejar clics en el botón "like"
            like.connect_clicked(move |_| {
                commando_subida(&label_text_dislike,&id_text_dislike);
            });


            // Agregar iconos a los botones


            filas.pack_start(&label, false, false, 15);
            filas.pack_start(&like, false, false, 0);
            filas.pack_start(&dislike, false, false, 10);
            caja.pack_start(&filas, false, false, 10);
        }
    }
}


fn commando_subida(nombre_cancion: &str,id_cancion: &str){

    let json_data = json!({
        "command": "Vote-up",
        "id": id_cancion
    });
    let comando = json_data.to_string();

    println!("{}", comando);
    abrir_socket_mensaje(&comando);

}


fn commando_bajar(nombre_cancion: &str, id_cancion: &str){

    let json_data = json!({
        "command": "Vote-Down",
        "id": id_cancion
    });
    let comando = json_data.to_string();

    println!("{}", comando);
    abrir_socket_mensaje(&comando);
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

fn convert_unicode_sequences(songs: Vec<Value>) -> Vec<Value> {
    songs.iter().map(|song| {
        if let Some(name) = song.get("name") {
            if let Some(name_str) = name.as_str() {
                let unescaped_name = unescape_unicode(name_str);
                let mut song_copy = song.clone();
                song_copy["name"] = Value::String(unescaped_name);
                return song_copy;
            }
        }
        song.clone() // Si no se pudo procesar, simplemente devuelve una copia del elemento original
    }).collect()
}

fn unescape_unicode(input: &str) -> String {
    let mut result = String::new();
    let mut chars = input.chars();
    while let Some(c) = chars.next() {
        if c == '\\' {
            if let Some('u') = chars.next() {
                let code = chars.take(4).collect::<String>();
                if let Ok(unicode_char) = u32::from_str_radix(&code, 16) {
                    if let Some(unicode) = std::char::from_u32(unicode_char) {
                        result.push(unicode);
                        continue;
                    }
                }
            }
        }
        result.push(c);
    }
    result
}