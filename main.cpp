//#include "mainwindow.h"
#include "login.h"
#include <QApplication>
#include <QDebug>
#include "VHHandsetAPI.h"

QString byteToString(const BYTE *data, int length)
{
    QString result;
    for (int i = 1; i < length; ++i)
    {
        // Dodajemy kolejne bajty do wynikowego QStringa jako szesnastkowe wartości
        result += QString("%1").arg(data[i], 2, 16, QChar('0')).toUpper();
    }
    return result;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;
    w.show();

    // apiReturn result = VH_InitUsb(NULL);
    // //apiReturn result;
    // HANDLE hScanner;
    // result = VH_ConnectScannerUsb(&hScanner);
    // // Tutaj możesz dodać więcej kodu do obsługi czytnika RFID, na przykład odczytywanie tagów itp.

    // BYTE IDBuffer[100];//5
    // int nCounter = 0;
    // result = EPC1G2_ReadLabelID(hScanner, 1, 0, 0, NULL, IDBuffer, &nCounter, 0);
    // if (result == _OK) {
    //     qDebug() << "Odczytano EPC NO.:";
    //     for (int i = 0; i < nCounter; ++i) {
    //         qDebug() << "Tag" << i+1 << ":" << byteToString(IDBuffer,5);
    //         //qDebug() << "Tag" << i+1 << ":" << IDBuffer;
    //     }
    // } else {
    //     qDebug() << "Błąd odczytu EPC NO.: " << result;
    // }

    // // Zamknięcie połączenia USB
    // result = VH_DisconnectScannerUsb(hScanner);

    // // Zamknięcie połączenia USB
    // result = VH_CloseUsb();

    return a.exec();
}
