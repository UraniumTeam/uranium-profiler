#pragma once
#include "ProfilingSession.h"
#include "FileParser.h"
#include <QMainWindow>
#include <QTableWidget>
#include <vector>

class MainFrame;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    MainFrame* m_MainFrame;
    QTableWidget* m_FunctionInfoTable;
    QPalette m_Palette;

    void createDockWidgets();

protected:
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;
};
