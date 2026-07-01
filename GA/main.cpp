#include "ga.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <iomanip>
using namespace std;

struct InstancjaTestowa {
    string nazwa;
    int optymalny;
};



class TSP_Zarzadzaj
{
private:
    int N;
    vector<vector<int>> macierz;
    string aktualny_plik = "brak";
    int limit_czasowy_s = 900; 
    int pop_size = 100;
    double wsp_mutacji = 0.05;
    double wsp_krzyzowania = 0.8;


public:
    TSP_Zarzadzaj() : N(0) {}



    bool zPliku(const string& nazwa_pliku)
    {
        ifstream plik(nazwa_pliku);
        if (!plik.is_open()) return false;
        string pierwsze_slowo;
        if (!(plik >> pierwsze_slowo)) return false;

        aktualny_plik = nazwa_pliku;

        if (isdigit(pierwsze_slowo[0]))
        {
            N = stoi(pierwsze_slowo);
            macierz.assign(N, vector<int>(N));
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                {
                    plik >> macierz[i][j];
                    if (i == j) macierz[i][j] = -1;
                }
        }
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
                    else plik >> N;
                }
                else if (slowo == "EDGE_WEIGHT_SECTION") break;
            } while (plik >> slowo);

            if (N <= 0) return false;
            macierz.assign(N, vector<int>(N));
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                {
                    plik >> macierz[i][j];
                    if (macierz[i][j] >= 100000000 || i == j) macierz[i][j] = -1;
                }
        }
        plik.close();
        return true;
    }



    void wyswietlDane()
    {
        if (N == 0) { cout << "Brak danych do wyswietlenia.\n"; return; }
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (macierz[i][j] == -1) cout << setw(5) << "-1";
                else cout << setw(5) << macierz[i][j];
            }
            cout << "\n";
        }
        cout << "\n";
    }



    void ustawKryteriumStopu() { cout << "Obecny limit to " << limit_czasowy_s << "s.\nPodaj nowy czas: "; cin >> limit_czasowy_s; }
    void ustawPopulacje() { cout << "Obecna populacja to " << pop_size << ".\nPodaj wielkosc: "; cin >> pop_size; }
    void ustawMutacjeRate() { cout << "Obecny wsp. mutacji to " << wsp_mutacji << ".\nPodaj wartosc: "; cin >> wsp_mutacji; }
    void ustawKrzyzowanieRate() { cout << "Obecny wsp. krzyzowania to " << wsp_krzyzowania << ".\nPodaj wartosc: "; cin >> wsp_krzyzowania; }



    void uruchomAlgorytm()
    {
        if (N == 0) { cout << "Brak danych. Wczytaj plik.\n"; return; }

        unordered_map<string, int> baza_optymalnych = {
            {"br17", 39}, {"ft53", 6905}, {"ft70", 38673}, {"ftv33", 1286},
            {"ftv35", 1473}, {"ftv38", 1530}, {"ftv44", 1613}, {"ftv47", 1776},
            {"ftv55", 1608}, {"ftv64", 1839}, {"ftv70", 1950}, {"ftv90", 1579},
            {"ftv100", 1788}, {"ftv110", 1958}, {"ftv120", 2166}, {"ftv130", 2307},
            {"ftv140", 2420}, {"ftv150", 2611}, {"ftv160", 2683}, {"ftv170", 2755},
            {"kro124p", 36230}, {"p43", 5620}, {"rbg323", 1326}, {"rbg358", 1163},
            {"rbg403", 2465}, {"rbg443", 2720}, {"ry48p", 14422}
        };
        string nazwa_bazowa = aktualny_plik;
        size_t pos = nazwa_bazowa.find(".atsp");
        if (pos != string::npos)
        {
            nazwa_bazowa = nazwa_bazowa.substr(0, pos);
        }

        int optymalny = 0;
        if (baza_optymalnych.count(nazwa_bazowa)) { optymalny = baza_optymalnych[nazwa_bazowa]; }

        AlgorytmGenetyczny ga(macierz, N);
        ga.ustawParametry(pop_size, wsp_mutacji, wsp_krzyzowania, limit_czasowy_s);
        WynikGA wynik = ga.run();

        cout << "\nWyniki:\n";
        cout << "Najlepsza dlugosc: " << wynik.koszt << "\n";
        cout << "Srednia dlugosc ost. populacji: " << fixed << setprecision(2) << wynik.sredni_koszt_populacji << "\n";

        if (optymalny > 0)
        {
            double blad_najlepszy = ((wynik.koszt - optymalny) / (double)optymalny) * 100.0;
            double blad_sredni = ((wynik.sredni_koszt_populacji - optymalny) / (double)optymalny) * 100.0;
            cout << "Blad wzgledny najlepszego rozw.: " << fixed << setprecision(3) << blad_najlepszy << "%\n";
            cout << "Blad wzgledny sredniej pop.: " << fixed << setprecision(3) << blad_sredni << "%\n";
        }
        else { cout << "Blad wzgledny: Nieznany\n"; }
        cout << "Czas znalezienia rozwiazania: " << wynik.czas_najlepszego_us / 1000.0 << " ms\n";
    }



    void uruchomTesty3_0()
    {
        vector<InstancjaTestowa> pliki =
        {
            {"br17.atsp", 39}, {"ftv33.atsp", 1286}, {"ftv35.atsp", 1473},
            {"ftv38.atsp", 1530}, {"ftv44.atsp", 1613}, {"ftv47.atsp", 1776},
            {"ftv55.atsp", 1608}, {"ftv64.atsp", 1839}, {"ftv70.atsp", 1950},
            {"kro124p.atsp", 36230}
        };
        ofstream plik_zapis("wyniki_3_0.csv");
        cout << "\nTest GA: Rozmiar instancji\n";
        plik_zapis << "Plik;N;Populacja;Najlepszy_Koszt;Blad_Procentowy;Sredni_Koszt_Pop;Czas_Znalezienia_ms\n";
        for (const auto& plik : pliki)
        {
            if (!zPliku(plik.nazwa)){ cout << plik.nazwa << " -> Blad: Brak pliku\n"; continue; }
            AlgorytmGenetyczny ga(macierz, N);
            ga.ustawParametry(100, wsp_mutacji, wsp_krzyzowania, 900);
            WynikGA w = ga.run();
            double blad_proc = ((double)(w.koszt - plik.optymalny) / plik.optymalny) * 100.0;
            plik_zapis << plik.nazwa << ";" << N << ";100;"
                << w.koszt << ";" << fixed << setprecision(3) << blad_proc << ";"
                << w.sredni_koszt_populacji << ";" << w.czas_najlepszego_us / 1000.0 << "\n";
            cout << "    Zakonczono. Blad: " << fixed << setprecision(3) << blad_proc << "%\n";
        }
        plik_zapis.close();
    }



    void uruchomTesty3_5()
    {
        ofstream plik_zapis("wyniki_3_5.csv");
        cout << "\n--- Test GA: Wplyw populacji ---\n";
        plik_zapis << "Plik;N;Populacja;Najlepszy_Koszt;Blad_Procentowy;Sredni_Koszt_Pop;Czas_Znalezienia_ms\n";
        if (zPliku("ftv70.atsp"))
        {
            vector<int> testowane_populacje = { 10, 50, 100, 200, 400 };
            for (int pop : testowane_populacje) {
                AlgorytmGenetyczny ga(macierz, N);
                ga.ustawParametry(pop, wsp_mutacji, wsp_krzyzowania, 900);
                WynikGA w = ga.run();
                double blad_proc = ((double)(w.koszt - 1950) / 1950) * 100.0;
                plik_zapis << "ftv70.atsp;" << N << ";" << pop << ";"
                    << w.koszt << ";" << fixed << setprecision(3) << blad_proc << ";"
                    << w.sredni_koszt_populacji << ";" << w.czas_najlepszego_us / 1000.0 << "\n";
                cout << "    Zakonczono. Blad: " << fixed << setprecision(3) << blad_proc << "%\n";
            }
        }
        else { cout << "Blad: Nie znaleziono pliku ftv70.atsp\n"; }
        plik_zapis.close();
    }
};



