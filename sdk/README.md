# Biblioteka czytnika RFID

Dołączony `INC/VHLib.lib` jest biblioteką importową **32-bitową (x86)**.
Nie można jej połączyć z aplikacją zbudowaną przez Qt MinGW 64-bit.

Aplikacja ładuje teraz `VHLib.dll` w czasie działania, z katalogu obok
`spizarka.exe`. Budowanie i uruchamianie aplikacji nie wymaga sterownika RFID.
Bez zgodnej biblioteki połączenie z czytnikiem jest niedostępne; szczegóły
są wypisywane w logu aplikacji.

Dla zestawu Desktop Qt 6.8.2 MinGW 64-bit potrzebny jest **64-bitowy**
`VHLib.dll` od producenta, wraz z jego zależnościami. Sama zmiana nazwy
lub skopiowanie 32-bitowej biblioteki nie rozwiązuje niezgodności.
Alternatywnie można użyć 32-bitowego zestawu Qt z 32-bitowym DLL producenta.

Po zmianie pliku `spizarka.pro` w Qt Creator wybierz **Uruchom qmake**,
a następnie zbuduj projekt ponownie.
