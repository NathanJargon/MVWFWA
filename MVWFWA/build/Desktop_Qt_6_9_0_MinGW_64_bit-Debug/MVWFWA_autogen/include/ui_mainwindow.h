/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget1;
    QTableWidget *tableWidget2;
    QLabel *orderLabel;
    QHBoxLayout *buttonLayout;
    QPushButton *runButton;
    QPushButton *fileButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1280, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        tableWidget1 = new QTableWidget(centralwidget);
        tableWidget1->setObjectName("tableWidget1");

        verticalLayout->addWidget(tableWidget1);

        tableWidget2 = new QTableWidget(centralwidget);
        tableWidget2->setObjectName("tableWidget2");

        verticalLayout->addWidget(tableWidget2);

        orderLabel = new QLabel(centralwidget);
        orderLabel->setObjectName("orderLabel");
        orderLabel->setAlignment(Qt::AlignLeft);

        verticalLayout->addWidget(orderLabel);

        buttonLayout = new QHBoxLayout();
        buttonLayout->setObjectName("buttonLayout");
        runButton = new QPushButton(centralwidget);
        runButton->setObjectName("runButton");

        buttonLayout->addWidget(runButton);

        fileButton = new QPushButton(centralwidget);
        fileButton->setObjectName("fileButton");

        buttonLayout->addWidget(fileButton);


        verticalLayout->addLayout(buttonLayout);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Modified Vertex-Weight Floyd-Warshall", nullptr));
        orderLabel->setText(QCoreApplication::translate("MainWindow", "Execution Time: ", nullptr));
        runButton->setText(QCoreApplication::translate("MainWindow", "Run", nullptr));
        runButton->setStyleSheet(QCoreApplication::translate("MainWindow", "background-color: green; color: white; font-weight: bold;", nullptr));
        fileButton->setText(QCoreApplication::translate("MainWindow", "Dataset File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
