//#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QDebug>
// #include "VHHandsetAPI.h"

// QString byteToString(const BYTE *data, int length)
// {
//     QString result;
//     for (int i = 1; i < length; ++i)
//     {
//         // Dodajemy kolejne bajty do wynikowego QStringa jako szesnastkowe wartości
//         result += QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
//     }
//     return result;
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Spizarka");
    a.setStyle("Fusion");
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#f3f6fa"));
    palette.setColor(QPalette::WindowText, QColor("#182b3a"));
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, QColor("#edf3f9"));
    palette.setColor(QPalette::Text, QColor("#182b3a"));
    palette.setColor(QPalette::Button, QColor("#e8eef5"));
    palette.setColor(QPalette::ButtonText, QColor("#182b3a"));
    palette.setColor(QPalette::Highlight, QColor("#165d96"));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, QColor("#536171"));
    palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor("#586777"));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#536171"));
    a.setPalette(palette);

    QFile file(":/main.qss");  // Jeśli plik jest w zasobach
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qWarning("Nie można otworzyć pliku stylów!");
    } else {
        QTextStream stream(&file);
        a.setStyleSheet(stream.readAll());
        file.close();
    }

    Login w;
    w.show();

    // apiReturn result = VH_InitUsb(NULL);

    // HANDLE hScanner;
    // result = VH_ConnectScannerUsb(&hScanner);
    // if (result == _OK) {
    //     qDebug() << result;

    //     BYTE IDBuffer[100];//24, 100
    //     int nCounter = 0;
    //     result = EPC1G2_ReadLabelID(hScanner, 1, 0, 0, NULL, IDBuffer, &nCounter, 0);
    //     if (result == _OK) {
    //         qDebug() << "Odczytano EPC NO.:";
    //         //qDebug() << "Tag" << ":" << byteToString(IDBuffer,13);//5
    //         for (int i = 0; i < nCounter; ++i) {
    //             qDebug() << "Tag" << i+1 << ":" << byteToString(IDBuffer,13);//5
    //             // qDebug() << "Tag" << i+1 << ":" << IDBuffer;
    //         }
    //     } else {
    //         qDebug() << "Błąd odczytu EPC NO.: " << result;
    //     }

    //     // Zamknięcie połączenia USB
    //     result = VH_DisconnectScannerUsb(hScanner);

    //     // Zamknięcie połączenia USB
    //     result = VH_CloseUsb();
    // }
    // else {
    //     qDebug() << "Brak podlaczonego czytnika rfid";
    // }

    return a.exec();
}
