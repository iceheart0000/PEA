#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono> 
#include <iomanip>
#include <string>
#include "BestFirstSearch.h"
#include "BreadthFirstSearch.h"
using namespace std;


class ATSP_Zarzadzaj
{
private:
    int N;
    vector<vector<int>> macierz;

public:
    ATSP_Zarzadzaj() : N(0) {}

    bool zPliku(const string& nazwa_pliku)
    {
        ifstream plik(nazwa_pliku);
        if (!plik.is_open()) return false;
        plik >> N;
        macierz.assign(N, vector<int>(N));

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                plik >> macierz[i][j];
            }
        }
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
                if (i == j) { macierz[i][j] = -1;}
                
                else { macierz[i][j] = koszt(rng);}
            }}}



    void wyswietl() const
    {
        if (N < 1) { cout << "Brak danych do wyswietlenia\n"; return; }
        cout << "Liczba miast (N): " << N << "\nMacierz:\n";
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                cout << macierz[i][j] << "\t";
            }
            cout << "\n";}}



    void uruchom()
    {
        if (N == 0)
        {
            cout << "Liczba miast rowna zero. Brak danych do obliczen." << endl;
            return;
        }

        int wybor_algorytmu;
        cout << "\n Wybierz algorytm:\n";
        cout << "1. Best-First-Search\n";
        cout << "2. Breadth-First-Search\n";
        cout << "Wybor: "; cin >> wybor_algorytmu;


        switch (wybor_algorytmu)
        {
        case 1:
        {
            cout << "\n";
            BestFirstSearch bfs(macierz, N);
            WynikBest w = bfs.run(false, 300); 
            cout << "Czas wykonania: " << w.czas_us << " us dla takiej liczby miast: " << N << "\n";
            break;
        }

        case 2:
        {
            cout << "\n";
            BreadthFirstSearch brfs(macierz, N);
            WynikBFS w = brfs.run(false, 300); 
            cout << "Czas wykonania: " << w.czas_us << " us dla takiej liczby miast: " << N << "\n";
            break;
        }

        default:
            cout << "Nieznany wybor!\n";
            break;}}



    void test() // fukcja licząca przerwania przy ograniczeniu 5 minut
    {
        vector<int> miastaTestowe = { 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };
        int tabliceTestowe = 100;
        int limitCzasowySekundy = 300; 

        cout << "N\tSredni Czas Best-FS [us]\tPrzerwane Best-FS [%]\tSredni Czas BFS [us]\tPrzerwane BFS [%]\n";
        cout << "--------------------------------------------------------------------------------------------------------\n";

        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> koszt(10, 99);

        for (int n : miastaTestowe)
        {
            long long sumaCzasuBest = 0;
            int przerwaneBest = 0;
            long long sumaCzasuBreadth = 0;
            int przerwaneBreadth = 0;

            for (int inst = 0; inst < tabliceTestowe; ++inst)
            {
                vector<vector<int>> tymczasowa_macierz(n, vector<int>(n));
                for (int i = 0; i < n; ++i)
                {
                    for (int j = 0; j < n; ++j)
                    {
                        if (i == j) tymczasowa_macierz[i][j] = -1;
                        else tymczasowa_macierz[i][j] = koszt(rng);
                    }
                }

                BestFirstSearch best(tymczasowa_macierz, n);
                WynikBest wBest = best.run(true, limitCzasowySekundy);
                if (wBest.przerwany) przerwaneBest++;
                else sumaCzasuBest += wBest.czas_us;

                BreadthFirstSearch breadth(tymczasowa_macierz, n);
                WynikBFS wBreadth = breadth.run(true, limitCzasowySekundy);
                if (wBreadth.przerwany) przerwaneBreadth++;
                else sumaCzasuBreadth += wBreadth.czas_us;
            }

            long long sredniaBest = (tabliceTestowe - przerwaneBest > 0) ? (sumaCzasuBest / (tabliceTestowe - przerwaneBest)) : -1;
            long long sredniaBreadth = (tabliceTestowe - przerwaneBreadth > 0) ? (sumaCzasuBreadth / (tabliceTestowe - przerwaneBreadth)) : -1;

            cout << n << "\t"
                << sredniaBest << "\t\t\t"
                << (double)przerwaneBest / tabliceTestowe * 100.0 << "%\t\t\t"
                << sredniaBreadth << "\t\t\t"
                << (double)przerwaneBreadth / tabliceTestowe * 100.0 << "%\n";
        }
    }
};



void pokazMenu()
{
    cout << "\nMENU:\n";
    cout << "1. Wczytaj dane z pliku\n";
    cout << "2. Wygeneruj losowe dane\n";
    cout << "3. Wyswietl ostatnio wygenerowane lub wczytane dane\n";
    cout << "4. Uruchom algorytm (B&B) dla obecnych danych\n";
    cout << "5. Wykonaj testy (pomiary czasu i procent przerwan)\n";
    cout << "0. Wyjdz\n";
    cout << "Wybierz opcje: ";
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
            cout << "Podaj nazwe pliku: ";
            cin >> plik_nazwa;
            if (manager.zPliku(plik_nazwa))
            {
                cout << "Dane wczytano pomyslnie.\n";
            }
            else
            {
                cout << "Niepoprawna nazwa pliku.\n";
            }
            break;
        }

        case 2:
        {
            int rozmiar;
            cout << "Podaj ilosc miast (N): ";
            cin >> rozmiar;
            manager.generujRandom(rozmiar);
            cout << "Wygenerowano losowe dane.\n";
            break;
        }

        case 3:
            manager.wyswietl();
            break;

        case 4:
            manager.uruchom();
            break;

        case 5:
            manager.test();
            break;

        case 0:
            break;

        default:
            cout << "\nWybierz jedna z dostepnych opcji\n" << endl;
        }
    } while (wybor != 0);


    return 0;
}