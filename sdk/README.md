# Biblioteka czytnika RFID

Dołączony `INC/VHLib.lib` jest biblioteką importową **32-bitową (x86)**.
Nie można jej połączyć z aplikacją zbudowaną przez Qt MinGW 64-bit.
Można użyć 32-bitowego zestawu Qt z 32-bitowym DLL producenta.

Aplikacja ładuje teraz `VHLib.dll` w czasie działania, z katalogu obok
`spizarka.exe` (Po skompilowaniu aplikacji). Budowanie i uruchamianie programu
nie wymaga sterownika RFID. Bez zgodnej biblioteki połączenie z czytnikiem jest
niedostępne; szczegóły są wypisywane w logu aplikacji.
