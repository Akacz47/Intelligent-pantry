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
