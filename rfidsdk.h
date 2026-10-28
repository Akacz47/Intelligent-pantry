#ifndef RFIDSDK_H
#define RFIDSDK_H

#include "VHHandsetAPI.h"

// The vendor DLL must match the application's architecture and be placed
// beside the executable. Missing/incompatible SDKs report a connection error.
namespace RfidSdk {
apiReturn VH_InitUsb(VH_fDechUsb callback);
apiReturn VH_ConnectScannerUsb(HANDLE *scanner);
apiReturn VH_DisconnectScannerUsb(HANDLE scanner);
apiReturn VH_CloseUsb();
apiReturn EPC1G2_ReadLabelID(HANDLE scanner, BYTE mem, int ptr, BYTE len,
                           BYTE *mask, BYTE *buffer, int *count, int address);
}

#endif
