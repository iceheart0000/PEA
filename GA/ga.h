#ifndef GA_H
#define GA_H
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include <climits>
#include <iostream>
using namespace std;

struct WynikGA {
    int koszt = 0;
    double sredni_koszt_populacji = 0.0;
    long long czas_us = 0;
    long long czas_najlepszego_us = 0;
    bool przerwany = false;
};

struct Osobnik {
    std::vector<int> sciezka;
    int koszt = 0;
    bool operator<(const Osobnik& inny) const { return koszt < inny.koszt; }
};

class AlgorytmGenetyczny {
private:
    std::vector<std::vector<int>> macierz;
    int N;
    int pop_size;
    double wsp_mutacji;
    double wsp_krzyzowania;
    int limit_czasowy_s;
    mt19937 rng;

    int obliczKoszt(const std::vector<int>& sciezka)
    {
        int k = 0;
        for (size_t i = 0; i < sciezka.size() - 1; ++i) 
        {
            k += macierz[sciezka[i]][sciezka[i + 1]];
        }
        k += macierz[sciezka.back()][sciezka[0]];
        return k;
    }

public:
    AlgorytmGenetyczny(const std::vector<std::vector<int>>& m, int n) : macierz(m), N(n) { random_device rd; rng.seed(rd()); }
    void ustawParametry(int pop, double mut, double krzyz, int limit) { pop_size = pop; wsp_mutacji = mut; wsp_krzyzowania = krzyz; limit_czasowy_s = limit; }


    WynikGA run()
    {
        WynikGA wynik;
        auto czas_start = std::chrono::high_resolution_clock::now();
        vector<Osobnik> populacja(pop_size);
        vector<int> baza(N);
        iota(baza.begin(), baza.end(), 0);
        int najlepszy_koszt = INT_MAX;
        //tworzenie losowych sciezek (liczba sciezek = liczba populacji)
        for (int i = 0; i < pop_size; ++i)
        {
            populacja[i].sciezka = baza;
            std::shuffle(populacja[i].sciezka.begin(), populacja[i].sciezka.end(), rng);
            populacja[i].koszt = obliczKoszt(populacja[i].sciezka);
            if (populacja[i].koszt < najlepszy_koszt) { najlepszy_koszt = populacja[i].koszt; }
        }
        
        // najmn. koszt = elitaryzm


        while (true)
        {
            auto czas_teraz = std::chrono::high_resolution_clock::now();
            auto czas_trwania_us = std::chrono::duration_cast<std::chrono::microseconds>(czas_teraz - czas_start).count();
            if ((czas_trwania_us / 1000000.0) >= limit_czasowy_s) break;
            

            vector<Osobnik> nowa_populacja; // wektor nowej populacji
            auto best_it = std::min_element(populacja.begin(), populacja.end()); 
            nowa_populacja.push_back(*best_it); // wrzucamy najlepsz¹ sciezkê ze starego pokolenia


            uniform_real_distribution<double> dist_01(0.0, 1.0);
            uniform_int_distribution<int> dist_idx(0, N - 1);
            uniform_int_distribution<int> dist_pop(0, pop_size - 1);


            while (nowa_populacja.size() < pop_size)
            {   // selekcja
                // losowanie dwoch rodzicow z poprzedniego genu, po dwóchkandydatow na jednego rodzica
                Osobnik rodzic1 = populacja[dist_pop(rng)]; Osobnik kandydat1 = populacja[dist_pop(rng)];
                if (kandydat1.koszt < rodzic1.koszt) rodzic1 = kandydat1;
                Osobnik rodzic2 = populacja[dist_pop(rng)];
                Osobnik kandydat2 = populacja[dist_pop(rng)];
                if (kandydat2.koszt < rodzic2.koszt) rodzic2 = kandydat2;



                Osobnik dziecko = rodzic1;
                if (dist_01(rng) < wsp_krzyzowania) 
                {
                    int start = dist_idx(rng); int end = dist_idx(rng);
                    if (start > end) std::swap(start, end); // podmiana dwoch indeksow 


                    vector<int> child_path(N, -1); // stworzenie wektora dziecka i wypelnienie -1
                    vector<bool> used(N, false);
                    for (int k = start; k <= end; ++k)
                    {
                        child_path[k] = rodzic1.sciezka[k]; // kopiowanie podmienionych wartosci z indeksow rodzica1 do wektora dziecka 
                        used[rodzic1.sciezka[k]] = true;
                    }
                    int curr = 0;
                    for (int k = 0; k < N; ++k)
                    {
                        if (child_path[k] == -1) // uzupelniamy pozosta³e indeksy wartosciami z rodzica2
                        {
                            while (used[rodzic2.sciezka[curr]]) curr++;
                            child_path[k] = rodzic2.sciezka[curr];
                            used[rodzic2.sciezka[curr]] = true;
                        }
                    }
                    dziecko.sciezka = child_path;
                }




                if (dist_01(rng) < wsp_mutacji)
                { // losowanie dwoch indeksow na nowej trasie dziecka i podmiana
                    int idx1 = dist_idx(rng);
                    int idx2 = dist_idx(rng);
                    swap(dziecko.sciezka[idx1], dziecko.sciezka[idx2]);
                }

                dziecko.koszt = obliczKoszt(dziecko.sciezka); // obliczenie i porownanie kosztu sciezki dziecka po swapie
                if (dziecko.koszt < najlepszy_koszt)
                {
                    najlepszy_koszt = dziecko.koszt;
                    wynik.czas_najlepszego_us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - czas_start).count();
                    
                }
                nowa_populacja.push_back(dziecko);
            }
            populacja = nowa_populacja;
        }

        auto czas_koniec = std::chrono::high_resolution_clock::now();
        wynik.czas_us = std::chrono::duration_cast<std::chrono::microseconds>(czas_koniec - czas_start).count();
        wynik.koszt = najlepszy_koszt;

        double suma_kosztow = 0;
        for (const auto& o : populacja) suma_kosztow += o.koszt;
        wynik.sredni_koszt_populacji = suma_kosztow / pop_size;

        if (wynik.czas_najlepszego_us == 0) wynik.czas_najlepszego_us = wynik.czas_us;

        return wynik;
    }
};

#endif