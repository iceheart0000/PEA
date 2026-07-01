#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <climits>
#include <cmath>
#include <random>

using namespace std;


struct WynikSW
{
    long long koszt, czas_us;
    vector<int> sciezka;
    bool przerwany;
};

class SymulowaneWyzarzanie
{
private:
    int N;
    vector<vector<int>> macierz;
    double T0;               
    double T_end;            
    double alpha;            // wsp chlodzenia dla sch geo
    double beta;             // krok chlodzenia dla sch liniowego
    int epoka;               // liczba iteracji w stalej temperaturze
    int schemat_chlodzenia;  
    int start_roz;           
    int limit_czasu_s;       




    long long obliczKoszt(const vector<int>& sciezka)
    {
        long long koszt = 0;
        for (size_t i = 0; i < sciezka.size() - 1; ++i)
        {
            if (macierz[sciezka[i]][sciezka[i + 1]] == -1) return LLONG_MAX;
            koszt += macierz[sciezka[i]][sciezka[i + 1]];
        }
        if (macierz[sciezka.back()][sciezka[0]] == -1) return LLONG_MAX; // sprawdzenie powrotu
        koszt += macierz[sciezka.back()][sciezka[0]];
        return koszt;
    }



    vector<int> rozwiazanieLosowe(mt19937& rng)
    {
        vector<int> sciezka(N);
        for (int i = 0; i < N; ++i) sciezka[i] = i;
        shuffle(sciezka.begin() + 1, sciezka.end(), rng);
        return sciezka;
    }




    vector<int> rozwiazanieZachlanne()
    {
        vector<int> sciezka;
        sciezka.push_back(0);
        vector<bool> odwiedzone(N, false);
        odwiedzone[0] = true;
        int obecny = 0;
        for (int i = 1; i < N; ++i)
        {
            int najblizszy = -1;
            int min_koszt = INT_MAX;
            for (int j = 0; j < N; ++j)
            {
                if (!odwiedzone[j] && macierz[obecny][j] != -1 && macierz[obecny][j] < min_koszt)
                {
                    min_koszt = macierz[obecny][j];
                    najblizszy = j;
                }
            }
            if (najblizszy == -1) return sciezka; 
            odwiedzone[najblizszy] = true;
            obecny = najblizszy;
            sciezka.push_back(najblizszy);
        }
        return sciezka;
    }



    
    vector<int> generujSasiada(const vector<int>& obecna, mt19937& rng)
    {
        vector<int> sasiad = obecna;
        uniform_int_distribution<int> dist(1, N - 1); 
        int i = dist(rng);
        int j = dist(rng);
        while (i == j)
        {
            j = dist(rng);
        }
        swap(sasiad[i], sasiad[j]); // stworzenie nowej trasy przez zeswapowanie
        return sasiad;
    }



public:
    SymulowaneWyzarzanie(const vector<vector<int>>& mat, int n) : macierz(mat), N(n)
    {
        // domyœlne parametry
        T0 = 1000.0;
        T_end = 0.001;
        alpha = 0.99;
        beta = 1.0;
        epoka = N * 10;
        schemat_chlodzenia = 1; // 1 - geometryczny, 2 - linowy
        start_roz = 2;          // 1 - losowe, 2 - nn
        limit_czasu_s = 900;    
    }



    void ustawParametry(double t0, double alf, double bet, int epok, int schemat, int start, int limit)
    {
        T0 = t0;
        alpha = alf;
        beta = bet;
        epoka = epok;
        schemat_chlodzenia = schemat;
        start_roz = start;
        limit_czasu_s = limit;
    }



    WynikSW run(bool silent = false)
    {
        WynikSW wynik;
        wynik.przerwany = false;
        if (N <= 1) { wynik.koszt = LLONG_MAX; return wynik; }

        auto czas_poczatek = chrono::high_resolution_clock::now();
        random_device rd;
        mt19937 rng(rd());
        uniform_real_distribution<double> dist_prob(0.0, 1.0);

        vector<int> obecna_sciezka;
        // inicjalizacja losowej lub nn
        if (start_roz == 1) obecna_sciezka = rozwiazanieLosowe(rng);
        else obecna_sciezka = rozwiazanieZachlanne();

        long long obecny_koszt = obliczKoszt(obecna_sciezka);
        vector<int> najlepsza_sciezka = obecna_sciezka;
        long long najlepszy_koszt = obecny_koszt;

        double T = T0;



        while (T > T_end)
        {
            auto teraz = chrono::high_resolution_clock::now();
            if (chrono::duration_cast<chrono::seconds>(teraz - czas_poczatek).count() >= limit_czasu_s) { wynik.przerwany = true; break; }
      


            for (int i = 0; i < epoka; ++i) // ile losowan nowych tras przy stalej temp przed obnizeniem temp
            {
                vector<int> sasiad = generujSasiada(obecna_sciezka, rng);
                long long koszt_sasiada = obliczKoszt(sasiad);
                if (koszt_sasiada < obecny_koszt)
                {
                    obecna_sciezka = sasiad; obecny_koszt = koszt_sasiada;
                    if (obecny_koszt < najlepszy_koszt)
                    {
                        najlepszy_koszt = obecny_koszt; najlepsza_sciezka = obecna_sciezka;
                    }}




                else
                {
                    double delta = koszt_sasiada - obecny_koszt;
                    double prawdopodobienstwo = exp(-delta / T);
                    if (dist_prob(rng) < prawdopodobienstwo) // rng tj. <0;1)
                    {
                        obecna_sciezka = sasiad;
                        obecny_koszt = koszt_sasiada;
                    }
                }
            }

            // chlodzenie
            if (schemat_chlodzenia == 1) 
            {
                T *= alpha;
            }
            else if (schemat_chlodzenia == 2) 
            {
                T -= beta;
                if (T <= 0) T = T_end; 
            }
        }

        auto czas_koniec = chrono::high_resolution_clock::now();
        wynik.czas_us = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek).count();
        wynik.koszt = najlepszy_koszt;
        wynik.sciezka = najlepsza_sciezka;

        if (!silent)
        {
            cout << "\n--- Symulowane Wyzarzanie (SW) ---\n";
            if (wynik.przerwany) cout << "Przerwano dzialanie: przekroczono limit czasu (" << limit_czasu_s << "s)\n";
            cout << "Najnizszy koszt: " << wynik.koszt << "\nSciezka: ";
            for (int miasto : wynik.sciezka) cout << miasto << " -> ";
            if (!wynik.sciezka.empty()) cout << wynik.sciezka[0] << "\n";
            cout << "Czas wykonania: " << wynik.czas_us << " us\n";
        }

        return wynik;
    }
};