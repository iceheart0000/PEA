#pragma once
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class NN
{
private:
    const vector<vector<int>>& macierz;
    int N;

public:
    NN(const vector<vector<int>>& mat, int n) : macierz(mat), N(n) {}

    long long run(bool silent = false)
    {
        if (N <= 1) return LLONG_MAX;

        vector<bool> odwiedzone(N, false); // zapobiega powrotowi do odwiedzonego miasta
        vector<int> biezaca_sciezka;
        long long biezacy_koszt = 0;

        int biezacy_wezel = 0; // poczatek w mieście 0
        odwiedzone[biezacy_wezel] = true;
        biezaca_sciezka.push_back(biezacy_wezel);
        bool sciezka_zakonczona = true;

        for (int krok = 1; krok < N; ++krok) // n-1 krokow 
        {
            int min_koszt = INT_MAX;
            int nastepny_wezel = -1;

            for (int i = 0; i < N; ++i) // przeglad wszystkich pozostalych sasiadow
            {
                if (!odwiedzone[i] && macierz[biezacy_wezel][i] != -1 && macierz[biezacy_wezel][i] < min_koszt) 
                {
                    min_koszt = macierz[biezacy_wezel][i]; // min koszt to waga tej krawedzi
                    nastepny_wezel = i; 
                }
            }

            if (nastepny_wezel == -1)
            {
                sciezka_zakonczona = false;
                break;
            }

            odwiedzone[nastepny_wezel] = true;
            biezaca_sciezka.push_back(nastepny_wezel); // miasto trafia na koniec trasy
            biezacy_koszt += min_koszt; // dodanie kosztu krawedzi
            biezacy_wezel = nastepny_wezel;
        }

        if (sciezka_zakonczona && macierz[biezacy_wezel][0] != -1) // dokończenie cyklu
        {
            biezacy_koszt += macierz[biezacy_wezel][0];
        }
        else
        {
            sciezka_zakonczona = false;
        }

        if (!silent)
        {
            cout << "Nearest Neighbor - NN\n";
            if (!sciezka_zakonczona) cout << "Nie znaleziono poprawnej sciezki.\n";
            else
            {
                cout << "Najlepszy koszt: " << biezacy_koszt << "\nSciezka: ";
                for (int miasto : biezaca_sciezka) cout << miasto << " -> ";
                cout << biezaca_sciezka[0] << "\n";
            }
        }

        return sciezka_zakonczona ? biezacy_koszt : LLONG_MAX;
    }
};