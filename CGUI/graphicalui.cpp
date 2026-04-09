// graphicalui.cpp
#include "graphicalui.h"
#include <QApplication>

GraphicalUI::GraphicalUI(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    connect(ui.inputLine, &QLineEdit::returnPressed, this, &GraphicalUI::OnInput);
    connect(ui.sendButton, &QPushButton::clicked, this, &GraphicalUI::OnInput);

    Print("Stakan Console. Type 'help' for available commands.");
}

void GraphicalUI::OnInput() {
    QString input = ui.inputLine->text().trimmed();
    if (input.isEmpty()) return;

    Print("> " + input);
    ui.inputLine->clear();

    // TODO: передать в Menu::ParseCommand
    if (input == "help") {
        Print("Commands: buy <qty> <price>, sell <qty> <price>, book <ms>, exit, help");
    } else if (input == "exit") {
        close();
    } else {
        Print("Unknown command: " + input);
    }
}

void GraphicalUI::Print(const QString& text) {
    ui.outputArea->appendPlainText(text);
}

int GraphicalUI::Run(int argc, char* argv[]) {
    QApplication a(argc, argv);
    GraphicalUI w;
    w.show();
    return a.exec();
}