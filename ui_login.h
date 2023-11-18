/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QGridLayout *gridLayout;
    QLabel *loginLabel;
    QLineEdit *lineEditLogin;
    QLabel *passwordLabel;
    QLineEdit *lineEditPassword;
    QPushButton *loginButton;
    QPushButton *registerButton;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName("Login");
        Login->resize(400, 200);
        Login->setMinimumSize(QSize(400, 200));
        Login->setMaximumSize(QSize(400, 200));
        gridLayout = new QGridLayout(Login);
        gridLayout->setObjectName("gridLayout");
        loginLabel = new QLabel(Login);
        loginLabel->setObjectName("loginLabel");

        gridLayout->addWidget(loginLabel, 0, 0, 1, 1);

        lineEditLogin = new QLineEdit(Login);
        lineEditLogin->setObjectName("lineEditLogin");

        gridLayout->addWidget(lineEditLogin, 0, 1, 1, 1);

        passwordLabel = new QLabel(Login);
        passwordLabel->setObjectName("passwordLabel");

        gridLayout->addWidget(passwordLabel, 1, 0, 1, 1);

        lineEditPassword = new QLineEdit(Login);
        lineEditPassword->setObjectName("lineEditPassword");

        gridLayout->addWidget(lineEditPassword, 1, 1, 1, 1);

        loginButton = new QPushButton(Login);
        loginButton->setObjectName("loginButton");

        gridLayout->addWidget(loginButton, 2, 0, 1, 2);

        registerButton = new QPushButton(Login);
        registerButton->setObjectName("registerButton");

        gridLayout->addWidget(registerButton, 3, 0, 1, 2);


        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QCoreApplication::translate("Login", "Dialog", nullptr));
        loginLabel->setText(QCoreApplication::translate("Login", "Login", nullptr));
        passwordLabel->setText(QCoreApplication::translate("Login", "Has\305\202o", nullptr));
        loginButton->setText(QCoreApplication::translate("Login", "Zaloguj", nullptr));
        registerButton->setText(QCoreApplication::translate("Login", "Zarejestruj si\304\231", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
