use gtk::prelude::*;
use gtk::{Application, ApplicationWindow, Button,Label,Box};

fn main() {
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

        // let button = Button::with_label("Click me!");
        // 
        let container = Box::new(gtk::Orientation::Vertical,0);

        let label = Label::new (Some("Lista de canciones"));
        
        label.set_size_request(100,50);
        container.pack_start(&label, false, false, 0);


        crear_songs(&window, &container);


        window.add(&container);
        // window.add(&button);

        window.show_all();
    });

    application.run();
}

fn crear_songs(ventana: &ApplicationWindow,caja: &Box) {

    for i in 0..4{
        let label_text = format!("Cancion {}",i+1);

        let filas = Box::new(gtk::Orientation::Horizontal,0);
  

        let label = Label::new (Some(&label_text));
        label.set_halign(gtk::Align::Start);
        label.set_size_request(200,50);

        let like = Button::with_label("👍");
        like.set_size_request(50,50);

        let dislike = Button::with_label("👎");
        dislike.set_size_request(50,50);



        filas.pack_start(&label, false, false, 0);
        filas.pack_start(&like, false, false, 0);
        filas.pack_start(&dislike, false, false, 0);

        caja.pack_start(&filas, false, false, 0);




    }
    



    



}
