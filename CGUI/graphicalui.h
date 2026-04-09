// graphicalui.h
#pragma once
#include <QMainWindow>
#include "ui_mainwindow.h"

class GraphicalUI : public QMainWindow {
    Q_OBJECT
    Ui_MainWindow ui;
public:
    explicit GraphicalUI(QWidget* parent = nullptr);
    static int Run(int argc, char* argv[]);

private slots:
    void OnInput();

private:
    void Print(const QString& text);
};