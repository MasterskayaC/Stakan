#include"graphicalui.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QEventLoop>

GraphicalUI::GraphicalUI() {
    window = new QMainWindow();
    textDisplay = new QTextEdit();
    inputLine = new QLineEdit();
    progressBar = new QProgressBar();
    
    // Создаем центральный виджет и layout
    QWidget* central = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->addWidget(textDisplay);
    layout->addWidget(inputLine);
    layout->addWidget(progressBar);
    
    window->setCentralWidget(central);
    window->show();
}

void GraphicalUI::ShowMessage(const std::string& msg) {
    textDisplay->append(QString::fromStdString(msg));
}

std::string GraphicalUI::GetUserInput(const std::string& prompt) {
    ShowMessage(prompt);
    QEventLoop loop;
    connect(inputLine, &QLineEdit::returnPressed, &loop, &QEventLoop::quit);
    loop.exec();
    QString result = inputLine->text();
    inputLine->clear();
    return result.toStdString();
}

void GraphicalUI::DisplayError(const std::string& error) {
    QMessageBox::critical(window, "Ошибка", 
                          QString::fromStdString(error));
}

void GraphicalUI::ShowProgress(int percent) {
    progressBar->setValue(percent);
}

void GraphicalUI::Clear() {
    textDisplay->clear();
}