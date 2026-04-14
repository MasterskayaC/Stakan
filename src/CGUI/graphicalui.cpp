#include "graphicalui.h"

GraphicalUI::GraphicalUI(const IOrderBookClient* client, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);
    connect(ui.connectButton, &QPushButton::clicked, this, &GraphicalUI::OnConnectClicked);
    connect(ui.placeOrderButton, &QPushButton::clicked, this, &GraphicalUI::OnPlaceOrderClicked);
    connect(ui.getUpdateButton, &QPushButton::clicked, this, &GraphicalUI::OnGetUpdateClicked);
}

int GraphicalUI::Run(int argc, char *argv[]){
    QApplication a(argc, argv);
    GraphicalUI graph(nullptr);
    graph.show();
    return a.exec();
}

void GraphicalUI::OnConnectClicked(){
    // TODO: подключение к серверу
}

void GraphicalUI::OnPlaceOrderClicked(){
    // TODO: считать поля и отправить ордер
}

void GraphicalUI::OnGetUpdateClicked(){
    // TODO: запросить снапшот
}