extern crate gtk;
use gtk::prelude::*;
use gtk::{Label, Window, WindowType};

fn main() {
    // Initialize GTK
    gtk::init().expect("Failed to initialize GTK.");

    // Create a new top-level window
    let window = Window::new(WindowType::Toplevel);
    window.set_title("Hello GTK");
    window.set_default_size(300, 200);

    // Create a label
    let label = Label::new(Some("Hello, GTK!"));

    // Add the label to the window
    window.add(&label);

    // Handle the destroy event
    window.connect_destroy(|_| {
        // Terminate the GTK main loop
        gtk::main_quit();
    });

    // Show all components in the window
    window.show_all();

    // Start the GTK main loop
    gtk::main();
}
