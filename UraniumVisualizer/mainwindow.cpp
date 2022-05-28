#include "mainwindow.h"
#include "MainFrame.h"
#include <QApplication>
#include <QDockWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Uranium profiler v0.1.0"));
    setUnifiedTitleAndToolBarOnMac(true);

    m_Palette = QPalette();
    m_Palette.setColor(QPalette::Window, QColor(53, 53, 53));
    m_Palette.setColor(QPalette::WindowText, Qt::white);
    m_Palette.setColor(QPalette::Base, QColor(25, 25, 25));
    m_Palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    m_Palette.setColor(QPalette::ToolTipBase, Qt::white);
    m_Palette.setColor(QPalette::ToolTipText, Qt::white);
    m_Palette.setColor(QPalette::PlaceholderText, Qt::white);
    m_Palette.setColor(QPalette::Text, Qt::white);
    m_Palette.setColor(QPalette::Button, QColor(53, 53, 53));
    m_Palette.setColor(QPalette::ButtonText, Qt::white);
    m_Palette.setColor(QPalette::BrightText, Qt::red);
    m_Palette.setColor(QPalette::Link, QColor(42, 130, 218));

    m_Palette.setColor(QPalette::Light, QColor(0xb1b1b1));
    m_Palette.setColor(QPalette::Midlight, QColor(0x9e9e9e));
    m_Palette.setColor(QPalette::Mid, QColor(0x7e7e7e));
    m_Palette.setColor(QPalette::Dark, QColor(0x626262));
    m_Palette.setColor(QPalette::NoRole, QColor(0x626262));

    m_Palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    m_Palette.setColor(QPalette::HighlightedText, Qt::black);
    m_Palette.setColor(QPalette::Shadow, Qt::black);
    QApplication::setPalette(m_Palette);

    m_MainFrame = new MainFrame(this);
    m_MainFrame->resize(width(), height());
    setCentralWidget(m_MainFrame);

    m_MainFrame->setAutoFillBackground(true);
    auto filename = "C:/Users/raguc/TheCode/uranium-profiler/backend_manual_tests/uranium_session_bin/thread--0.upt";
    auto session = UN::FileParser::GetProfilingSession(filename);
    m_MainFrame->setProfilingSession(session);
    m_MainFrame->show();

    createDockWidgets();
}

void MainWindow::createDockWidgets()
{
    auto* dock = new QDockWidget(tr("Function Info"), this);
    dock->setPalette(m_Palette);
    m_FunctionInfoTable = new QTableWidget(dock);
    m_FunctionInfoTable->setColumnCount(3);
    m_FunctionInfoTable->setRowCount(1);
    m_FunctionInfoTable->verticalHeader()->setVisible(false);
    m_FunctionInfoTable->setStyleSheet("QHeaderView::section, QHeaderView { background-color: #626262 }");
    m_FunctionInfoTable->setShowGrid(false);

    m_FunctionInfoTable->setHorizontalHeaderLabels(
        QStringList{} << "Name"
                      << "Time in ms"
                      << "% of parent");

    for (int i = 0; i < 3; ++i)
    {
        auto* newItem = new QTableWidgetItem(tr("..."));
        m_FunctionInfoTable->setItem(0, i, newItem);
    }
    dock->setWidget(m_FunctionInfoTable);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}
