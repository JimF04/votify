

slint::slint! {
    import { Button, VerticalBox } from "std-widgets.slint";
    component MainWindow inherits Window {

        VerticalBox {
            alignment: start;
            Text {text: "hello uwu";}
            Button {text: "uwu";}
            Rectangle {
                width: 100px;
                height: 100px;
                background:  #3960D5;
            }


        }
   
    }
    
}

fn main() {
    MainWindow::new().unwrap().run().unwrap();
}