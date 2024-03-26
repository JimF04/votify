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
        json!({"album": "China", "artist": "Brother Hao", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Without-the-Communist-Party.mp3", "genre": "pop", "id": "6fff1d7b-61cc-4f5c-b853-6c963e6b4ba9", "name": "Without the Communist Party", "up_votes": 0}),
        json!({"album": "Japon", "artist": "Yonezu Kenshi", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Eine-Kleine---Yonezu-Kenshi.mp3", "genre": "jpop", "id": "0ee42edc-e0bc-4ed1-abde-69467f499b06", "name": "Eine Kleine", "up_votes": 0}),
        json!({"album": "China", "artist": "BEYOND", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_海阔天空---BEYOND.mp3", "genre": "pop", "id": "ab28dea6-cc43-446c-9281-3fc88e1d2429", "name": "海阔天空", "up_votes": 0}),
        json!({"album": "china", "artist": "Zhong Xina", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Red_Sun.mp3", "genre": "pop", "id": "ec772044-84ec-4af0-87db-ebac21d07f02", "name": "Red Sun in the Sky", "up_votes": 0}),
        json!({"album": "China", "artist": "Jia Fei", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Ye-Hua-Xiang.mp3", "genre": "pop", "id": "0153f356-0ba2-4585-bac7-feeaa81c2bc2", "name": "Ye Hua Xiang", "up_votes": 0}),
        json!({"album": "Korea", "artist": "TWICE", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Likey---TWICE.mp3", "genre": "kpop", "id": "58e488d9-b8ac-41fe-8fb7-b107da6e6642", "name": "Likey", "up_votes": 0}),
        json!({"album": "China", "artist": "BEYOND", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_歌词版---BEYOND.mp3", "genre": "pop", "id": "ee16a253-2584-42f2-a840-74c447c69510", "name": "歌词版", "up_votes": 0}),
        json!({"album": "Japon", "artist": "Yonezu Kenshi", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Orion---Yonezu-Kenshi.mp3", "genre": "jpop", "id": "2fa220a9-f364-4dee-9623-bf1480355109", "name": "Orion", "up_votes": 0}),
        json!({"album": "China", "artist": "Eggman", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Xue_Hua_Piao.mp3", "genre": "pop", "id": "68ffdffa-951d-4596-9f9a-f40afcb388d9", "name": "Xue Hua Piao", "up_votes": 0}),
        json!({"album": "China", "artist": "Tian Yiming", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Super_Idol.mp3", "genre": "pop", "id": "05c604eb-ab6d-4409-94d4-ea96048ac9cb", "name": "Super Idol", "up_votes": 0}),
        json!({"album": "Japon", "artist": "Yonezu Kenshi", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Lemon---Yonezu-Kenshi.mp3", "genre": "jpop", "id": "e7ee9a48-24c6-4a66-b7ac-c5216a7c3262", "name": "Lemon", "up_votes": 0}),
        json!({"album": "Japon", "artist": "YOASOBI", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Yoru-ni-kakeru---YOASOBI.mp3", "genre": "jpop", "id": "1f9bcdff-63f0-4385-af1c-71ea60a501d4", "name": "Yoru ni Kakeru", "up_votes": 0}),
        json!({"album": "China", "artist": "Michael Wong", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Tong-Hua---Michael-Wong.mp3", "genre": "pop", "id": "2a4b0a20-efca-4a90-abaf-ee40883fac06", "name": "Tong Hua", "up_votes": 0}),
        json!({"album": "Japon", "artist": "YOASOBI", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Idol---YOASOBI.mp3", "genre": "jpop", "id": "a8249a04-ea82-4b6b-9611-92802479407e", "name": "アイドル", "up_votes": 0}),
        json!({"album": "Korea", "artist": "TWICE", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Fancy---TWICE.mp3", "genre": "kpop", "id": "cba7e2ae-94ba-4409-982a-edb0cf15310a", "name":"Fancy", "up_votes": 0}), json!({"album": "Korea", "artist": "TWICE", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Yes-or-yes---TWICE.mp3", "genre": "kpop", "id": "9cc7ac02-c126-4e42-96cb-70bd4ccd4077", "name": "Yes or Yes", "up_votes": 0}),
        json!({"album": "Japon", "artist": "Yonezu Kenshi", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Flamingo---Yonezu-Kenshi.mp3", "genre": "jpop", "id": "fd972c2c-71a4-42ed-b481-382834e59db3", "name": "  Flamingo", "up_votes": 0}),
        json!({"album": "Japon", "artist": "YOASOBI", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_ラフレター---YOASOBI.mp3", "genre": "jpop", "id": "4be38bb8-453c-48c4-9a6a-bd7a2cfa7a9c", "name": "ラフレター", "up_votes": 0}),
        json!({"album": "Korea", "artist": "TWICE", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/tagmp3_Cry-for-me---TWICE.mp3", "genre": "kpop", "id": "eb036c60-4c77-48f9-bf7f-0000cd5e70e1", "name": "Cry for me", "up_votes": 0}),
        json!({"album": "china", "artist": "Justin Bibo", "down_votes": 0, "file_path": "/home/jose/Downloads/PlayList/Baby-Chinese.mp3", "genre": "pop", "id": "5b905406-32c1-4689-acaa-a209fc4313eb", "name": "Baby Chinese", "up_votes": 0})
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
        "id": nombre_cancion
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