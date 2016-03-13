/*
Napisati program za unos reci sa tastature sa ukljucenom proverom ispravnosti unosa.
Dat je vektor recnik koji predstavlja reci koje sistem prepoznaje kao ispravne.
Funkcija proveri_ispravnost() utvrdjuje da li se prosledjena rec nalazi u recniku.

U glavnom programu korisnik unosi jednu po jednu rec. Nakon unosa reci, u posebnoj niti se vrsi provera ispravnosti reci.

Kada korisnik zavrsi unos svih reci, za svaku rec se ispisuje da li je ispravno unesena.
Primer ispisa:
Rec 1 ispravno napisana.
Rec 2 neispravno napisana.
Rec 3 ispravno napisana.
*/

#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

vector<string> dozvoljeneReci = {"crno", "crveno", "zeleno"};

void f(string rec, vector<bool> &ispravnost, int n, vector<string> dozvoljeneReci) {
    ispravnost[n] = false;
    for(int i = 0; i < dozvoljeneReci.size(); i++)
        if(dozvoljeneReci[i] == rec) {
            ispravnost[n] = true;
            break;
        }
}

int main()
{
    cout << "unesite reci" << endl;
    vector<string> vektorReci;
    string rec;

    while(getline(cin,rec)) {
        vektorReci.push_back(rec);
    }

    int brojReci = vektorReci.size();
    vector<bool> ispravnostReci(brojReci);
    thread t[brojReci];

    for(int i = 0; i < brojReci; i++) {
        t[i] = thread(f,vektorReci[i],ref(ispravnostReci), i, dozvoljeneReci);
    }

    for(int i = 0; i < brojReci; i++) {
        t[i].join();
    }

    for(int i = 0; i < brojReci; i++)
        cout << "Rec " << (i+1) << " " << vektorReci[i] << " je " << ((ispravnostReci[i] == 1) ? "ispravna" : "neispravna " )<< endl;
    return 0;
}
