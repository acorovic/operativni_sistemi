/* Napisati program koji omogucuje evidenciju fakultetskih ucionica.
		Za svaku ucionicu potrebno je evidentirati broj ucionice, kvadraturu i racunare 
		koji se nalaze u ucionici. Za svaki racunar evidentiraju se podaci o hard disku i 
		procesoru.
		
		U okviru main funkcije, definisati jednu ucionicu koja sadrzi dva racunara.
		Zatim je potrebno na konzolu ispisati podatke o svakom racunaru u ucionici.	
*/

#include <iostream>
#include <string>
#include <list>

#define MAX 100

using namespace std;


class Racunar {
    private:
        string disk;
        string procesor;
    public:
        void setDisk(string d) {
            disk = d;
        }

        string getDisk() {
            return disk;
        }

        void setProcesor(string d) {
            procesor = d;
        }

        string getProcesor() {
            return procesor;
        }
};

class Ucionica {
    private:
        string broj;
        double kvadratura;
        //Racunar racunari[MAX];
        list<Racunar> racunari;
    public:
        void setBroj(string br) {
            broj = br;
        }

        string getBroj() {
            return broj;
        }

        void setKvadratura(double kv) {
            kvadratura = kv;
        }

        double getKvadratura() {
            return kvadratura;
        }

        void dodajRacunar(Racunar r,int i) {
            racunari.push_back(r);
        }

        list<Racunar> getRacunari() {
            return racunari;
        }
};


int main()
{
    Ucionica u;
    u.setBroj("MIA2-3");
    u.setKvadratura(100);

    Racunar r1;
    r1.setDisk("seagate");
    r1.setProcesor("i5");

    Racunar r2;
    r2.setDisk("wd");
    r2.setProcesor("i7");

    u.dodajRacunar(r1,0);
    u.dodajRacunar(r2,1);

    list<Racunar> racunari = u.getRacunari();
    list<Racunar>::iterator it;
    for(it = racunari.begin(); it != racunari.end(); it++) {
        cout << "disk racunara " << it->getDisk() <<endl;
        cout << "procesor racunara " << it->getProcesor() <<endl;
    }
    return 0;
}
