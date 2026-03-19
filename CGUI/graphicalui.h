#pragma once
#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressBar>
#include <QLabel>
#include <QObject>       
#include <QMetaObject>    
#include <functional>  

class GraphicalUI:public QObject{
    Q_OBJECT
    QMainWindow* window;
    QTextEdit* textDisplay;
    QLineEdit* inputLine;
    QProgressBar* progressBar;
    
public:
    GraphicalUI();
        
    void ShowMessage(const std::string& msg);
    
    std::string GetUserInput(const std::string& prompt);
    
    void DisplayError(const std::string& error);
    
    void ShowProgress(int percent);
    
    void Clear();
};

