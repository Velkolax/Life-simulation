#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int xMacierzy = 0;
int yMacierzy = 0;
float* macierz = NULL;

typedef struct
{
    int liczbaWarstw;
    int* warstwy;
    int liczbaNeuronow; // bez wejsciowej bo ona nie ma biasów
    float* neurony;
    int liczbaPolaczen;
    float* polaczenia;
} SiecNeuronowa;

float relu(float wartosc)
{
    if(wartosc > 0) return wartosc;
    else return 0;
}

float losowa(float min, float max)
{
    return min + (max - min) * ((float)rand() / RAND_MAX);
}

SiecNeuronowa budujSiec(int liczbaWarstw, int* warstwy)
{
    int liczbaNeuronow = 0; // bez wejsciowej bo ona nie ma biasów
    int liczbaPolaczen = 0;
    for(int i = 0; i < liczbaWarstw - 1; i++)
    {
        liczbaNeuronow += warstwy[i+1];
        liczbaPolaczen += warstwy[i] * warstwy[i+1];
    }
    float* pamiec = malloc((liczbaNeuronow + liczbaPolaczen) * sizeof(float));
    SiecNeuronowa sn;
    sn.liczbaWarstw = liczbaWarstw;
    sn.warstwy = warstwy;
    sn.liczbaNeuronow = liczbaNeuronow;
    sn.neurony = pamiec;
    sn.liczbaPolaczen = liczbaPolaczen;
    sn.polaczenia = pamiec + liczbaNeuronow;
    return sn;
}

/*void wpiszBiasyWarstwie(SiecNeuronowa* wsn, int indeksWarstwy, float* wartosci)
{
    if(indeksWarstwy < 1 || indeksWarstwy >= wsn->liczbaWarstw) return;
    int sum = 0;
    for(int i = 1; i < indeksWarstwy; i++)
    {
        sum += wsn->warstwy[i];
    }
    memcpy(wsn->neurony + sum, wartosci, wsn->warstwy[indeksWarstwy] * sizeof(float));
}

void wpiszWagiPolaczeniomPrzedWarstwa(SiecNeuronowa* wsn, int indeksWarstwy, float* wartosci)
{
    if(indeksWarstwy < 1 || indeksWarstwy >= wsn->liczbaWarstw) return;
    int sum = 0;
    for(int i = 1; i < indeksWarstwy; i++)
    {
        sum += wsn->warstwy[i] * wsn->warstwy[i-1];
    }
    memcpy(wsn->polaczenia + sum, wartosci, wsn->warstwy[indeksWarstwy] * wsn->warstwy[indeksWarstwy-1] * sizeof(float));
}*/

float* przepuscPrzezSiec(SiecNeuronowa* wsn, float* dane)
{
    int liczbaWarstw = wsn->liczbaWarstw;
    int* warstwy = wsn->warstwy;
    float* neurony = wsn->neurony;
    float* polaczenia = wsn->polaczenia;

    // dostosowujemy macierz
    int wymaganyX = 0;
    for(int i = 0; i < liczbaWarstw; i++)
    {
        if(warstwy[i] > wymaganyX) wymaganyX = warstwy[i];
    }
    int wymaganyY = liczbaWarstw;
    if(wymaganyX > xMacierzy) xMacierzy = wymaganyX;
    if(wymaganyY > yMacierzy) yMacierzy = wymaganyY;
    macierz = realloc(macierz, xMacierzy * yMacierzy * sizeof(float));
    
    // wklejamy dane
    memcpy(macierz, dane, warstwy[0] * sizeof(float)); // zakładamy że dane mają rozmiar pierwszej warstwy
    
    // przepuszczamy przez sieć
    int indeksNeuronu = 0;
    int indeksPolaczenia = 0;
    for(int y1 = 1; y1 < liczbaWarstw; y1++) // dla każdej warstwy oprócz wejściowej
    {
        int y0 = y1 - 1; // indeks poprzedniej warstwy
        
        int liczbaNeuronow1 = warstwy[y1];
        for(int x1 = 0; x1 < liczbaNeuronow1; x1++) // dla każdego neuronu tej warstwy
        {
            int y1x1 = y1 * xMacierzy + x1; // indeks wartości neuronu w macierzy
            
            macierz[y1x1] = neurony[indeksNeuronu++]; // ustawiamy wartość na bias

            int liczbaNeuronow0 = warstwy[y0];
            for(int x0 = 0; x0 < liczbaNeuronow0; x0++) // dla każdego neuronu poprzedniej warstwy
            {
                // dodajemy wartości z poprzednich neuronów pomnożone przez wagi
                macierz[y1x1] += polaczenia[indeksPolaczenia++] * macierz[y0 * xMacierzy + x0];
            }
            // funkja aktywacji
            macierz[y1x1] = relu(macierz[y1x1]);
        }
    }

    // wklejamy wynik do bufora
    float* wynik = malloc(warstwy[liczbaWarstw - 1] * sizeof(float));
    memcpy(wynik, macierz + ((liczbaWarstw - 1) * xMacierzy), warstwy[liczbaWarstw - 1] * sizeof(float));
    return wynik;
}

