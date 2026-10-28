# Lokalna baza SQLite

Aplikacja korzysta ze sterownika Qt `QSQLITE` i automatycznie tworzy sześć
tabel ze schematu `schema.sql` podczas pierwszego uruchomienia. Nie wymaga
serwera ani danych dostępowych MySQL. Przy dystrybucji aplikacji należy
dołączyć plugin Qt `sqldrivers/qsqlite.dll` (np. przez `windeployqt`).

Domyślny plik na Windows: `%LOCALAPPDATA%/Spizarka/spizarka.sqlite`.
Dokładna ścieżka jest wypisywana w logu uruchomienia. Zmienna środowiskowa
`SPIZARKA_DB_PATH` pozwala wskazać inny plik. Baza pozostaje zachowana
po zamknięciu programu i przebudowaniu projektu.

Nowa i istniejąca baza otrzymuje 20 wspólnych składników i 6 przykładowych
przepisów (właściciel `userId = 0`). Migracja do wersji 2 dodaje dane raz,
bez nadpisywania danych użytkowników. Ilości dotyczą jednej porcji, a kcal
są orientacyjne. Zasoby użytkowników pozostają bez zmian.
Najpierw zarejestruj konto, żeby korzystać z aplikacji.
Dane z dawnej bazy MySQL nie są pobierane automatycznie.
Do ich przeniesienia potrzebny jest eksport danych ze starej bazy.

Zachowane są nazwy tabel i kolumn istniejących zapytań. `id` jest kluczem
`INTEGER PRIMARY KEY AUTOINCREMENT`, a `rfidBool` domyślnie wynosi 0.
`userId = 0` oznacza wspólne składniki i przepisy i nie wymaga konta systemowego.
`tagId` jest tekstem, ponieważ czytnik zwraca kod szesnastkowy, który może
zaczynać się od zer. `ingrOrCraftBool` oznacza 0 dla składnika i 1 dla przepisu.
Odwołania `ingrOrCraftId` są polimorficzne, więc nie mają pojedynczego klucza
obcego. Klucze obce składników przepisów oraz właścicieli zasobów i RFID są
włączane przy każdym otwarciu połączenia.

Kopię zapasową pliku SQLite wykonuj po zamknięciu aplikacji.

Test integracyjny: uruchom `qmake tests/database_tests.pro` w osobnym katalogu
budowania, następnie `mingw32-make` i `database_tests.exe`.
