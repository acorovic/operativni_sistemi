
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

typedef vector<vector<int>>::reverse_iterator vvitR;
typedef vector<vector<int>>::iterator vvit;
typedef vector<int>::iterator vit;

void ispis(vector<vector<int>> matrica) {
    for(int i = 0; i < matrica.size(); i++) {
        for(int j = 0; j < matrica[0].size(); j++)
            cout << matrica[i][j];
        cout << endl;
    }
    cout << endl;
}

void f(vvitR pocetak, vvitR kraj, vvit &pocetakRez) {
    for( ; pocetak != kraj; pocetak++, pocetakRez++) {
        *pocetakRez = *pocetak;
    }

}
int main()
{

    vector<vector<int>> matrica = {{1,1,1,1,1,1},
                                    {1,1,1,1,1,1},
                                    {2,2,2,2,2,2},
                                    {2,2,2,2,2,2},
                                    {3,3,3,3,3,3},
                                    {3,3,3,3,3,3},
                                    {4,4,4,4,4,4}};
    ispis(matrica);
    int broj_niti = 3;
    int broj_redova = matrica.size();
    vector<vector<int>> matricaRez(broj_redova,vector<int>(matrica[0].size()));

    ispis(matricaRez);
    thread niti[broj_niti];
    vvitR pocetak = matrica.rbegin();
    vvitR kraj = pocetak + broj_redova/broj_niti;
    vvit pocetakRez = matricaRez.begin();
    for(int i = 0; i < broj_niti - 1; i++) {
        niti[i] = thread(f,pocetak,kraj,ref(pocetakRez));
        pocetak = kraj;
        kraj = pocetak + broj_redova/broj_niti;
    }
    niti[broj_niti-1] = thread(f,pocetak,matrica.rend(),ref(pocetakRez));

    for(int i = 0; i < broj_niti; i++)
        niti[i].join();
    ispis(matricaRez);
    return 0;
}
