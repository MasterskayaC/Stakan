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

class IOrderBookClient;

class GraphicalUI:public QMainWindow{
    Q_OBJECT
     Ui_MainWindow ui;
     IOrderBookClient* client;
public:
    GraphicalUI()=default;
    GraphicalUI(const IOrderBookClient* client, QWidget *parent = nullptr);
    static int Run(int argc, char* argv[]);
    void notify();
    void LoadConfiguration();
private:

    void TestSubClicked();
    void PushButton1Clicked();
};

void notify(GraphicalUI& gui);
