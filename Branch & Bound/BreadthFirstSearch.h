#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <chrono>
using namespace std;

struct WezelBFS
{
    vector<int> sciezka;
    vector<vector<int>> macierz;
    long long dolne_ograniczenie;
    int poziom;
    int wierzcholek;
};

struct WynikBFS
{
    long long koszt;
    vector<int> sciezka;
    long long czas_us;
    bool przerwany;
};

class BreadthFirstSearch
{
private:
    int N;
    vector<vector<int>> macierz_poczatkowa;

    long long redukujMacierz(vector<vector<int>>& m)
    {
        long long koszt_redukcji = 0;
        for (int i = 0; i < N; ++i) 
        {
            int min_val = INT_MAX;
            for (int j = 0; j < N; ++j)
            {
                if (m[i][j] != -1 && m[i][j] < min_val)
                {
                    min_val = m[i][j];
                }
            }
            if (min_val != INT_MAX && min_val > 0)
            {
                koszt_redukcji += min_val;
                for (int j = 0; j < N; ++j)
                {
                    if (m[i][j] != -1) m[i][j] -= min_val;
                }}}
        

        for (int j = 0; j < N; ++j) 
        {
            int min_val = INT_MAX;
            for (int i = 0; i < N; ++i)
            {
                if (m[i][j] != -1 && m[i][j] < min_val)
                {
                    min_val = m[i][j];
                }
            }
            if (min_val != INT_MAX && min_val > 0)
            {
                koszt_redukcji += min_val;
                for (int i = 0; i < N; ++i)
                {
                    if (m[i][j] != -1) m[i][j] -= min_val;
                }}}
        return koszt_redukcji;
    }

     

    long long obliczPoczatkowyKoszt(vector<int>& poczatkowa_sciezka) 
    {
        vector<bool> odwiedzone(N, false);
        long long koszt = 0;
        int obecny = 0;
        odwiedzone[0] = true;
        poczatkowa_sciezka.push_back(0);

        for (int i = 1; i < N; ++i)
        {
            int najblizszy = -1;
            int min_koszt = INT_MAX;
            for (int j = 0; j < N; ++j)
            {
                if (!odwiedzone[j] && macierz_poczatkowa[obecny][j] != -1 && macierz_poczatkowa[obecny][j] < min_koszt)
                {
                    min_koszt = macierz_poczatkowa[obecny][j];
                    najblizszy = j;
                }
            }
            if (najblizszy == -1) return LLONG_MAX;
            koszt += min_koszt;
            odwiedzone[najblizszy] = true;
            obecny = najblizszy;
            poczatkowa_sciezka.push_back(najblizszy);
        }
        if (macierz_poczatkowa[obecny][0] == -1) return LLONG_MAX;
        koszt += macierz_poczatkowa[obecny][0];
        return koszt;
    }

public:
    BreadthFirstSearch(const vector<vector<int>>& mat, int n) : macierz_poczatkowa(mat), N(n) {}

    WynikBFS run(bool silent = false, int limit_czasu_s = 300)
    {
        WynikBFS wynik;
        wynik.przerwany = false;
        if (N <= 1)
        {
            wynik.koszt = LLONG_MAX;
            return wynik;
        }

        auto czas_poczatek = chrono::high_resolution_clock::now();

        long long najlepszy_koszt = LLONG_MAX;
        vector<int> najlepsza_sciezka;
        najlepszy_koszt = obliczPoczatkowyKoszt(najlepsza_sciezka); // ub


        queue<WezelBFS> q; // fifo


        WezelBFS korzen;
        korzen.macierz = macierz_poczatkowa;
        korzen.sciezka.push_back(0);
        korzen.poziom = 0;
        korzen.wierzcholek = 0;
        korzen.dolne_ograniczenie = redukujMacierz(korzen.macierz); // lb

        q.push(korzen);

        while (!q.empty())
        {
            auto teraz = chrono::high_resolution_clock::now();
            if (chrono::duration_cast<chrono::seconds>(teraz - czas_poczatek).count() >= limit_czasu_s)
            {
                wynik.przerwany = true;
                break;
            }

            WezelBFS obecny = q.front(); // bierzemy z poczatku
            q.pop();

            if (obecny.dolne_ograniczenie >= najlepszy_koszt) continue;

            if (obecny.poziom == N - 1)
            {
                
                long long aktualny_koszt = 0;
                bool valid = true;
                // sumowanie kosztów po calej œcie¿ce
                for (int i = 0; i < N - 1; ++i)
                {
                    int u = obecny.sciezka[i];
                    int v = obecny.sciezka[i + 1];
                    if (macierz_poczatkowa[u][v] == -1) { valid = false; break; }
                    aktualny_koszt += macierz_poczatkowa[u][v];
                }
                int powrot_u = obecny.sciezka.back();
                int powrot_v = obecny.sciezka[0];
                if (macierz_poczatkowa[powrot_u][powrot_v] == -1) valid = false;
                else aktualny_koszt += macierz_poczatkowa[powrot_u][powrot_v];

                if (valid && aktualny_koszt < najlepszy_koszt)
                {
                    najlepszy_koszt = aktualny_koszt;
                    najlepsza_sciezka = obecny.sciezka;
                }
                continue;
            }

            for (int v = 0; v < N; ++v) 
            {
                if (obecny.macierz[obecny.wierzcholek][v] != -1) // do ktorych miast mozna pojechac
                {// tworzenie dzieci
                    WezelBFS dziecko;
                    dziecko.sciezka = obecny.sciezka;
                    dziecko.sciezka.push_back(v);
                    dziecko.poziom = obecny.poziom + 1;
                    dziecko.wierzcholek = v;
                    dziecko.macierz = obecny.macierz;

                    int koszt_krawedzi = dziecko.macierz[obecny.wierzcholek][v];

                    for (int i = 0; i < N; ++i) dziecko.macierz[obecny.wierzcholek][i] = -1;
                    for (int i = 0; i < N; ++i) dziecko.macierz[i][v] = -1;
                    dziecko.macierz[v][0] = -1;

                    long long koszt_redukcji = redukujMacierz(dziecko.macierz); // redukcja
                    dziecko.dolne_ograniczenie = obecny.dolne_ograniczenie + koszt_krawedzi + koszt_redukcji;

                    if (dziecko.dolne_ograniczenie < najlepszy_koszt)
                    {
                        q.push(dziecko); // na koniec kolejki
                    }
                }
            }
        }

        auto czas_koniec = chrono::high_resolution_clock::now();
        wynik.czas_us = chrono::duration_cast<chrono::microseconds>(czas_koniec - czas_poczatek).count();
        wynik.koszt = najlepszy_koszt;
        wynik.sciezka = najlepsza_sciezka;

        if (!silent)
        {
            cout << "Branch & Bound (Breadth-First-Search)\n";
            if (wynik.przerwany) cout << "przerwano dzialanie z podowodu przekroczenia limitu czasowego (" << limit_czasu_s << "s)\n";
            cout << "Najnizszy koszt (najlepszy): " << wynik.koszt << "\nSciezka: ";
            for (int miasto : wynik.sciezka) cout << miasto << " -> ";
            if (!wynik.sciezka.empty()) cout << wynik.sciezka[0] << "\n";
        }

        return wynik;
    }
};