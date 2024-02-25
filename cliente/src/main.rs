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
            .default_height(350)
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
        let label_text = format!("Cnacion {}",i+1);
        let buttoni = Button::with_label(&label_text);
        buttoni.set_size_request(100,50);
        caja.pack_start(&buttoni, false, false, 0);
        buttoni.connect_clicked(|_| {
            eprintln!("Clicked!");
        });
    }
    


    



}
