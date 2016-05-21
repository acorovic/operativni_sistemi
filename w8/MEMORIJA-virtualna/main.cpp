/*
    Napisati konkurentni program koji simulira menjanje kapaciteta raspolozive memorije pri izvrsavanju nekog programa.
    Program je predstavljen klasom Program.
    U konstruktoru klase prosledjuje se ukupni kapacitet RAM memorije.
    Nit predstavljena datom funkcijom "izvrsavanje" izvrsava listu naredbi programa pozivom metode
    izvrsi_naredbu() u klasi Program.
    Metoda izvrsi_naredbu() izvrsava prosledjenu naredbu. Moguce naredbe su:
        - malloc(X) - zauzima se X kolicina memorije i
        - repeat[N]malloc(X) - N puta se zauzima kolicina memorije X

    Pri zauzimanju memorije, pokusava se zauzeti RAM memorija. Ukoliko nema dovoljno RAM memorije, vrsi se
    zauzimanje virtuelne memorije. U ovoj simulaciji kapacitet virtuelne memorije je neogranicen.

    Za zauzimanje virtuelne memorije zaduzen je operativni sistem predstavljem posebnom niti koja izvrsava datu
    funkciju "os". Funkcija "os" poziva metodu zauzmi_virtuelnu_memoriju() klase program.
    Nit vrsi zauzimanje virtuelne memorije onda kada dobije zahtev od niti koja vrsi izvrsavanje naredbi programa.
    Kao rezultat, metoda zauzmi_virtuelnu_memoriju() vraca podatak koliko je ukupno virtuelne memorije trenutno zauzeto.

	Bilo kakav pristup memoriji (i RAM i virtuelnoj) je ekskluzivan - u jednom trenutku samo jedna naredba moze
	zauzimati memoriju i dok operativni sistem zauzima virtuelnu memoriju, nije moguc pristup bilo kojoj memoriji.

*/

#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;
using namespace chrono;

struct Povratna_vrednost {          //Struktura koju vraca program nakon pokusaja zauzimanja memorije
    int slobodno_ram;
    int zauzeto_u_virtuelnoj;
};

struct Naredba {				//Struktura koja modeluje 2 tipa naredbe - ili malloc(kolicina memorije) ili
	string tip;                 //repeat[ponavjanja]malloc(kolicina_memorije)
	int ponavljanja;
	int kolicina_memorije;
	Naredba(string t, int p, int k_m) : tip(t), ponavljanja(p), kolicina_memorije(k_m) {}
};

class Program {
    private:
        mutex m;
        int slobodnoRam;
        int zauzetoVirt;
        bool aktivirajVirt;
        condition_variable cv_virt_start, cv_virt_stop;
        int zauzmiVirt;
        void izvrsi_malloc(int kolicina_memorije);
    public:
    	Program(int kapacitet);
        Povratna_vrednost izvrsi_naredbu(Naredba naredba);
        int zauzmi_virtuelnu_memoriju();
};

int Program::zauzmi_virtuelnu_memoriju() {
    unique_lock<mutex> l(m);
    while(!aktivirajVirt)
        cv_virt_start.wait(l);

    this_thread::sleep_for(seconds(1));
    zauzetoVirt += zauzmiVirt;
    aktivirajVirt = false;
    zauzmiVirt = 0;
    cv_virt_stop.notify_one();

    return zauzetoVirt;
}

void Program::izvrsi_malloc(int kolicina_memorije) {
    unique_lock<mutex> l(m);
    if(slobodnoRam < kolicina_memorije) {
        aktivirajVirt = true;
        zauzmiVirt = kolicina_memorije;
        cv_virt_start.notify_one();
        while(aktivirajVirt)
            cv_virt_stop.wait(l);
    } else
        slobodnoRam -= kolicina_memorije;

    this_thread::sleep_for(seconds(1));
}

Povratna_vrednost Program::izvrsi_naredbu(Naredba naredba) {
    if(naredba.tip == "malloc")
        izvrsi_malloc(naredba.kolicina_memorije);
    else
        for(int i = 0; i < naredba.ponavljanja; i++) {
            izvrsi_malloc(naredba.kolicina_memorije);

            }

    return {slobodnoRam,zauzetoVirt};
}

Program::Program(int kapacitet) {
    slobodnoRam = kapacitet;
    zauzetoVirt = 0;
    aktivirajVirt = false;
    zauzetoVirt = 0;
}

mutex term_mx;

void izvrsavanje(Program &p, vector<Naredba>& vektor_naredbi) {
    for (auto it = vektor_naredbi.begin(); it != vektor_naredbi.end(); ++it) {       //izvrsi sve naredbe iz vektora naredbi

        string str_naredba = "";
        str_naredba = "Izvrsavanje naredbe: " + it->tip;
        if (it->tip == "malloc") {
        	str_naredba += "(" + to_string(it->kolicina_memorije) + ")";
        } else if (it->tip == "repeat") {
        	str_naredba += "[" + to_string(it->ponavljanja) + "]" + "malloc" + "(" + to_string(it->kolicina_memorije) + ")";
        }
        {
            unique_lock<mutex> l(term_mx);
            cout << str_naredba << endl;
        }
        Povratna_vrednost ret = p.izvrsi_naredbu(*it);        //uzima se jedna po jedna naredba i izvrsava
        unique_lock<mutex> l(term_mx);
        string ispis = str_naredba.substr(str_naredba.find(":"));
        cout << "Ostalo ram memorije nakon naredbe: " << ispis << ":" << ret.slobodno_ram << endl;
        cout << "Zauzeto virtuelne memorije nakon naredbe: " << ispis << ":" << ret.zauzeto_u_virtuelnoj << endl;
    }
}

void os(Program& p) {
    while (true) {                          //OS radi ciklicno i ceka komandu za virtuelnom memoriju od malloc-a.
        int ret = p.zauzmi_virtuelnu_memoriju();

        cout << "Izvrseno zauzimanje virtuelne memorije. Trenutno se koristi: " << ret << " MB ove memorije." << endl;
    }
}



int main() {

    Program p(100);                         //Ima 100mb memorije
    vector<Naredba> vektor_naredbi;             //Vektor naredbi

    Naredba n1("malloc", 1, 70);
    Naredba n2("malloc", 1, 50);
    Naredba n3("malloc", 1, 30);
    Naredba n4("repeat", 3, 10);

    vektor_naredbi.push_back(n1);
    vektor_naredbi.push_back(n2);
    vektor_naredbi.push_back(n3);
    vektor_naredbi.push_back(n4);

    thread i(izvrsavanje, ref(p), ref(vektor_naredbi));
    thread o(os, ref(p));
    i.join();
    o.detach();
    exit(0);   //exit 0 - predstavlja signal detach-ovanoj niti da prekine rad jer vise nije neophodna
}

