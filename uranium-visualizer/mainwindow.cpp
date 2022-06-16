#include "mainwindow.h"
#include "MainFrame.h"
#include <QApplication>
#include <QDockWidget>
#include <QFileDialog>
#include <QHeaderView>
#include <QPushButton>
#include <QResizeEvent>
#include <fstream>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Uranium profiler v0.1.0"));
    setUnifiedTitleAndToolBarOnMac(true);

    setStyleSheet(R"(
QMessageBox {
    background-color: rgb(53, 53, 53);
}
QMenuBar::item:selected {
    color: white;
    background: #5D5E5F;
}
QMenuBar QMenu::item:selected {
    color: black;
    background-color: #90a0c0;
}
QMessageBox QPushButton {
    color: rgb(53, 53, 53);
    background-color: #F2F2F2;
}
QDockWidget::title {
    background-color: #505050;
}
QDockWidget QTableView::item:alternate {
    background-color: #252525;
}
QDockWidget QTableView::item {
    background-color: #404040;
}
QDockWidget QTableView::item:selected {
    background-color: #90a0c0;
}
)");

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

    m_Palette.setColor(QPalette::Highlight, QColor(0x90a0c0));
    m_Palette.setColor(QPalette::HighlightedText, Qt::black);
    m_Palette.setColor(QPalette::Shadow, Qt::black);
    QApplication::setPalette(m_Palette);

    m_MainFrame = new MainFrame(this);
    m_MainFrame->setEnabled(false);
    m_MainFrame->resize(width(), height());
    setCentralWidget(m_MainFrame);

    connect(m_MainFrame->FunctionSelectionChanged, &QAction::triggered, this, &MainWindow::selectFunction);

    m_MainFrame->setAutoFillBackground(true);
    m_MainFrame->show();

    createActions();
    createDockWidgets();
}

void MainWindow::createActions()
{
    m_OpenAct = new QAction(tr("&Open"), this);
    m_OpenAct->setShortcuts(QKeySequence::Open);
    m_OpenAct->setStatusTip(tr("Open a session file"));
    connect(m_OpenAct, &QAction::triggered, this, &MainWindow::openFile);
    m_AboutAct = new QAction(tr("&About"), this);
    m_AboutAct->setStatusTip(tr("About the software"));
    connect(m_AboutAct, &QAction::triggered, this, &MainWindow::showAbout);

    menuBar()->setStyleSheet("background-color: rgb(53, 53, 53);");
    m_FileMenu = menuBar()->addMenu(tr("&File"));
    m_FileMenu->addAction(m_OpenAct);
    m_FileMenu->setStyleSheet("background-color: rgb(53, 53, 53);");
    m_HelpMenu = menuBar()->addMenu(tr("&Help"));
    m_HelpMenu->addAction(m_AboutAct);
    m_HelpMenu->setStyleSheet("background-color: rgb(53, 53, 53);");
}

