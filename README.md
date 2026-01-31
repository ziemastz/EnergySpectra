# EnergySpectra

Prosta aplikacja desktopowa Qt (Qt Charts) do wczytywania plików z energiami, budowania i podglądu widm (histogramów znormalizowanych), oraz zarządzania wieloma seriami na jednym wykresie. Wydzielone biblioteki pozwalają ponownie używać wykresu i serwisu w innych aplikacjach.

## Funkcje
- Wczytywanie wielu plików tekstowych (`*.tab`, `*.dat`, `*.txt`) i nakładanie ich na jeden wykres.
- Parsowanie danych: pierwsza kolumna = `eventId` (opcjonalnie), kolejne kolumny = energie; linie zaczynające się od `#` są ignorowane.
- Histogram z konfigurowalną szerokością koszyka (`binWidth`, domyślnie 0.1; łatwo podpiąć pod kontrolkę UI).
- Normalizacja: gdy są identyfikatory zdarzeń – dzielnik to liczba unikalnych zdarzeń; gdy brak ID – dzielnik to liczba wpisów.
- Statystyki per plik: liczba zdarzeń, liczba wpisów (entries), min/max/mean energii, liczba zer (energies == 0).
- Tabela plików z checkboxem widoczności serii, możliwością usuwania wierszy (numeracja aktualizowana).
- Interaktywność wykresu: zoom kółkiem (także selektywnie po osi), przesuwanie myszą.
- Eksport:
  - `Export Data (TXT)` – zapis wszystkich serii do TSV (nagłówki: Series, X, Y) kompatybilnego z Excel; nazwa z timestampem.
  - `Export Chart (PNG)` – zapis bieżącego widoku wykresu do obrazu (PNG/JPEG), również z nazwą opartą o timestamp.
- Obliczenia w osobnym wątku (`SpectrumService`) – UI nie blokuje się przy dużych plikach.

## Struktura katalogów
- `chartlib/` – biblioteka statyczna z komponentem `SpectrumChartWidget` i `ZoomableChartView`.
- `spectrumlib/` – biblioteka statyczna z serwisem (`SpectrumService`) i DTO (`SpectrumDtos`).
- `app/` – aplikacja GUI korzystająca z obu bibliotek.
- `EnergySpectra.pro` – projekt nadrzędny (qmake, TEMPLATE=subdirs).

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
7) Eksport danych lub obrazu: przyciski pod wykresem; domyślne nazwy plików zawierają timestamp.

## Budowanie i uruchamianie
Wymagania: Qt 6 (z modułem Qt Charts) lub Qt 5.15 z odpowiednim add-onem. Projekt korzysta z dwóch bibliotek statycznych (`chartlib`, `spectrumlib`).

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
- Eksport danych w CSV z wyborem separatora i zakresu osi.  
- Przełącznik trybu normalizacji (per event / per wpis).  
- Podgląd liczby zer w tooltipie serii lub w stopce wykresu.
