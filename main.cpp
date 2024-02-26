#include <iostream>

using namespace std;

struct node{
    int data;
    node* next;

};

node *head = head, *newnode , *tail;
void create_CLL(){



    newnode = new node;
    cout<<"Enter data "<<endl;
    cin>>newnode->data;
    newnode ->next =  NULL;
    if (head == NULL){
         head = tail = newnode;

    }
    else{
        tail -> next = newnode;
        tail =newnode;

    }
    tail ->next = head ;
    cout<<"check "<<tail->next->data<<endl;



}

void display(){

    node *trav = head;
    while(trav->next != tail->next){
        cout<<trav->data<<" ";
        trav = trav->next;


    }
    cout<<trav->data<<endl;



}


void menu(){
    cout<<"Enter 1 create circular ll"<<endl;
    cout<<"display circular ll"<<endl;



}

int main() {

    int op;
    while(true){
        menu();
        cin>>op;

        switch (op) {

            case 1:
                create_CLL();
                break;
            case 2:
                display();
                break;
            default:
                cout<<"invalid"<<endl;

        }

    }




}
