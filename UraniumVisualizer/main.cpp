#include "mainwindow.h"
#include "ProfilingSession.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    w.showMaximized();
    UN::Print(UN::ProfilingSession::ToString(UN::ProfilingSession::GetFakeProfilingSession()));
    return QApplication::exec();
}
