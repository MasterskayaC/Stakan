/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonsLayout;
    QPushButton *connectButton;
    QPushButton *placeOrderButton;
    QPushButton *getUpdateButton;
    QHBoxLayout *mainAreaLayout;
    QGroupBox *asksGroup;
    QVBoxLayout *vboxLayout;
    QTableView *asksView;
    QGroupBox *orderGroup;
    QVBoxLayout *vboxLayout1;
    QLabel *sideLabel;
    QRadioButton *askRadio;
    QRadioButton *bidRadio;
    QLabel *priceLabel;
    QDoubleSpinBox *priceSpinBox;
    QLabel *quantityLabel;
    QSpinBox *quantitySpinBox;
    QSpacerItem *verticalSpacer;
    QGroupBox *bidsGroup;
    QVBoxLayout *vboxLayout2;
    QTableView *bidsView;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(900, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QVBoxLayout(centralwidget);
        mainLayout->setObjectName("mainLayout");
        buttonsLayout = new QHBoxLayout();
        buttonsLayout->setObjectName("buttonsLayout");
        connectButton = new QPushButton(centralwidget);
        connectButton->setObjectName("connectButton");

        buttonsLayout->addWidget(connectButton);

        placeOrderButton = new QPushButton(centralwidget);
        placeOrderButton->setObjectName("placeOrderButton");

        buttonsLayout->addWidget(placeOrderButton);

        getUpdateButton = new QPushButton(centralwidget);
        getUpdateButton->setObjectName("getUpdateButton");

        buttonsLayout->addWidget(getUpdateButton);


        mainLayout->addLayout(buttonsLayout);

        mainAreaLayout = new QHBoxLayout();
        mainAreaLayout->setObjectName("mainAreaLayout");
        asksGroup = new QGroupBox(centralwidget);
        asksGroup->setObjectName("asksGroup");
        vboxLayout = new QVBoxLayout(asksGroup);
        vboxLayout->setObjectName("vboxLayout");
        asksView = new QTableView(asksGroup);
        asksView->setObjectName("asksView");

        vboxLayout->addWidget(asksView);


        mainAreaLayout->addWidget(asksGroup);

        orderGroup = new QGroupBox(centralwidget);
        orderGroup->setObjectName("orderGroup");
        vboxLayout1 = new QVBoxLayout(orderGroup);
        vboxLayout1->setObjectName("vboxLayout1");
        sideLabel = new QLabel(orderGroup);
        sideLabel->setObjectName("sideLabel");

        vboxLayout1->addWidget(sideLabel);

        askRadio = new QRadioButton(orderGroup);
        askRadio->setObjectName("askRadio");

        vboxLayout1->addWidget(askRadio);

        bidRadio = new QRadioButton(orderGroup);
        bidRadio->setObjectName("bidRadio");

        vboxLayout1->addWidget(bidRadio);

        priceLabel = new QLabel(orderGroup);
        priceLabel->setObjectName("priceLabel");

        vboxLayout1->addWidget(priceLabel);

        priceSpinBox = new QDoubleSpinBox(orderGroup);
        priceSpinBox->setObjectName("priceSpinBox");
        priceSpinBox->setMaximum(999999.000000000000000);

        vboxLayout1->addWidget(priceSpinBox);

        quantityLabel = new QLabel(orderGroup);
        quantityLabel->setObjectName("quantityLabel");

        vboxLayout1->addWidget(quantityLabel);

        quantitySpinBox = new QSpinBox(orderGroup);
        quantitySpinBox->setObjectName("quantitySpinBox");
        quantitySpinBox->setMaximum(999999);

        vboxLayout1->addWidget(quantitySpinBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        vboxLayout1->addItem(verticalSpacer);


        mainAreaLayout->addWidget(orderGroup);

        bidsGroup = new QGroupBox(centralwidget);
        bidsGroup->setObjectName("bidsGroup");
        vboxLayout2 = new QVBoxLayout(bidsGroup);
        vboxLayout2->setObjectName("vboxLayout2");
        bidsView = new QTableView(bidsGroup);
        bidsView->setObjectName("bidsView");

        vboxLayout2->addWidget(bidsView);


        mainAreaLayout->addWidget(bidsGroup);


        mainLayout->addLayout(mainAreaLayout);

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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Stakan Console", nullptr));
        connectButton->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        placeOrderButton->setText(QCoreApplication::translate("MainWindow", "Place an order", nullptr));
        getUpdateButton->setText(QCoreApplication::translate("MainWindow", "Get Update", nullptr));
        asksGroup->setTitle(QCoreApplication::translate("MainWindow", "Asks", nullptr));
        orderGroup->setTitle(QCoreApplication::translate("MainWindow", "Order", nullptr));
        sideLabel->setText(QCoreApplication::translate("MainWindow", "Side", nullptr));
        askRadio->setText(QCoreApplication::translate("MainWindow", "Ask", nullptr));
        bidRadio->setText(QCoreApplication::translate("MainWindow", "Bid", nullptr));
        priceLabel->setText(QCoreApplication::translate("MainWindow", "Price", nullptr));
        quantityLabel->setText(QCoreApplication::translate("MainWindow", "Quantity", nullptr));
        bidsGroup->setTitle(QCoreApplication::translate("MainWindow", "Bids", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
