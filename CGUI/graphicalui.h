#pragma once
#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include"ui_mainwindow.h"
class GraphicalUI:public QMainWindow{
    Q_OBJECT
     Ui_MainWindow ui;
    
public:
    GraphicalUI(QWidget *parent = nullptr);
    static int Run(int argc, char* argv[]);
private:
    void TestSubClicked();
    void PushButton1Clicked();
};

