/*Autor : Mateusz Jastrzebski
*/

/*
Zadaniem programu jest uproszczenie grafu, to jest przeprowadzenie
słowa opisanego gramatyką:
graf = "strict" "digraph" podgraf ;
podgraf = "{" { węzły { "->" węzły } } "}" ;
węzły = ID | podgraf ;

w słowo opisane gramatyką:
graf = "digraph" "{" { ID "->" "{" { ID } "}" } "}" ;
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ZERO 48
#define DZIEWIEC 57

struct sciezka{
    int w;
    struct sciezka *sciezki;
};
typedef struct sciezka Tsciezki;

struct wezel{
    int w;
    struct wezel *nast;
    Tsciezki *sciezki;
};
typedef struct wezel Twezel;

void wczytaj_liczbe(int t[], int *indeks, int liczba, bool *czykoniec, bool *czywrzucenie, bool *czynawiasotw){
    int znak = liczba + ZERO;
    while(znak >= ZERO && znak <= DZIEWIEC){
        znak = getchar();
        if(znak >= ZERO && znak <= DZIEWIEC)
            liczba = liczba*10 + znak - ZERO;
    }
    t[(*indeks)] = liczba;
    (*indeks)++;
    if(znak == (int)('}')) *czykoniec = true;//na wypadek pobrania znaku rozpatrywanego w warunkach
    if(znak == (int)('-')) *czywrzucenie = true;
    if(znak == (int)('{')) *czynawiasotw = true;
}

//funkcja wczytuje wszystko przed pierwszym nawiasem otwierajacym
void wczytaj_poczatek(){
    int znak = -1;
    while(znak != (int)('{'))
        znak = getchar();
}

/*funkcja sprawdza czy istnieje na liscie wierzcholek o danym numerze
jesli taki wierzcholek nie istnieje, funkcja dopelnia liste o ten wierzcholek*/
void sprawdz_czy_istnieje(Twezel **lista_powiazan, int liczba){
    Twezel *pom = *lista_powiazan;
    bool czyistnieje = false;
    while(!czyistnieje){
        if(pom->nast == NULL){
            pom->nast = (Twezel *)malloc(sizeof(Twezel));
            pom->nast->w = liczba;
            pom->nast->nast = NULL;
            pom->nast->sciezki = (Tsciezki *)malloc(sizeof(Tsciezki));
            pom->nast->sciezki->w = -1;
            pom->nast->sciezki->sciezki = NULL;
            czyistnieje = true;
        }
        else if(pom->nast->w == liczba){
            czyistnieje = true;
        }
        else if(pom->nast->w > liczba){
            Twezel *S = (Twezel *)malloc(sizeof(Twezel));
            S->w = liczba;
            S->nast = pom->nast;
            S->sciezki = (Tsciezki *)malloc(sizeof(Tsciezki));
            S->sciezki->w = -1;
            S->sciezki->sciezki = NULL;
            pom->nast = S;
            czyistnieje = true;
        }
        pom = pom->nast;
    }
}

void uzupelnij(Twezel **lista_powiazan, int gdzie,  int liczba){
    Twezel *pom; Tsciezki *pom2;
    pom = *lista_powiazan;
    while(pom->w != gdzie){//wyszukiwanie wierzcholka, z ktorego mozna dojsc do wierzcholka o numerze liczba
        pom = pom->nast;//wierzcholek istnieje, bo wczesniej zostal dodany w funkcji sprawdz_czy_istnieje
    }
    pom2 = pom->sciezki;
    bool czyistnieje = false;
    while(!czyistnieje){
        if(pom2->sciezki == NULL){
            Tsciezki *S = (Tsciezki *)malloc(sizeof(Tsciezki));
            S->w = liczba;
            pom2->sciezki = S;
            pom2->sciezki->sciezki = NULL;
            czyistnieje = true;
        }
        else if(pom2->sciezki->w == liczba){
            czyistnieje = true;
        }
        else if(pom2->sciezki->w > liczba){
            Tsciezki *S = (Tsciezki *)malloc(sizeof(Tsciezki));
            S->w = liczba;
            S->sciezki = pom2->sciezki;
            pom2->sciezki = S;
            czyistnieje = true;
        }
        pom2 = pom2->sciezki;
    }
}

