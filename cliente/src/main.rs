static window_size: i32 = 800;

slint::slint! {
    import { Button, VerticalBox, HorizontalBox } from "std-widgets.slint";

    component MainWindow inherits Window {
        width: 800px;
        title: "VOTIFY";

        VerticalLayout {



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

        VerticalLayout {

            HorizontalLayout{
                Text {
                text: "Cancion1";
            }

            Rectangle {
                width: 400px;
                height: 50px;
                background: blue;
            }

            padding-top: 50px;

        }


            

            
        }
    }
}

fn main() {
    MainWindow::new().unwrap().run().unwrap();

}
