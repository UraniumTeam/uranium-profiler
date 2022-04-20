#include "mainwindow.h"
#include "ProfilingSession.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    UN::Print(UN::ProfilingSession::ToString(UN::ProfilingSession::GetFakeProfilingSession()));
    return a.exec();
}