void pokazMenu()
{
    cout << "\nMENU:\n";
    cout << "1. Wczytanie danych z pliku i wyswietlenie\n";
    cout << "2. Wprowadzenie kryterium stopu\n";
    cout << "3. Ustawienie populacji\n";
    cout << "4. Ustawienie mutacji\n";
    cout << "5. Ustawienie krzyzowania\n";
    cout << "6. Uruchom algorytm pojedynczo\n";
    cout << "7. Test automatyczny - zaleznosc od N\n";
    cout << "8. Test automatyczny - zaleznosc od populacji\n";
    cout << "0. Wyjdz\n";
    cout << "\n";
    cout << "Podaj wybor: ";
}



int main()
{
    TSP_Zarzadzaj manager;
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
            cout << "Nazwa pliku: "; cin >> plik_nazwa;
            if (manager.zPliku(plik_nazwa)) {  cout << "Wczytano pomyslnie.\n"; manager.wyswietlDane(); }
            break;
        }
        case 2: manager.ustawKryteriumStopu(); break;
        case 3: manager.ustawPopulacje(); break;
        case 4: manager.ustawMutacjeRate(); break;
        case 5: manager.ustawKrzyzowanieRate(); break;
        case 6: manager.uruchomAlgorytm(); break;
        case 7: manager.uruchomTesty3_0(); break;
        case 8: manager.uruchomTesty3_5(); break;
        case 0: break;
        }
    } while (wybor != 0);
    return 0;
}