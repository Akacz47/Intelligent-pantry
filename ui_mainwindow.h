/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOption_3;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QPushButton *option1Button;
    QPushButton *option2Button;
    QLabel *welcomeLabel;
    QSpacerItem *verticalSpacer;
    QMenuBar *menubar;
    QMenu *menuMenu;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(750, 434);
        MainWindow->setMinimumSize(QSize(400, 200));
        MainWindow->setMaximumSize(QSize(750, 500));
        actionOption_3 = new QAction(MainWindow);
        actionOption_3->setObjectName("actionOption_3");
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setMinimumSize(QSize(0, 0));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName("gridLayout");
        option1Button = new QPushButton(centralwidget);
        option1Button->setObjectName("option1Button");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(option1Button->sizePolicy().hasHeightForWidth());
        option1Button->setSizePolicy(sizePolicy);

        gridLayout->addWidget(option1Button, 3, 0, 1, 1);

        option2Button = new QPushButton(centralwidget);
        option2Button->setObjectName("option2Button");
        sizePolicy.setHeightForWidth(option2Button->sizePolicy().hasHeightForWidth());
        option2Button->setSizePolicy(sizePolicy);
        option2Button->setSizeIncrement(QSize(0, 0));
        option2Button->setBaseSize(QSize(0, 0));
        option2Button->setLayoutDirection(Qt::LeftToRight);

        gridLayout->addWidget(option2Button, 4, 0, 1, 1);

        welcomeLabel = new QLabel(centralwidget);
        welcomeLabel->setObjectName("welcomeLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(welcomeLabel->sizePolicy().hasHeightForWidth());
        welcomeLabel->setSizePolicy(sizePolicy1);
        welcomeLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(welcomeLabel, 1, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 750, 21));
        menuMenu = new QMenu(menubar);
        menuMenu->setObjectName("menuMenu");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuMenu->menuAction());
        menuMenu->addAction(actionOption_3);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionOption_3->setText(QCoreApplication::translate("MainWindow", "Opcja 3", nullptr));
        option1Button->setText(QCoreApplication::translate("MainWindow", "Opcja 1", nullptr));
        option2Button->setText(QCoreApplication::translate("MainWindow", "Opcja 2", nullptr));
        welcomeLabel->setText(QCoreApplication::translate("MainWindow", "Witaj:", nullptr));
        menuMenu->setTitle(QCoreApplication::translate("MainWindow", "Menu", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