void inicjalizujLosowo(SiecNeuronowa* sn)
{
    srand(time(NULL));

    int indeksNeuronu = 0;
    int indeksPolaczenia = 0;

    for (int i = 1; i < sn->liczbaWarstw; i++)
    {
        int n0 = sn->warstwy[i - 1];
        int n1 = sn->warstwy[i];

        float zakres = sqrtf(6.0f / n0);

        for(int j = 0; j < n1; j++)
        {
            sn->neurony[indeksNeuronu++] = 0.01f;
        }

        for(int j = 0; j < n1 * n0; j++)
        {
            sn->polaczenia[indeksPolaczenia++] = losowa(-zakres, zakres);
        }
    }
}

void wypiszSiec(SiecNeuronowa* wsn)
{
    printf("Siec:\n");
    int licznik = 0;
    for(int y = 1; y < wsn->liczbaWarstw; y++)
    {
        if(y == wsn->liczbaWarstw - 1) printf("Wyjscia: ");
        else printf("Gesta: ");
        for(int x = 0; x < wsn->warstwy[y]; x++)
        {
            printf("%f ", wsn->neurony[licznik++]);
        }
        printf("\n");
    }

    printf("Polaczenia:\n");
    licznik = 0;
    for(int y = 1; y < wsn->liczbaWarstw; y++)
    {
        int iloczyn = wsn->warstwy[y] * wsn->warstwy[y-1];
        for(int x = 0; x < iloczyn; x++)
        {
            printf("%f ", wsn->polaczenia[licznik++]);
        }
        printf("\n");
    }
}

void wypiszMacierz(SiecNeuronowa* wsn)
{
    printf("Macierz: %d, %d\n", xMacierzy, yMacierzy);
    for(int y = 0; y < wsn->liczbaWarstw; y++)
    {
        for(int x = 0; x < wsn->warstwy[y]; x++)
        {
            printf("%f ", macierz[y * xMacierzy + x]);
        }
        printf("\n");
    }
}

int main()
{
    //int warstwy[] = {3, 5, 5, 5, 3};
    int warstwy[] = {2, 3, 2};
    int liczbaWarstw = sizeof(warstwy)/sizeof(warstwy[0]);
    SiecNeuronowa sn = budujSiec(liczbaWarstw, warstwy);
    printf("Neurony: %d, Polaczenia: %d\n", sn.liczbaNeuronow, sn.liczbaPolaczen);

    /*for(int i = 0; i < sn.liczbaNeuronow; i++)
    {
        sn.neurony[i] = 1;
    }

    for(int i = 0; i < sn.liczbaPolaczen; i++)
    {
        sn.polaczenia[i] = 1;
    }*/

    inicjalizujLosowo(&sn);

    float dane[] = {1, 2};
    float* wynik = przepuscPrzezSiec(&sn, dane);
    wypiszSiec(&sn);
    printf("\n");
    wypiszMacierz(&sn);
    printf("\n");
    printf("Wynik: %f, %f\n", wynik[0], wynik[1]);
    
    free(macierz);
    return 0;
}