# Inteligentna spiżarka

Desktopowa aplikacja do zarządzania produktami spożywczymi, planowania posiłków i identyfikacji produktów za pomocą RFID. Pozwala sprawdzić, co znajduje się w spiżarce, jakie potrawy można przygotować z dostępnych składników i czego brakuje na liście zakupów.

Projekt rozwijany w ramach pracy inżynierskiej **„Projekt i implementacja aplikacji do zarządzania inteligentną spiżarką z wykorzystaniem technologii RFID”**, Artur Piotrowski, Szczecin 2024. Punktem wyjścia był projekt zespołowy, rozbudowany w pracy m.in. o integrację RFID, refaktoryzację, wzorzec strategii, ustawienia konta i zmianę hasła.

Niniejsza instrukcja opisuje aktualny kod: zamiast zewnętrznego serwera MySQL aplikacja używa lokalnej bazy SQLite.

## Funkcje

- Rejestracja, logowanie, wylogowanie i zmiana hasła użytkownika.
- Wspólna lista składników i przepisów oraz możliwość dodawania i usuwania własnych pozycji.
- Przepisy z ilościami składników i określoną kalorycznością.
- Zarządzanie zasobami: dodawanie produktów, zmiana ilości i usuwanie ze spiżarki.
- Sugerowanie przepisów na podstawie posiadanych składników i ich ilości, z wyliczeniem liczby możliwych porcji.
- Oznaczanie przepisów przekraczających ustawiony przez użytkownika limit kalorii.
- Lista zakupów uwzględniająca brakujące składniki, z możliwością zapisu do pliku i odczytu.
- Wyszukiwanie składników i przepisów z filtrami oraz sortowaniem.
- Opcjonalne przypisywanie, odczytywanie i usuwanie powiązań znaczników RFID z produktami.

## Jak uporządkowane są dane?

**Składnik** to definicja produktu, np. mleko z jednostką `ml`. **Przepis** określa potrzebne składniki, ich ilości i kaloryczność potrawy. **Zasób** oznacza produkt faktycznie posiadany w spiżarce, np. 500 ml mleka lub gotową potrawę.

Samo istnienie mleka na liście składników nie oznacza, że użytkownik ma je w zasobach. Podobnie predefiniowane przepisy są propozycjami potraw, a nie gotowymi posiłkami dodanymi do inwentarza.

## Technologie i wymagania

| Element | Zastosowanie |
| --- | --- |
| C++17 | Logika aplikacji |
| Qt Widgets, Qt Core, Qt GUI | Interfejs desktopowy, sygnały i sloty, obsługa plików |
| Qt SQL i SQLite (`QSQLITE`) | Lokalny zapis danych |
| qmake i plik `spizarka.pro` | Konfiguracja budowania |
| Pliki `.ui` i `main.qss` | Układ okien oraz wygląd interfejsu |
| SDK producenta / `VHLib.dll` | Opcjonalna komunikacja z czytnikiem RFID |

Projekt przeznaczony jest dla **Windows 10/11**. Budowanie sprawdzono z **Qt 6.8.2 i MinGW 13.1.0 64-bit**. Potrzebny jest zestaw Qt z modułami Widgets i SQL oraz pluginem SQLite. Czytnik RFID nie jest wymagany do korzystania z pozostałych funkcji.

## Uruchomienie w Qt Creator

1. Pobierz projekt:

   ```sh
   git clone git@github.com:Akacz47/Intelligent-pantry.git
   cd Intelligent-pantry
   ```

2. Otwórz plik `spizarka.pro` w Qt Creator.
3. Wybierz zestaw **Desktop Qt 6.8.2 MinGW 64-bit**.
4. Uruchom **qmake**, a następnie zbuduj projekt i uruchom aplikację.
5. W oknie logowania wpisz login i hasło, a następnie wybierz **Zarejestruj się**. Po rejestracji zaloguj się na utworzone konto.

### Budowanie z terminala

Poniższe polecenia wykonaj w PowerShell z narzędziami wybranego zestawu Qt i MinGW dostępnymi w `PATH`, z katalogu głównego projektu:

```powershell
New-Item -ItemType Directory -Force build/desktop | Out-Null
Set-Location build/desktop
qmake ../../spizarka.pro "CONFIG+=debug" "CONFIG-=release"
mingw32-make debug -j8
./debug/spizarka.exe
```

Przy przenoszeniu pliku EXE poza środowisko Qt dołącz biblioteki i pluginy za pomocą `windeployqt` z tego samego zestawu Qt. W szczególności potrzebny jest plugin `sqldrivers/qsqlite.dll`. Bibliotekę producenta RFID dostarcza się osobno.

## Pierwsze użycie