//funkcja uzupelniajaca reprezentacje o polaczenia
void wypelnij_sciezki(int t[], Twezel **lista_powiazan, int wcz_ind, int ob_ind, int rozmiar_el_wcz, int rozmiar_el){
    for(int i = wcz_ind; i < wcz_ind + rozmiar_el_wcz; i++){
        for(int j =ob_ind; j < ob_ind + rozmiar_el; j++){
            uzupelnij(lista_powiazan, t[i], t[j]);
        }
    }
}

void wczytaj_nawiasy(int t[], int *indeks, Twezel **lista_powiazan){
    int znak = -1;
    int rozmiar_el = 0;
    int rozmiar_el_wcz = 0;
    int ob_ind = (*indeks);
    int wcz_ind = (*indeks);
    bool wrzucenie = false;
    bool czykoniec = false;
    bool czywrzucenie = false;
    bool czynawiasotw = false;

    while(znak != (int)('}')){
        znak = getchar();
        if(znak >= ZERO && znak <= DZIEWIEC){
            wcz_ind = ob_ind;
            ob_ind = (*indeks);
            rozmiar_el_wcz = rozmiar_el;
            wczytaj_liczbe(t, indeks, znak - ZERO, &czykoniec, &czywrzucenie, &czynawiasotw);
            sprawdz_czy_istnieje(lista_powiazan, t[(*indeks)-1]);
            rozmiar_el = 1;
            if(wrzucenie){
                wypelnij_sciezki(t, lista_powiazan, wcz_ind, ob_ind, rozmiar_el_wcz, rozmiar_el);
                wrzucenie = false;
            }
            wrzucenie = false;
        }
        if(znak == (int)('{') || czynawiasotw){
            czynawiasotw = false;
            rozmiar_el_wcz = rozmiar_el;
            wcz_ind = ob_ind;
            ob_ind = (*indeks);
            wczytaj_nawiasy(t, indeks, lista_powiazan);
            rozmiar_el = (*indeks) - ob_ind;
            if(wrzucenie){
                wypelnij_sciezki(t, lista_powiazan, wcz_ind, ob_ind, rozmiar_el_wcz, rozmiar_el);
                wrzucenie = 0;
            }
            wrzucenie = false;
        }
        if(znak == (int)('-') || czywrzucenie){//zabezpieczenia przed pominieciem znakow w funkcji wczytaj_liczbe
            wrzucenie = true;
            czywrzucenie = false;
        }
        if(czykoniec){
            znak = (int)('}');
            czykoniec = false;
        }
    }
}

void zwolnij(Twezel **lista_powiazan){
    Twezel *pomocnicza; Tsciezki *pomocnicza2, *pomocnicza3;
    while((*lista_powiazan) != NULL){
        pomocnicza2 = (*lista_powiazan)->sciezki;
        while(pomocnicza2 != NULL){
            pomocnicza3 = pomocnicza2->sciezki;
            free(pomocnicza2);
            pomocnicza2 = pomocnicza3;
        }
        pomocnicza = (*lista_powiazan)->nast;
        free(*lista_powiazan);
        (*lista_powiazan) = pomocnicza;
    }
}

void wydrukuj(Twezel **lista_powiazan){
    Twezel *S = *lista_powiazan; Tsciezki *P;
    printf("digraph {\n");
    while(S != NULL){
        S = S->nast;
        if(S != NULL){
            printf("%d -> { ", S->w);
            P = S->sciezki;
            P = P->sciezki;
            while(P != NULL){
                printf("%d ", P->w);
                P = P->sciezki;
            }
        }
        printf("}\n");
    }
}


void wczytaj(){
    wczytaj_poczatek();
    int t[100000];
    int indeks = 0;
    Twezel *lista_powiazan = (Twezel *)malloc(sizeof(Twezel));//lista wszystkich wierzcholkow i polaczen
    lista_powiazan->w = -1;//pierwszy element - zabezpieczajacy
    lista_powiazan->nast = NULL;
    lista_powiazan->sciezki = NULL;
    wczytaj_nawiasy(t,&indeks,&lista_powiazan);
    wydrukuj(&lista_powiazan);
    zwolnij(&lista_powiazan);
}

int main(){
    wczytaj();
    return 0;
}
