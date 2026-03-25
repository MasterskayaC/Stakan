#include"graphicalui.h"

GraphicalUI::GraphicalUI(QWidget *parent):QMainWindow(parent){
    ui.setupUi(this);   
    connect(ui.pushButton1,&QPushButton::clicked,this,&GraphicalUI::PushButton1Clicked);
    connect(ui.actiontest_sub,&QAction::triggered,this,&GraphicalUI::TestSubClicked);
}

int GraphicalUI::Run(int argc, char *argv[]){
    QApplication a(argc, argv);
    GraphicalUI graph;
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