1. Zaloguj się i wyświetl listę składników oraz przepisów z górnego menu.
2. Wybierz **Zasoby → Dodaj** i wpisz nazwę istniejącego składnika oraz posiadaną ilość w jego jednostce.
3. Przykładowo dodaj `Jajka` w ilości 3, `Masło` w ilości 10 g i `Sól` w ilości 1 g.
4. Kliknij **Sugeruj przepis**. Dla powyższych zasobów dostępna będzie jedna porcja jajecznicy.
5. Aby uzupełnić brakujące produkty do innej potrawy, otwórz jej składniki przez aktywację wiersza przepisu i użyj przycisku **Dodaj składniki do listy zakupów**.

Limit kalorii i opcje RFID można zmienić w **Konto → Ustawienia**. Lista zakupów jest przechowywana w pamięci aplikacji; aby zachować ją na później, zapisz ją do pliku z menu listy zakupów.

## Lokalna baza danych

Domyślna lokalizacja pliku:

```text
%LOCALAPPDATA%\Spizarka\spizarka.sqlite
```

Jeżeli katalog lub plik nie istnieje, aplikacja tworzy je automatycznie. Przy pierwszym otwarciu tworzy też tabele oraz **20 predefiniowanych składników i 6 przepisów**: naleśniki, owsiankę z bananem, jajecznicę, ryż z kurczakiem i brokułem, makaron w sosie pomidorowym oraz sałatkę z fetą. Ilości w tych przepisach dotyczą jednej porcji, a kaloryczność jest orientacyjna.

Wspólne składniki, przepisy i ich powiązania mają **`userId = 0`**. Prywatne pozycje i zasoby są przypisane do identyfikatora zalogowanego użytkownika.

| Tabela | Zawartość |
| --- | --- |
| `users` | Dane logowania i ustawienia konta |
| `ingredients` | Składniki i ich jednostki |
| `crafting` | Przepisy i kaloryczność |
| `crafting_ingredients` | Składniki wymagane przez przepisy i ich ilości |
| `resources` | Ilości produktów w spiżarce użytkownika |
| `rfid` | Powiązania identyfikatorów znaczników z produktami |

Zmienna środowiskowa `SPIZARKA_DB_PATH` pozwala wskazać inną lokalizację bazy. Przykład dla bieżącej sesji PowerShell:

```powershell
$env:SPIZARKA_DB_PATH = "$PWD/moje-dane/spizarka.sqlite"
```

## Czytnik RFID

W projekcie wykorzystano **UHF Desktop Reader 2** firmy RFID Polska, połączony przez USB, oraz pasywne etykiety inwentaryzacyjne. Integracja aplikacji korzysta z trybu SDK czytnika.

Program ładuje `VHLib.dll` z katalogu obok `spizarka.exe`. Biblioteka i jej zależności muszą być zgodne z architekturą aplikacji: wersja 64-bitowa programu wymaga 64-bitowego DLL. Dołączony `sdk/INC/VHLib.lib` jest 32-bitową biblioteką importową i nie jest bezpośrednio linkowany do obecnej aplikacji.

Bez zgodnego SDK lub podłączonego czytnika obsługa RFID pozostaje niedostępna, a aplikacja nadal pozwala ręcznie zarządzać spiżarką. Po poprawnym podłączeniu urządzenia można włączyć RFID w ustawieniach konta, przypisać tag do składnika lub przepisu i rozpocząć odczyt.

## Struktura projektu

| Pliki / katalog | Odpowiedzialność |
| --- | --- |
| `main.cpp`, `main.qss`, `resources.qrc` | Start aplikacji, paleta, style i zasoby |
| `login.*`, `account.*`, `settings.*` | Logowanie, zmiana hasła i ustawienia |
| `mainwindow.*` | Główne widoki, sugestie przepisów i lista zakupów |
| `baseclass.*` | Wspólne operacje na danych |
| `ingredients.*`, `recipes.*`, `resources.*` | Formularze zarządzania produktami |
| `search.*`, `shoppinglist.*`, `remove*` | Wyszukiwanie, dodawanie do listy zakupów i formularze usuwania |
| `rfid.*`, `rfidsdk.*`, `sdk/` | Strategie obsługi znaczników i integracja SDK |
| `database.*`, `database/` | Otwarcie SQLite, migracje, schemat i dane przykładowe |
| `tests/` | Testy integracyjne SQLite |

## Testy

Test integracyjny korzysta z osobnej, tymczasowej bazy. Sprawdza m.in. tworzenie pliku i tabel, dane predefiniowane, migrację istniejącej bazy, trwałość danych, transakcje, klucze obce oraz zapytanie wyświetlające zasoby różnych typów i użytkowników.

Z katalogu głównego projektu, w terminalu skonfigurowanym dla Qt/MinGW:

```powershell
New-Item -ItemType Directory -Force build/database-tests | Out-Null
Set-Location build/database-tests
qmake ../../tests/database_tests.pro
mingw32-make release -j8
./release/database_tests.exe
```

Prawidłowy wynik kończy się komunikatem `SQLite integration tests passed` i kodem wyjścia 0. Testy te nie zastępują testu z fizycznym czytnikiem RFID.
