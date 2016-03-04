/*
		Napraviti funkciju:
			map<int,vector<string>> grupisi(const vector<string>& v);
			U ulaznom vektoru v funkcije grupisi() se nalaze reci.
			Reci iz vektora v treba grupisati tako da u istoj grupi budu
			sve reci iste duzine.
			Povratna vrednost ove funkcije je mapa.
			Kljuc (key) predstavlja duzinu reci, a vrednost (value) je vektor sacinjen
			od svih reci iz vektora v iste (vrednost kljuca) duzine.
*/

#include <iostream>
#include <map>
#include <vector>
#include <string>

using namespace std;

typedef map<int,vector<string>> mapa;

mapa grupisi(const vector<string>& v) {
    mapa temp;
    for(auto it : v) {
        temp[it.size()].push_back(it);
    }
    return temp;
}

void ispis(mapa x) {
    for(mapa::iterator it = x.begin(); it != x.end(); it++) {
        cout<<it->first<<" : "<<endl;
        for(string iter : it->second)
            cout<<iter<<endl;
    }
}

int main()
{
    string reci[] = {"aleksa", "aleksandar", "marko", "milos", "dragan"};
    vector<string> vektor_reci;
    for(auto it : reci)
        vektor_reci.push_back(it);

    mapa rezultat = grupisi(vektor_reci);

    ispis(rezultat);

    return 0;
}
