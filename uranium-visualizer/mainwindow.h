#pragma once
#include "ProfilingSession.h"
#include "FileParser.h"
#include <QMainWindow>
#include <QTableWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <vector>

class MainFrame;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QMenu* m_FileMenu;
    QMenu* m_HelpMenu;
    QAction* m_OpenAct;
    QAction* m_AboutAct;

    MainFrame* m_MainFrame;
    QTableWidget* m_FunctionInfoTable;
    QPalette m_Palette;
    std::vector<uint32_t> m_SelectedFunctionDescendants;

    void createActions();

    void createDockWidgets();
    void openFile();
    void showAbout();

private slots:
    void selectFunction();

protected:

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;
};
