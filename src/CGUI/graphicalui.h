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
struct Snapshot;
struct TopLevel;
struct ClientError{};

class GraphicalUI:public QMainWindow{
    Q_OBJECT
     Ui_MainWindow ui;
     IOrderBookClient* client;
public:
    GraphicalUI(const IOrderBookClient* client = nullptr, QWidget *parent = nullptr);
    static int Run(int argc, char* argv[]);
    
private:
    void OnConnected();
    void OnDisconnected();
    void OnStateChanged();
    void OnSnapshot(const Snapshot&);
    void OnTopOfBook(const TopLevel&);
    void OnError(ClientError, std::string_view);
    void TestSubClicked();
    void PushButton1Clicked();
    void OnConnectClicked();
    void OnPlaceOrderClicked();
    void OnGetUpdateClicked();
};

