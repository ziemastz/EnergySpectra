# EnergySpectra

Prosta aplikacja desktopowa Qt (Qt Charts) do wczytywania plików z energiami, budowania i podglądu widm (histogramów znormalizowanych), oraz zarządzania wieloma seriami na jednym wykresie.

## Funkcje
- Wczytywanie wielu plików tekstowych (`*.tab`, `*.dat`, `*.txt`) i nakładanie ich na jeden wykres.
- Parsowanie danych: pierwsza kolumna = `eventId` (opcjonalnie), kolejne kolumny = energie; linie zaczynające się od `#` są ignorowane.
- Histogram z konfigurowalną szerokością koszyka (w kodzie `binWidth`, domyślnie 0.1; łatwo podpiąć pod kontrolkę UI).
- Normalizacja: jeżeli w pliku występują identyfikatory zdarzeń, wartości Y są dzielone przez liczbę unikalnych zdarzeń; gdy brak ID – dzielnik to liczba wpisów. Dzięki temu tabela i wykres są spójne.
- Statystyki per plik: liczba zdarzeń, liczba wpisów (entries), min/max/mean energii, liczba zer (energies == 0).
- Tabela plików z checkboxem widoczności serii, możliwością usuwania wierszy (numeracja aktualizowana).
- Interaktywność wykresu: zoom kółkiem (również selektywnie po osi, gdy kursor przy osi), przesuwanie wykresu myszą (lewy/środkowy przycisk).
- Obsługa błędów w UI (status bar + message box) bez blokowania aplikacji – obliczenia w osobnym wątku (`SpectrumService`).

## Struktura plików
- `mainwindow.cpp/h/ui` – logika UI, tabela, wykres, obsługa sygnałów serwisu.
- `zoomablechartview.cpp/h` – rozszerzony `QChartView` z pan/zoom.
- `spectrumservice.cpp/h` – wątek serwisowy: parsowanie pliku, histogram, statystyki.
- `SpectrumDtos.h` – struktury DTO (`SpectrumResult`, `SpectrumStats`).
- `EnergySpectra.pro` – projekt qmake.

## Format danych wejściowych
```
# komentarze są pomijane
<eventId> <energia1> <energia2> ...
<eventId> <energia1>
... 
```
- Kolumny rozdzielane dowolną liczbą spacji lub tabów.
- `eventId` może być pominięty (linia z samymi energiami – wtedy licznik zdarzeń pozostaje 0, normalizacja będzie per wpis).
- Energies równe 0 są zliczane w statystyce `Zeros`, ale nie trafiają do histogramu.

## Jak używać (UI)
1) Uruchom aplikację `EnergySpectra`.  
2) Kliknij `Add`, wybierz plik z energiami.  
3) Po wczytaniu:  
   - Wykres pokazuje serię znormalizowaną (per event lub per wpis – jak wyżej).  
   - Tabela prezentuje statystyki; checkbox w kolumnie 0 steruje widocznością serii.  
4) Dodawaj kolejne pliki, aby porównywać widma.  
5) Usuń zaznaczony wiersz przyciskiem `Remove` – seria znika, numeracja w tabeli aktualizuje się.  
6) Zoom: kółko myszy (nad osią – zoom tylko tej osi; nad wykresem – zoom obu). Pan: przytrzymaj lewy/środkowy przycisk i przeciągnij.

## Budowanie i uruchamianie
Wymagania: Qt 6 (z modułem Qt Charts) lub Qt 5.15 z odpowiednim add-onem.

### qmake
```
qmake EnergySpectra.pro
make    # na Windows: nmake / jom
./EnergySpectra   # na Windows: EnergySpectra.exe
```

### Qt Creator
1) Otwórz `EnergySpectra.pro`.  
2) Wybierz zestaw kompilacyjny z Qt Charts.  
3) Zbuduj i uruchom.

## Pomysły na rozwój
- Dodać kontrolkę UI do ustawiania `binWidth`.  
- Eksport wykresu (PNG/CSV).  
- Przełącznik trybu normalizacji (per event / per wpis).  
- Podgląd liczby zer w tooltipie serii lub w stopce wykresu.