void MainWindow::openFile()
{
    QFileDialog dialog(this, tr("Open a session file"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Uranium session files (*.ups)"));
    dialog.setViewMode(QFileDialog::Detail);
    if (!dialog.exec())
    {
        return;
    }
    m_FunctionInfoTable->clearContents();
    m_MainFrame->clearProfilingSessions();

    auto filename = dialog.selectedFiles()[0].toStdString();
    std::ifstream file(filename);
    std::string upt;
    auto hasErrors = false;
    while (file >> upt)
    {
        std::vector<UN::ParsingProblem> problems;
        auto session = UN::FileParser::open(upt.c_str(), problems).parse();
        if (!problems.empty())
        {
            std::stringstream ss;
            if (upt.length() > 32)
            {
                upt = upt.substr(0, 29) + "...";
            }
            ss << "<h3>Problems while parsing the file <em>\"" << upt << "\"</em> of session <em>\"" << filename
               << "\"</em>.</h3>";
            for (const auto& problem : problems)
            {
                hasErrors |= problem.Kind == UN::ParsingProblemKind::Error;
                if (problem.Kind == UN::ParsingProblemKind::Error)
                {
                    ss << "<p style='color: red'>";
                }
                else
                {
                    ss << "<p style='color: yellow'>";
                }
                ss << problem.toString() << "</p>";
            }
            if (hasErrors)
            {
                ss << "Aborting due to previous error(s)";
            }
            QMessageBox msgBox(this);
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setWindowTitle(tr("Problems with session file"));
            msgBox.setTextFormat(Qt::RichText);
            msgBox.setText(ss.str().c_str());
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
            if (hasErrors)
            {
                m_MainFrame->clearProfilingSessions();
                break;
            }
        }
        m_MainFrame->addProfilingSession(session);
    }
    m_MainFrame->setEnabled(!hasErrors);
    m_MainFrame->update();
}

void MainWindow::showAbout()
{
    const char* message = R"(<h3>Uranium Profiler v0.1.0</h3>
        Fast instrumentation for <em>Rust</em><br>
        Visit our <a href="https://github.com/UraniumTeam/">GitHub</a> for more info and support.<br>
        <br>
        Contributors:
        <ul>
            <li>Nikita Dubovikov</li>
            <li>Dmitry Ishoev</li>
            <li>Daniil Lapygin</li>
        </ul>
        <h4>Copyright UraniumTeam (C) 2022</h4>
    )";
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("About"));
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(message);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::createDockWidgets()
{
    auto* dock = new QDockWidget(tr("Function Info"), this);
    dock->setPalette(m_Palette);
    m_FunctionInfoTable = new QTableWidget(dock);
    m_FunctionInfoTable->setColumnCount(6);
    m_FunctionInfoTable->setRowCount(1);
    m_FunctionInfoTable->verticalHeader()->setVisible(false);
    m_FunctionInfoTable->setStyleSheet("QHeaderView::section, QHeaderView { background-color: #626262 }");
    m_FunctionInfoTable->setShowGrid(false);
    m_FunctionInfoTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_FunctionInfoTable->setAlternatingRowColors(true);
    m_FunctionInfoTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_FunctionInfoTable->setSelectionMode(QAbstractItemView::SingleSelection);

    m_FunctionInfoTable->setHorizontalHeaderLabels(
        QStringList{} << "Function"
                      << "Self (ns)"
                      << "Total (ms)"
                      << "Total (%)"
                      << "Max (ms)"
                      << "Count");
    m_FunctionInfoTable->horizontalHeader()->setStyleSheet("QHeaderView::section, QHeaderView { background-color: #626262 }");
    dock->setWidget(m_FunctionInfoTable);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}

void MainWindow::selectFunction()
{
    m_FunctionInfoTable->clearContents();
    m_FunctionInfoTable->setRowCount(1);
    if (!m_MainFrame->SelectedFunction.has_value())
    {
        return;
    }

    auto call = m_MainFrame->SelectedFunction.value();
    auto descendants = call.Session->functionDescendants(call.BeginIndex);
    m_FunctionInfoTable->setRowCount((int)descendants.size());
    for (int i = 0; i < descendants.size(); ++i)
    {
        double selfNanos, totalMs, totalPercent, maxMs;
        uint64_t count;
        call.Session->functionStats(descendants[i], selfNanos, totalMs, totalPercent, maxMs, count);

//        connect( m_FunctionInfoTable->verticalHeader(), &QHeaderView::sectionClicked , this, [](int idx){
//
//        });
        auto event = call.Session->events()[descendants[i]];
        auto functionName  = call.Session->header().functionNames()[event.functionIndex()];
        auto* functionItem = new QTableWidgetItem(QString::fromStdString(functionName));
        m_FunctionInfoTable->setItem(i, 0, functionItem);
        auto* selfMsItem = new QTableWidgetItem(QString::number(std::round(selfNanos)));
        m_FunctionInfoTable->setItem(i, 1, selfMsItem);
        auto* totalMsItem = new QTableWidgetItem(QString::number(totalMs));
        m_FunctionInfoTable->setItem(i, 2, totalMsItem);
        auto* totalPercentItem = new QTableWidgetItem(QString::number(totalPercent));
        m_FunctionInfoTable->setItem(i, 3, totalPercentItem);
        auto* maxMsItem = new QTableWidgetItem(QString::number(maxMs));
        m_FunctionInfoTable->setItem(i, 4, maxMsItem);
        auto* countItem = new QTableWidgetItem(QString::number(count));
        m_FunctionInfoTable->setItem(i, 5, countItem);
    }
}
