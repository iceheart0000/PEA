#include "sw.h"
#include <iostream>
#include <fstream>
#include <chrono> 
#include <iomanip>
#include <vector>
#include <random>
#include <string>
using namespace std;


class ATSP_Zarzadzaj
{
private:
    int N;
    vector<vector<int>> macierz;
    int limit_czasowy_s = 900; 
    int rozwiazanie_poczatkowe = 2; // 1 - losowe, 2 - NN 
    double T0 = 1000.0; 
    double alpha = 0.99;
    double beta = 1.0;
    int epoka = 100;
    int schemat_chlodzenia = 1; // 1 - geometryczne, 2 - liniowe 

public:
    ATSP_Zarzadzaj() : N(0) {}

    bool zPliku(const string& nazwa_pliku)
    {
        ifstream plik(nazwa_pliku);
        if (!plik.is_open()) return false;
        string pierwsze_slowo;
        if (!(plik >> pierwsze_slowo)) return false; 
        if (isdigit(pierwsze_slowo[0]))
        {
            N = stoi(pierwsze_slowo);
            macierz.assign(N, vector<int>(N));
            for (int i = 0; i < N; ++i)
            {
                for (int j = 0; j < N; ++j)
                {
                    plik >> macierz[i][j];
                    if (i == j) macierz[i][j] = -1;
                }}}
        else
        {
            string slowo = pierwsze_slowo;
            N = 0;
            do
            {
                if (slowo == "DIMENSION" || slowo == "DIMENSION:")
                {
                    if (slowo == "DIMENSION")
                    {
                        plik >> slowo;
                        if (slowo == ":") plik >> N;
                        else N = stoi(slowo);
                    }
                    else { plik >> N; }
                }
                else if (slowo == "EDGE_WEIGHT_SECTION") { break; }
            } while (plik >> slowo);
            if (N <= 0) return false;
            macierz.assign(N, vector<int>(N));
            for (int i = 0; i < N; ++i)
            {
                for (int j = 0; j < N; ++j)
                {
                    plik >> macierz[i][j];
                    if (macierz[i][j] >= 100000000 || i == j) { macierz[i][j] = -1; }
                }}}
        plik.close();
        return true;
    }



    void generujRandom(int rozmiar)
    {
        N = rozmiar;
        macierz.assign(N, vector<int>(N));
        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> koszt(10, 99);

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (i == j) { macierz[i][j] = -1; }
                else { macierz[i][j] = koszt(rng); }
            }
        }
    }



    void ustawKryteriumStopu()
    {
        cout << "\nPodaj nowy limit czasu dzialania algorytmu w sekundach: "; cin >> limit_czasowy_s;
        cout << "Ustawiono limit na " << limit_czasowy_s << " s.\n";
    }



    void ustawRozwiazaniePoczatkowe()
    {
        cout << "\nWybierz sposob generowania rozwiazania poczatkowego:\n";
        cout << "1. Losowe\n";
        cout << "2. Nearest Neighbor\n";
        cout << "Twoj wybor: ";
        cin >> rozwiazanie_poczatkowe;
        if (rozwiazanie_poczatkowe != 1 && rozwiazanie_poczatkowe != 2) rozwiazanie_poczatkowe = 2;
        cout << "Zapisano opcje: " << rozwiazanie_poczatkowe << "\n";
    }

   

    void modyfikujUstawienia()
    {
        cout << "\nModyfikuj parametry\n";
        cout << "1. Temperatura poczatkowa (obecnie " << T0 << "): "; cin >> T0;
        cout << "2. Schemat chlodzenia (1 - Geometryczny [T*a], 2 - Liniowy [T-b]) (obecnie " << schemat_chlodzenia << "): "; cin >> schemat_chlodzenia;
        if (schemat_chlodzenia == 1){ cout << "Podaj wspolczynnik alpha (np. 0.99) (obecnie " << alpha << "): "; cin >> alpha; }
        else { cout << "Podaj krok beta (np. 1.0) (obecnie " << beta << "): "; cin >> beta; }
        cout << "3. Dlugosc epoki (iteracje na stopien temp.) (obecnie " << epoka << "): "; cin >> epoka;
        cout << "Parametry zaktualizowane\n";
    }


    
    void uruchomAlgorytm()
    {
        if (N == 0)
        {
            cout << "Brak danych.\n";
            return;
        }
        SymulowaneWyzarzanie sw(macierz, N);
        sw.ustawParametry(T0, alpha, beta, epoka, schemat_chlodzenia, rozwiazanie_poczatkowe, limit_czasowy_s);
        sw.run(false); 
    }
};



void pokazMenu()
{
    cout << "\nMENU\n";
    cout << "1. Wczytaj z pliku\n"; 
    cout << "2. Wprowadz kryterium stopu\n"; 
    cout << "3. Obliczanie rozwiazania poczatkowego\n"; 
    cout << "4. Modyfikuj domyslne ustawienia algorytmu\n"; 
    cout << "5. Uruchom algorytm\n"; 
    cout << "-------------------------------------------\n";
    cout << "6. Generuj macierz\n";
    cout << "0. Wyjdz\n";
    cout << "Podaj wybor: ";
}



int main()
{
    ATSP_Zarzadzaj manager;
    int wybor;
    do
    {
        pokazMenu();
        cin >> wybor;

        switch (wybor)
        {
        case 1:
        {
            string plik_nazwa;
            cout << "podaj nazwe pliku: ";
            cin >> plik_nazwa;
            if (manager.zPliku(plik_nazwa))
            {
                cout << "dane wczytano pomyslnie\n";
            }
            else
            {
                cout << "niepoprawna nazwa pliku\n";
            }
            break;
        }

        case 2:
            manager.ustawKryteriumStopu();
            break;

        case 3:
            manager.ustawRozwiazaniePoczatkowe();
            break;

        case 4:
            manager.modyfikujUstawienia();
            break;

        case 5:
            manager.uruchomAlgorytm();
            break;

        case 6: 
        {
            int rozmiar;
            cout << "Podaj ilosc miast (N): ";
            cin >> rozmiar;
            manager.generujRandom(rozmiar);
            cout << "Wygenerowano losowe dane.\n";
            break;
        }

        case 0:
            break;

        default:
            cout << "\nWybierz jedna z dostepnych opcji\n" << endl;
        }
    } while (wybor != 0);

    return 0;
}