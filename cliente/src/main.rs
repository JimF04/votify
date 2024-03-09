mod cliente_Server;

use gtk::gdk::keys::constants::CD;
use gtk::prelude::*;
use gtk::{Application, ApplicationWindow, Button,Label,Box};
use std::sync::{Arc, Mutex};
use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::str::from_utf8;
use std::thread;

fn main() {


    match TcpStream::connect("localhost:50000") {
        Ok(mut stream) => {
            println!("Successfully connected to server in port 50000");

            let msg = b"Hello!";

            stream.write(msg).unwrap();
            println!("Sent Hello, awaiting reply...");

            let mut data = [0 as u8; 6]; // using 6 byte buffer
            match stream.read_exact(&mut data) {
                Ok(_) => {
                    if &data == msg {
                        println!("Reply is ok!");
                    } else {
                        let text = from_utf8(&data).unwrap();
                        println!("Unexpected reply: {}", text);
                    }
                },
                Err(e) => {
                    println!("Failed to receive data: {}", e);
                }
            }
        },
        Err(e) => {
            println!("Failed to connect: {}", e);
        }
    }
    println!("Terminated.");



    let application = Application::builder()
        .application_id("com.example.FirstGtkApp")
        .build();

    application.connect_activate(|app| {
        let window = ApplicationWindow::builder()
            .application(app)
            .title("Votify")
            .default_width(350)
            .default_height(400)
            .build();


        // 
        let container = Box::new(gtk::Orientation::Vertical,0);

        let label = Label::new (Some("Lista de canciones"));
        
        label.set_size_request(100,50);
        container.pack_start(&label, false, false, 5);


        crear_songs(&window, &container);


        window.add(&container);


        window.show_all();
    });

    application.run();






}




fn crear_songs(ventana: &ApplicationWindow,caja: &Box) {


    let votos = Arc::new(Mutex::new(Vec::new()));

    for n in 0..2{
        let label_text = format!("Cancion {}",n+1);

        let filas = Box::new(gtk::Orientation::Horizontal,0);
        let label = Label::new (Some(&label_text));
        label.set_halign(gtk::Align::Start);
        label.set_size_request(200,50);

        let like = Button::with_label("👍");
        like.set_size_request(50,50);


        let votos_ref = Arc::clone(&votos);

        like.connect_clicked(move |_| {
            
            let valor = n + 1;
            votos_ref.lock().unwrap().push(valor);
            println!("{:?}", votos_ref.lock().unwrap());
            

        });



        let dislike = Button::with_label("👎");
        dislike.set_size_request(50,50);


        filas.pack_start(&label, false, false, 0);
        filas.pack_start(&like, false, false, 0);
        filas.pack_start(&dislike, false, false, 0);
        caja.pack_start(&filas, false, false, 0);
        

    }
    



    



}




