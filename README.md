
```
mkdir release
cd release
cmake ..

- polecenie make tworzy plik wykonywalny poly całego kalkulatora,
- polecenie make test tworzy plik wykonywalny poly_test z testami biblioteki poly,
- polecenie make doc tworzy dokumentację w formacie doxygen.

```
### Opis programu

Zadanie polega na zaimplementowaniu operacji na wielomianach
rzadkich wielu zmiennych o współczynnikach całkowitych. Zmienne wielomianu
oznaczamy <img src="https://render.githubusercontent.com/render/math?math=x_0, x_1, x_2"> itd. Definicja wielomianu jest rekurencyjna. Wielomian
jest sumą jednomianów postaci <img src="https://render.githubusercontent.com/render/math?math=p*x_0^{n}">, gdzie <img src="https://render.githubusercontent.com/render/math?math=n"> jest wykładnikiem tego jednomianu
będącym nieujemną liczbą całkowitą, a <img src="https://render.githubusercontent.com/render/math?math=p"> jest współczynnikiem, który jest wielomianem.
Współczynnik w jednomianie zmiennej <img src="https://render.githubusercontent.com/render/math?math=x_{i}"> jest sumą jednomianów zmiennej <img src="https://render.githubusercontent.com/render/math?math=x_{i + 1}"> . Rekurencja
kończy się, gdy współczynnik jest liczbą (czyli wielomianem stałym), a nie sumą kolejnych
jednomianów. Wykładniki jednomianów w każdej z rozważanych sum są parami różne. Wielomiany
są rzadkie, co oznacza, że stopień wielomianu może być znacznie większy niż liczba składowych jednomianów.

### Najważniejsze dostępne operacje i szczególy implementacji

Dążymy do tego, aby wielomian był zawsze w formie uproszczonej(wielomiany postaci <img src="https://render.githubusercontent.com/render/math?math=cx^{0}"> i <img src="https://render.githubusercontent.com/render/math?math=0x^{n}">
są zawsze upraszczane do c i 0), miał posortowane według wykładniku jednomiany.

- Tworzenie wielomianu

    Dostępne są 3 rodzaje tej operacji: tworzenie wielomianu tożsamościowo równego zeru, tworzenie wielomianu stałego,
    tworzenie wielomianu z tablicy jednomianów.
    - Tworzenie wielomianu z tablicy jednomianów

        Funkcja sortuje tablicę jednomianów według wykładnika i sumuje jednomiany o tym samym wykładniku.
        Więc dla każdego wykładnika istnieje dokładnie jeden jednomian w wielomianie z takim wykładnikiem.
        Ponadto funkcja zapewnia brak zerowych jednomianów w nowym wielomianie.
        Wielomiany postaci <img src="https://render.githubusercontent.com/render/math?math=cx^{0}"> i <img src="https://render.githubusercontent.com/render/math?math=0x^{n}"> są zawsze upraszczane do <img src="https://render.githubusercontent.com/render/math?math=c"> i <img src="https://render.githubusercontent.com/render/math?math=0">
- Dodawanie

    Operacja jest podzielona na trzy przypadki w zależności od tego, czy wielomian jest stały. Sumują się jednomiany
    o tym samym wykładniku. Zera w wynikowym wielomianie są pomijane. Wynik jest w formie uproszczonej.
- Mnożenie

    Operacja jest podzielona na trzy przypadki w zależności od tego, czy wielomian jest stały. Przywraca
    zepsuty porządek jednomianów. Zera w wynikowym wielomianie są pomijane. Wynik jest w formie uproszczonej.
- Odejmowanie

    To samo co dodawanie wielomianu przeciwnego.
- Tworzenie wielomianu przeciwnego
- Obliczenie wartośći wielomianu w punkcie x

    Funkcja wstawia pod pierwszą zmienną wielomianu wartość <img src="https://render.githubusercontent.com/render/math?math=x">. Czyli dla każdego jednomianu tworzy
    wielomian: wylicza wartośc <img src="https://render.githubusercontent.com/render/math?math=x^{k}"> i stosuje mnożenie wielomianu(współczynnik jednomianu) przez
    liczbę. Wynik jest sumą wszystkich wielomianów utworzonych w ten sposób.
- Sprawdzanie równośći dwóch wielomianów

    Sprawdzanie równości jednomianów należących do wielomianów.
- Obliczanie stopnia wielomianu
- Składanie wielomianów

    Implementacja z funkcją pomocniczą, która wylicza <img src="https://render.githubusercontent.com/render/math?math=exp">-tą potęgę wielomianu <img src="https://render.githubusercontent.com/render/math?math=p">

### Kalkulator

Program kalkulatora czyta dane wierszami ze standardowego wejścia. Wiersz zawiera wielomian lub polecenie do wykonania.

Wielomian reprezentujemy jako stałą, jednomian lub sumę jednomianów. Stała jest liczbą całkowitą.
Jednomian reprezentujemy jako parę (coeff,exp), gdzie współczynnik coeff jest wielomianem, a wykładnik
exp jest liczbą nieujemną. Do wyrażenia sumy używamy znaku +. Jeśli wiersz zawiera wielomian,
to program wstawia go na stos. Przykłady poprawnych wielomianów:

