/*
		Napraviti funkciju:
			map<string,vector<string>> grupisi(const vector<string>& v);
			U ulaznom vektoru v funkcije grupisi() se nalaze reci.
			Reci iz ulaznog vektora treba grupisati tako da u istoj grupi budu sve
			reci napravljene permutacijom iste kolekcije karaktera
			(npr: istoj grupi pripadaju reci abc, acb, cba, bac, ali ne i ccb).
			Povratna vrednost ove funkcije je mapa. Kljuc mape je sortiran
			string sacinjen od kolekcije karaktera, a vrednost je vektor sacinjen
			od stringova iz vektora v koji se mogu napraviti permutacijom
			karaktera iz kljuca.
*/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

typedef map<string,vector<string>> mapa;

mapa grupisi(const vector<string>& v) {
    mapa temp;
    for(int i = 0; i < v.size(); i++) {
        string s1 = v[i];
        sort(s1.begin(),s1.end());
        temp[s1].push_back(v[i]);
        }

    return temp;
}

void ispis(mapa x) {
    for(mapa::iterator it = x.begin(); it != x.end(); it++) {
        cout<<"kljuc ";
        cout<<it->first<<" : "<<endl;
        cout<<"vrednosti :"<<endl;
        for(string iter : it->second)
            cout<<iter<<endl;
    }
}

int main()
{
    string reci[] = {"abc", "bac", "ccb", "bcc", "abcc", "cba"};
    vector<string> vektor_reci;
    for(auto it : reci)
        vektor_reci.push_back(it);

    mapa rez;
    rez = grupisi(vektor_reci);
    ispis(rez);

    return 0;
}
