static window_size: i32 = 800;

fn crear_cancion(){





}

slint::slint! {
    import { Button, VerticalBox, HorizontalBox } from "std-widgets.slint";



    component Cancion inherits VerticalLayout {

            HorizontalLayout {
            Text {
        text: "cancion";
                }

            Rectangle {
        width: 400px;
        height: 50px;
        background: red;
                }


}

}

    component MainWindow inherits Window {
        width: 800px;
        title: "VOTIFY";
        background: #FB88B4;





        Cancion{}



    }
}

fn main() {
    MainWindow::new().unwrap().run().unwrap();

}
