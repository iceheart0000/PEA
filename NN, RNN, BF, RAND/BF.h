#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <climits>
using namespace std;

class BruteForce
{
private:
    const vector<vector<int>>& macierz;
    int N;

public:
    BruteForce(const vector<vector<int>>& mat, int n) : macierz(mat), N(n) {}

    long long run(bool silent = false)
    {
        if (N <= 1) return LLONG_MAX;

        vector<int> biezaca_sciezka(N); // wektor kolejnosci odwiedzanych miast
        iota(biezaca_sciezka.begin(), biezaca_sciezka.end(), 0); 

        vector<int> najlepsza_sciezka = biezaca_sciezka;
        long long najlepszy_koszt = LLONG_MAX;

        do
        {
            long long biezacy_koszt = 0;
            bool valid = true;

            for (int i = 0; i < N - 1; ++i)
            {
                if (macierz[biezaca_sciezka[i]][biezaca_sciezka[i + 1]] == -1) // sprawdzenie czy istnieje krawędź
                {
                    valid = false; break;
                }
                biezacy_koszt += macierz[biezaca_sciezka[i]][biezaca_sciezka[i + 1]];
            }

            if (macierz[biezaca_sciezka[N - 1]][biezaca_sciezka[0]] == -1) valid = false; // sprawdzenie powrotu
            if (valid)
            {
                biezacy_koszt += macierz[biezaca_sciezka[N - 1]][biezaca_sciezka[0]]; 
                if (biezacy_koszt < najlepszy_koszt)
                {
                    najlepszy_koszt = biezacy_koszt;
                    najlepsza_sciezka = biezaca_sciezka;
                }
            }
        } while (next_permutation(biezaca_sciezka.begin() + 1, biezaca_sciezka.end()));

        if (!silent)
        {
            cout << "Brute Force\n";
            cout << "Najnizszy koszt (najlepszy): " << najlepszy_koszt << "\nSciezka: ";
            for (int miasto : najlepsza_sciezka) cout << miasto << " -> ";
            cout << najlepsza_sciezka[0] << "\n";
        }

        return najlepszy_koszt;
    }
};