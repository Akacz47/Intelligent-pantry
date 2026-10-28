#include "rfidsdk.h"
#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>

namespace {
struct VendorSdk {
    QLibrary library{QCoreApplication::applicationDirPath() + "/VHLib.dll"};
    decltype(&::VH_InitUsb) init = nullptr;
    decltype(&::VH_ConnectScannerUsb) connect = nullptr;
    decltype(&::VH_DisconnectScannerUsb) disconnect = nullptr;
    decltype(&::VH_CloseUsb) close = nullptr;
    decltype(&::EPC1G2_ReadLabelID) read = nullptr;
    bool available = false;

    template <typename Function>
    Function resolve(const char *name, int stackBytes)
    {
        auto symbol = library.resolve(name);
        // Some 32-bit vendor DLLs export stdcall-decorated names.
        if (!symbol) {
            const QByteArray decorated = QByteArray("_") + name + "@"
                                         + QByteArray::number(stackBytes);
            symbol = library.resolve(decorated.constData());
        }
        return reinterpret_cast<Function>(symbol);
    }

    VendorSdk()
    {
        if (library.load()) {
            init = resolve<decltype(init)>("VH_InitUsb", 4);
            connect = resolve<decltype(connect)>("VH_ConnectScannerUsb", 4);
            disconnect = resolve<decltype(disconnect)>("VH_DisconnectScannerUsb", 4);
            close = resolve<decltype(close)>("VH_CloseUsb", 0);
            read = resolve<decltype(read)>("EPC1G2_ReadLabelID", 32);
            available = init && connect && disconnect && close && read;
        }
        if (!available)
            qWarning().noquote() << "RFID niedostepne: umiesc VHLib.dll zgodny z architektura aplikacji"
                                << "obok pliku EXE. Szczegoly:" << library.errorString();
    }
};

VendorSdk &sdk()
{
    static VendorSdk instance;
    return instance;
}
}

namespace RfidSdk {
apiReturn VH_InitUsb(VH_fDechUsb callback)
{
    auto &vendor = sdk();
    return vendor.available ? vendor.init(callback) : _no_scanner;
}

apiReturn VH_ConnectScannerUsb(HANDLE *scanner)
{
    if (!scanner)
        return _parameter_error;
    *scanner = nullptr;
    auto &vendor = sdk();
    return vendor.available ? vendor.connect(scanner) : _no_scanner;
}

apiReturn VH_DisconnectScannerUsb(HANDLE scanner)
{
    auto &vendor = sdk();
    return vendor.available && scanner ? vendor.disconnect(scanner) : _no_scanner;
}

apiReturn VH_CloseUsb()
{
    auto &vendor = sdk();
    return vendor.available ? vendor.close() : _no_scanner;
}

apiReturn EPC1G2_ReadLabelID(HANDLE scanner, BYTE mem, int ptr, BYTE len,
                           BYTE *mask, BYTE *buffer, int *count, int address)
{
    if (!count)
        return _parameter_error;
    *count = 0;
    auto &vendor = sdk();
    return vendor.available && scanner
        ? vendor.read(scanner, mem, ptr, len, mask, buffer, count, address)
        : _no_scanner;
}
}
