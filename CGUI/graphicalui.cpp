#include"graphicalui.h"

GraphicalUI::GraphicalUI(const IOrderBookClient* client, QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);   
    connect(ui.pushButton1,&QPushButton::clicked,this,&GraphicalUI::PushButton1Clicked);
    connect(ui.actiontest_sub,&QAction::triggered,this,&GraphicalUI::TestSubClicked);
}

int GraphicalUI::Run(int argc, char *argv[]){
    QApplication a(argc, argv);
    GraphicalUI graph(nullptr);
    graph.show();
    return a.exec();
}

void GraphicalUI::TestSubClicked(){
    QString user_name = QInputDialog::getText(this,"","Input user name: ");
    QString password = QInputDialog::getText(this,"","Input password : ");
}

void GraphicalUI::PushButton1Clicked(){
    QMessageBox::information(this, "Привет", "Кнопка нажата!");
}

    void OnTopOfBook(const TopLevel&){}
    void OnError(ClientError, std::string_view){}
    void TestSubClicked(){}
    void PushButton1Clicked(){}
