[English](#english) | [Polski](#polski)

<a id="english"></a>

# Intelligent Pantry

A desktop application for managing food inventory, planning meals, and identifying products using RFID. It helps users check what is in their pantry, find meals they can prepare with available ingredients, and identify missing items for their shopping list.

The project was developed as part of the engineering thesis **“Design and Implementation of an Intelligent Pantry Management Application Using RFID Technology”**, Artur Piotrowski, Szczecin 2024. It started as a team project and was extended during the thesis work with RFID integration, refactoring, the strategy pattern, account settings, and password changes, among other improvements.

This guide describes the current code: the application uses a local SQLite database instead of an external MySQL server.

## Features

- User registration, login, logout, and password changes.
- Shared ingredients and recipes, with the ability to add and remove private entries.
- Recipes with ingredient quantities and calorie values.
- Pantry inventory management: adding products, updating quantities, and removing stock.
- Recipe suggestions based on available ingredients and quantities, including the number of servings that can be prepared.
- Highlighting recipes that exceed the user's configured calorie limit.
- A shopping list that accounts for missing ingredients and can be saved to and loaded from a file.
- Ingredient and recipe search with filters and sorting.
- Optional assignment, reading, and removal of RFID tag associations with products.

## How is the data organized?

An **ingredient** defines a product, such as milk measured in `ml`. A **recipe** specifies the required ingredients, quantities, and calorie value of a dish. An **inventory item** represents a product actually held in the pantry, such as 500 ml of milk or a prepared meal.

Having milk in the ingredient catalog does not mean the user has milk in stock. Likewise, predefined recipes describe possible dishes rather than prepared meals added to the inventory.

## Technologies and requirements

| Component | Purpose |
| --- | --- |
| C++17 | Application logic |
| Qt Widgets, Qt Core, Qt GUI | Desktop interface, signals and slots, file handling |
| Qt SQL and SQLite (`QSQLITE`) | Local data storage |
| qmake and `spizarka.pro` | Build configuration |
| `.ui` files and `main.qss` | Window layouts and interface styling |
| Vendor SDK / `VHLib.dll` | Optional communication with the RFID reader |

The project targets **Windows 10/11**. Building has been verified with **Qt 6.8.2 and MinGW 13.1.0 64-bit**. A Qt kit with the Widgets and SQL modules and the SQLite plugin is required. An RFID reader is not required for the other features.

## Running in Qt Creator

1. Clone the project:

   ```sh
   git clone git@github.com:Akacz47/Intelligent-pantry.git
   cd Intelligent-pantry
   ```

2. Open `spizarka.pro` in Qt Creator.
3. Select the **Desktop Qt 6.8.2 MinGW 64-bit** kit.
4. Run **qmake**, then build and run the application.
5. Enter a username and password in the login window and select **Zarejestruj się** (Register). Once registered, log in to the new account.

The application interface is in Polish; the instructions below retain its menu labels.

### Building from the terminal

Run the following commands from the project root in PowerShell, with the selected Qt and MinGW tools available in `PATH`:

```powershell
New-Item -ItemType Directory -Force build/desktop | Out-Null
Set-Location build/desktop
qmake ../../spizarka.pro "CONFIG+=debug" "CONFIG-=release"
mingw32-make debug -j8
./debug/spizarka.exe
```

When distributing the executable outside the Qt development environment, include the required libraries and plugins using `windeployqt` from the same Qt kit. In particular, the `sqldrivers/qsqlite.dll` plugin is required. The vendor's RFID library must be provided separately.

## First steps

1. Log in and open the ingredient and recipe lists from the top menu.
2. Select **Zasoby → Dodaj** (Inventory → Add) and enter an existing ingredient's name and the quantity you have, in that ingredient's unit.
3. For example, add 3 `Jajka` (eggs), 10 g of `Masło` (butter), and 1 g of `Sól` (salt).
4. Click **Sugeruj przepis** (Suggest a recipe). These quantities provide one serving of scrambled eggs.
5. To find missing products for another dish, open its ingredients by activating the recipe row and use **Dodaj składniki do listy zakupów** (Add ingredients to the shopping list).

The calorie limit and RFID options can be changed under **Konto → Ustawienia** (Account → Settings). The shopping list is kept in application memory; save it to a file from the shopping list menu if you want to keep it for later.

## Local database

The default file location is:

```text
%LOCALAPPDATA%\Spizarka\spizarka.sqlite
```

The application automatically creates the directory and file if they do not exist. On first opening, it also creates the tables and **20 predefined ingredients and 6 recipes**: pancakes, banana oatmeal, scrambled eggs, rice with chicken and broccoli, pasta in tomato sauce, and feta salad. The quantities describe one serving, and the calorie values are approximate.

Shared ingredients, recipes, and their associations use **`userId = 0`**. Private entries and inventory belong to the logged-in user's ID.

| Table | Contents |
| --- | --- |
| `users` | Login data and account settings |
| `ingredients` | Ingredients and their units |
| `crafting` | Recipes and calorie values |
| `crafting_ingredients` | Ingredients required by recipes and their quantities |
| `resources` | Product quantities in the user's pantry |
| `rfid` | Associations between tag identifiers and products |

The `SPIZARKA_DB_PATH` environment variable allows you to select a different database location. For example, in the current PowerShell session:

```powershell
$env:SPIZARKA_DB_PATH = "$PWD/my-data/spizarka.sqlite"
```

## RFID reader

The project used a **UHF Desktop Reader 2** from RFID Polska, connected over USB, and passive inventory tags. The application communicates with the reader in its SDK mode.

The program loads `VHLib.dll` from the directory containing `spizarka.exe`. The library and its dependencies must match the application's architecture: a 64-bit application requires a 64-bit DLL. The included `sdk/INC/VHLib.lib` is a 32-bit import library and is not directly linked into the current application.

Without a compatible SDK or a connected reader, RFID functionality remains unavailable, while manual pantry management remains usable. Once the reader is connected correctly, enable RFID in the account settings, associate a tag with an ingredient or recipe, and start reading.

## Project structure

| Files / directory | Responsibility |
| --- | --- |
| `main.cpp`, `main.qss`, `resources.qrc` | Application startup, palette, styles, and bundled resources |
| `login.*`, `account.*`, `settings.*` | Login, password changes, and settings |
| `mainwindow.*` | Main views, recipe suggestions, and shopping list |
| `baseclass.*` | Shared data operations |
| `ingredients.*`, `recipes.*`, `resources.*` | Product management forms |
| `search.*`, `shoppinglist.*`, `remove*` | Search, shopping list additions, and removal forms |
| `rfid.*`, `rfidsdk.*`, `sdk/` | Tag handling strategies and SDK integration |
| `database.*`, `database/` | SQLite initialization, migrations, schema, and sample data |
| `tests/` | SQLite integration tests |

## Tests

The integration test uses a separate temporary database. It checks file and table creation, predefined data, migration of an existing database, data persistence, transactions, foreign keys, and the query that displays inventory for different product types and users.

From the project root, in a terminal configured for Qt/MinGW:

```powershell
New-Item -ItemType Directory -Force build/database-tests | Out-Null
Set-Location build/database-tests
qmake ../../tests/database_tests.pro
mingw32-make release -j8
./release/database_tests.exe
```

A successful run ends with `SQLite integration tests passed` and exit code 0. These tests do not replace testing with a physical RFID reader.

---

<a id="polski"></a>

[English](#english) | [Polski](#polski)

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
