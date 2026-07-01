#pragma once
#include <iostream>
#include <vector>
#include <climits>
using namespace std;

class RNN
{
private:
    const vector<vector<int>>& macierz;
    int N;

    void branch(int biezacy_wezel, int odwiedzone_licznik, long long biezacy_koszt,
        vector<bool>& odwiedzone, vector<int>& biezaca_sciezka,
        long long& najlepszy_koszt, vector<int>& najlepsza_sciezka, int start_wezel)
    {

        if (odwiedzone_licznik == N) // czy odwiedzone n miast
        {
            if (macierz[biezacy_wezel][start_wezel] != -1) // czy istnieje krawędź powrotna
            {
                long long koszt_calkowity = biezacy_koszt + macierz[biezacy_wezel][start_wezel]; // doliczenie kosztu powrotu
                if (koszt_calkowity < najlepszy_koszt)
                {
                    najlepszy_koszt = koszt_calkowity;
                    najlepsza_sciezka = biezaca_sciezka;
                }
            }
            return;
        }

        int min_koszt = INT_MAX;
        for (int i = 0; i < N; ++i)
        {
            if (!odwiedzone[i] && macierz[biezacy_wezel][i] != -1 && macierz[biezacy_wezel][i] < min_koszt)
            {
                min_koszt = macierz[biezacy_wezel][i];
            }
        }

        if (min_koszt == INT_MAX) return;
        // branching 
        for (int i = 0; i < N; ++i)
        {
            if (!odwiedzone[i] && macierz[biezacy_wezel][i] == min_koszt) // jesli nieodwiedzona i koszt taki sam jak najtanszy
            {
                // wejście do wybranego miasta
                odwiedzone[i] = true;
                biezaca_sciezka.push_back(i);

                branch(i, odwiedzone_licznik + 1, biezacy_koszt + min_koszt, odwiedzone, biezaca_sciezka, najlepszy_koszt, najlepsza_sciezka, start_wezel);

                biezaca_sciezka.pop_back(); // backtracking
                odwiedzone[i] = false;
            }
        }
    }

public:
    RNN(const vector<vector<int>>& mat, int n) : macierz(mat), N(n) {}

    long long run(bool silent = false)
    {
        if (N <= 1) return LLONG_MAX;

        long long globalny_najlepszy_koszt = LLONG_MAX;
        vector<int> globalna_najlepsza_sciezka;


        for (int start_wezel = 0; start_wezel < N; ++start_wezel) // start po kolei z kazdego miasta
        {
            vector<bool> odwiedzone(N, false);
            vector<int> biezaca_sciezka;
            long long biezacy_koszt = 0;
            long long lokalny_najlepszy_koszt = LLONG_MAX;
            vector<int> lokalna_najlepsza_sciezka;

            odwiedzone[start_wezel] = true; // miasto startowe odwiedzone
            biezaca_sciezka.push_back(start_wezel);

            branch(start_wezel, 1, biezacy_koszt, odwiedzone, biezaca_sciezka, lokalny_najlepszy_koszt, lokalna_najlepsza_sciezka, start_wezel);

            if (lokalny_najlepszy_koszt < globalny_najlepszy_koszt)
            {
                globalny_najlepszy_koszt = lokalny_najlepszy_koszt;
                globalna_najlepsza_sciezka = lokalna_najlepsza_sciezka;
            }
        }

        if (!silent)
        {
            cout << "Repetitive Nearest Neighbor - RNN \n";
            if (globalny_najlepszy_koszt == LLONG_MAX)
            {
                cout << "Nie znaleziono poprawnej sciezki\n";
            }
            else
            {
                cout << "Najlepszy koszt: " << globalny_najlepszy_koszt << "\nSciezka: ";
                for (int miasto : globalna_najlepsza_sciezka) cout << miasto << " -> ";
                cout << globalna_najlepsza_sciezka[0] << "\n";
            }
        }

        return globalny_najlepszy_koszt;
    }
};