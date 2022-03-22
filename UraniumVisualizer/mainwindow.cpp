#include <QPushButton>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Uranium profiler v0.1.0");
    resize(800,  600);

    auto* button = new QPushButton(this);
    button->setText("Hello, World!");
    button->move(350, 250);
    button->resize(100, 100);
}

