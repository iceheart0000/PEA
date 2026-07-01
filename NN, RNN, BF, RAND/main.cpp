#include "BF.h"
#include "RANDOM.h"
#include "NN.h"
#include "RNN.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono> 

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
        uniform_int_distribution<int> koszt(1, 100);

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (i == j)
                {
                    macierz[i][j] = -1;
                }
                else
                {
                    macierz[i][j] = koszt(rng);
                }
            }
        }
    }


    void wyswietl() const
    {
        if (N < 1)
        {
            cout << "Brak danych do wyswietlenia\n"; return;
        }

        cout << "Liczba miast (N): " << N << "\nMacierz:\n";
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                cout << macierz[i][j] << "\t";
            }
            cout << "\n";
        }
    }


    void uruchom()
    {
        if (N == 0)
        {
            cout << "Liczba miast rowna zero. Brak danych do wyswietlenia." << endl;
            return;
        }

        int wybor_algorytmu;
        cout << "\n Wybierz algorytm:\n";
        cout << "1. Brute Force (BF)\n";
        cout << "2. Algorytm Losowy\n";
        cout << "3. Nearest Neighbor (NN)\n";
        cout << "4. Repetitive Nearest Neighbor (RNN)\n";
        cout << "Wybor: ";
        cin >> wybor_algorytmu;

        switch (wybor_algorytmu)
        {
        case 1:
        {
            cout << "\n";
            BruteForce bf(macierz, N);
            auto czas_poczatek = chrono::high_resolution_clock::now();
            bf.run();
            auto czas_koniec = chrono::high_resolution_clock::now();
            auto czas_uplyniety = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek);
            cout << "Czas wykonania: " << czas_uplyniety.count() << " us dla takiej liczby miast: " << N << "\n";
            break;
        }
        case 2:
        {
            int permutations;
            cout << "\nJaka ilosc permutacji: ";
            cin >> permutations;
            RandomSearch rs(macierz, N);
            auto czas_poczatek = chrono::high_resolution_clock::now();
            rs.run(permutations);
            auto czas_koniec = chrono::high_resolution_clock::now();
            auto czas_uplyniety = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek);
            cout << "Czas wykonania: " << czas_uplyniety.count() << " us dla takiej liczby miast: " << N << "\n";
            break;
        }
        case 3:
        {
            cout << "\n";
            NN nn(macierz, N);
            auto czas_poczatek = chrono::high_resolution_clock::now();
            nn.run();
            auto czas_koniec = chrono::high_resolution_clock::now();
            auto czas_uplyniety = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek);
            cout << "Czas wykonania: " << czas_uplyniety.count() << " us dla takiej liczby miast: " << N << "\n";
            break;
        }
        case 4:
        {
            cout << "\n";
            RNN rnn(macierz, N);
            auto czas_poczatek = chrono::high_resolution_clock::now();
            rnn.run();
            auto czas_koniec = chrono::high_resolution_clock::now();
            auto czas_uplyniety = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek);
            cout << "Czas wykonania: " << czas_uplyniety.count() << " us dla takiej liczby miast: " << N << "\n";
            break;
        }
        default:
            break;
        }
    }


    void test()
    {
        vector<int> miastaTestowe = { 10, 11, 12, 13, 14 };
        int tabliceTestowe = 100;

        cout << "N\tBlad NN [%]\tBlad RNN [%]\tBlad Losowy [%]\n";
        cout << "--------------------------------------------------------\n";

        random_device rd;
        mt19937 rng(rd());
        uniform_int_distribution<int> koszt(1, 100);

        for (int n : miastaTestowe)
        {
            double sumaBleduNN = 0.0;
            double sumaBleduRNN = 0.0;
            double sumaBleduRandom = 0.0;

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

                BruteForce bf(tymczasowa_macierz, n);
                NN nn(tymczasowa_macierz, n);
                RNN rnn(tymczasowa_macierz, n);
                RandomSearch rs(tymczasowa_macierz, n);

                long long optymalny_koszt = bf.run(true);
                long long nn_koszt = nn.run(true);
                long long rnn_koszt = rnn.run(true);
                long long random_koszt = rs.run(10 * n, true);

                if (optymalny_koszt > 0 && optymalny_koszt != LLONG_MAX)
                {
                    if (nn_koszt != LLONG_MAX) sumaBleduNN += ((double)(nn_koszt - optymalny_koszt) / optymalny_koszt) * 100.0;
                    if (rnn_koszt != LLONG_MAX) sumaBleduRNN += ((double)(rnn_koszt - optymalny_koszt) / optymalny_koszt) * 100.0;
                    if (random_koszt != LLONG_MAX) sumaBleduRandom += ((double)(random_koszt - optymalny_koszt) / optymalny_koszt) * 100.0;
                }
            }

            cout << n << "\t"
                << (sumaBleduNN / tabliceTestowe) << "\t\t"
                << (sumaBleduRNN / tabliceTestowe) << "\t\t"
                << (sumaBleduRandom / tabliceTestowe) << "\n";
        }
    }
};

void showMenu()
{
    cout << "MENU:\n";
    cout << "1. Wczytaj dane z pliku\n";
    cout << "2. Wygeneruj losowe dane\n";
    cout << "3. Wyswietl ostatnio wygenerowane lub wczytane dane\n";
    cout << "4. Uruchom wybrany algorytm dla ostatnio wygenerowanych lub wczytanych danych i wyswietl wyniki\n";
    cout << "5. Wykonaj testy (liczenie usrednionych bledow wzglednych) dla 100 macierzy dla miast = [10,11,12,13,14]\n";
    cout << "0. Wyjdz\n";
    cout << "Wybierz opcje: ";
}

int main()
{
    ATSP_Zarzadzaj manager;
    int wybor;

    do
    {
        showMenu();
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