#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <climits>
using namespace std;

class RandomSearch
{
private:
    const vector<vector<int>>& macierz;
    int N;

public:
    RandomSearch(const vector<vector<int>>& mat, int n) : macierz(mat), N(n) {}

    long long run(int permutacje, bool silent = false)
    {
        if (N <= 1) return LLONG_MAX;

        vector<int> biezaca_sciezka(N);
        iota(biezaca_sciezka.begin(), biezaca_sciezka.end(), 0);

        vector<int> najlepsza_sciezka = biezaca_sciezka;
        long long najlepszy_koszt = LLONG_MAX;

        random_device rd;
        mt19937 rng(rd());

        for (int p = 0; p < permutacje; ++p) // dla zadanej liczby permutacji
        {
            shuffle(biezaca_sciezka.begin() + 1, biezaca_sciezka.end(), rng); // tasowanie miast w wektorze

            long long biezacy_koszt = 0;
            bool valid = true;

            
            for (int i = 0; i < N - 1; ++i) // pętla po wylosowanej trasie
            {
                if (macierz[biezaca_sciezka[i]][biezaca_sciezka[i + 1]] == -1) // czy istnieje trasa między dwoma kolejnymi miastami
                {
                    valid = false; break;
                }
                biezacy_koszt += macierz[biezaca_sciezka[i]][biezaca_sciezka[i + 1]];
            }


            if (valid && macierz[biezaca_sciezka[N - 1]][biezaca_sciezka[0]] != -1) // zamkniecie cyklu
            {
                biezacy_koszt += macierz[biezaca_sciezka[N - 1]][biezaca_sciezka[0]];
                if (biezacy_koszt < najlepszy_koszt)
                {
                    najlepszy_koszt = biezacy_koszt;
                    najlepsza_sciezka = biezaca_sciezka;
                }
            }
        }

        if (!silent)
        {
            cout << "Algorytm Losowy\n";
            cout << "Najlepszy koszt: " << najlepszy_koszt << "\nSciezka: ";
            for (int miasto : najlepsza_sciezka) cout << miasto << " -> ";
            cout << najlepsza_sciezka[0] << "\n";
        }

        return najlepszy_koszt;
    }
};