<img src="https://render.githubusercontent.com/render/math?math=-2">

<img src="https://render.githubusercontent.com/render/math?math=(3,1)+(((4,4),100),2)">

Kalkulator wykonuje następujące polecenia:

- ZERO – wstawia na wierzchołek stosu wielomian tożsamościowo równy zeru;
- IS_COEFF – sprawdza, czy wielomian na wierzchołku stosu jest współczynnikiem – wypisuje na standardowe wyjście 0 lub 1;
- IS_ZERO – sprawdza, czy wielomian na wierzchołku stosu jest tożsamościowo równy zeru – wypisuje na standardowe wyjście 0 lub 1;
- CLONE – wstawia na stos kopię wielomianu z wierzchołka;
- ADD – dodaje dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich sumę;
- MUL – mnoży dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich iloczyn;
- NEG – neguje wielomian na wierzchołku stosu;
- SUB – odejmuje od wielomianu z wierzchołka wielomian pod wierzchołkiem, usuwa je i wstawia na wierzchołek stosu różnicę;
- IS_EQ – sprawdza, czy dwa wielomiany na wierzchu stosu są równe – wypisuje na standardowe wyjście 0 lub 1;
- DEG – wypisuje na standardowe wyjście stopień wielomianu (−1 dla wielomianu tożsamościowo równego zeru);
- DEG_BY idx – wypisuje na standardowe wyjście stopień wielomianu ze względu na zmienną o numerze <img src="https://render.githubusercontent.com/render/math?math=idx"> (−1 dla wielomianu tożsamościowo równego zeru);
- AT x – wylicza wartość wielomianu w punkcie <img src="https://render.githubusercontent.com/render/math?math=x">, usuwa wielomian z wierzchołka i wstawia na stos wynik operacji;
- PRINT – wypisuje na standardowe wyjście wielomian z wierzchołka stosu;
- POP – usuwa wielomian z wierzchołka stosu.
- COMPOSE - zdejmuje z wierzchołka stosu najpierw wielomian <img src="https://render.githubusercontent.com/render/math?math=p">, a potem kolejno wielomiany <img src="https://render.githubusercontent.com/render/math?math=q_{k - 1}, q_{k - 2}, \ldots, q_0"> i
umieszcza na stosie wynik operacji złożenia

### Szczególy implementacji kalkulatora

- Stack

    Stos wielomianów. Zaimplementowany za pomocą tablicy dynamicznej. Dostępne są wszystkie podstawowe funkcje stosa.
    Przyjmuje na własność wielomiany.
- Parser

    Przetwarza wierszy ze standardowego wejśćia. Jeśli nie ma więcej znakówów do odczytania(EOF na końcu wiersza),
    umieszcza EOF z powrotem. Dostępnych jest również kilka przydatnych funkcji : PeekChar, SkipLine, PrintError
- Executor

    Wykonuje polecenia kalkulatora
- Vector

    Pomocnicza dynamiczna tablica jednomianów.

### Dodatek
W interfejsie zostały przyjęte pewne konwencje, które mają ułatwić zarządzanie pamięcią. Dzięki tym konwencjom wiadomo,
co jest właścicielem obiektu. Bycie właścicielem obiektu implikuje odpowiedzialność za zwolnienie używanej przez niego
pamięci. W przypadku struktur Poly i Mono zwalnianie pamięci uzyskuje się poprzez wywołania odpowiednio funkcji PolyDestroy
i MonoDestroy.

Podstawową konwencją jest przekazywanie argumentów przez zmienną. Kod wołający funkcję, której przekazujemy argument
przez zmienną, odpowiada za utworzenie odpowiedniego wskaźnika. Odpowiedzialność za zwolnienie tak uzyskanej pamięci
nigdy nie przechodzi na wołaną funkcję.

Przy niektórych funkcjach argumenty przechodzą na własność funkcji wołanej. Jest to zaznaczone w komentarzu
opisującym daną funkcję. Funkcja przejmuje zawartość pamięci wskazywanej przez przekazany wskaźnik. Zazwyczaj jest to
pojedyncza struktura (np. Poly, Mono) bądź tablica struktur.

Wynikiem niektórych funkcji jest struktura (np. Poly, Mono). Przyjmujemy tu konwencję otrzymywania wyniku na własność,
co oznacza, że kod wołający taką funkcję otrzymuję zwracaną wartość na własność.

### Konstruowanie wielomianu z tablicy jednomianów
Dostępne funkcje:

- PolyOwnMonos - przejmuje na własność tablicę jednomianów i jej zawartość. Może dowolnie modyfikować
zawartość tej pamięci.
- PolyAddMonos - przejmuje na własność tablicę jednomianów (ale nie zawartość)
- PolyCloneMonos - nie modyfikuje zawartości tablicy jednomianów. Jeśli jest to wymagane, to wykonuje pełne kopie
jednomianów z tablicy
