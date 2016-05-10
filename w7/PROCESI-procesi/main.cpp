/*Napisati konkurentni program koji modeluje izvrsavanje procesa u multiprogramiranom jednoprocesorskom okruzenju.

Procesi su predstavljeni nitima. Svaki proces ima skup instrukcija koje treba da izvrsi, pri cemu postoje dva tipa
instrukcija: STANDARD (instrukcija zahteva odredjeno izracunavanje od procesora) i UI (zahtev za operacijom ulazno/
izlaznog uredjaja).

Procesor je incijalno slobodan i zauzima ga prvi proces koji do njega dodje i pokusa da izvrsi instrukciju.
Sledi opis obrade STANDARD i UI instrukcija.

-------------------------------------------------------------------------------------------------------------------
OPIS OBRADE STANDARD INSTRUKCIJE:

U sistemu je definisan maksimalan broj od 3 STANDARD instrukcije koje proces koji je zauzeo procesor moze
uzastopno da izvrsi. Nakon sto izvrsi 3 STANDARD instrukcije, proces prelazi u stanje spreman a drugi
proces iz liste spremnih dobija procesor. Na ovaj nacin na procesoru se vrsi prekljucivanje izmedju razlicitih
procesa i time se uspostavlja vremenska kvantizacija izvrsavanja procesa bez monopolizacije procesora od
strane jednog procesa.

Pri prekljucivanju nakon 3 STANDARD instrukcije treba voditi racuna da se obezbedi mehanizam da proces koji
oslobadja procesor (a koji ima jos preostalih instrukcija za izvrsavanje) ne pokusa da zauzme procesor ODMAH
nakon pustanja. U tom slucaju moglo bi se desiti izgladnjivanje ostalih procesa tj. do uskracivanje prava da
i drugi procesi koriste procesor, a sve usled neprekidnog zauzimanja procesora od strane jednog te istog procesa.

Iz liste spremnih procesa, naredni proces za izvrsavanje se bira bez odredjenog pravila. Bilo koji od spremnih
procesa prelazi u izvrsavanje.

Ukoliko neki proces nema vise instrukcija on zavrsava svoj rad i prepusta procesor nekom od procesa iz liste
spremnih procesa.

Izvrsavanje STANDARD instrukcije traje slucajan vremenski period izmedju 10 i 50 ms cime se npr. simulira aritmeticko
logicka operacija racunanja na procesoru.

--------------------------------------------------------------------------------------------------------------------

OPIS OBRADE UI INSTRUKCIJE:

UI instrukcije predstavljaju zahteve za operacijom ulazno izlaznih uredjaja i njih obradjuje UI podsistem
(preko niti ui_podsistem). UI podsistem REDOM opsluzuje pristigle zahteve ukoliko takvi zahtevi postoje.

Ako proces izvrsava UI instrukciju on salje zahtev UI podsistemu i odlazi u stanje cekanja radi zavrsetka
obrade UI zahteva. U cekanju proces otpusta procesor (tokom cekanja na UI obradu nema smisla da se drzi procesor
ako drugi procesi mogu da ga koriste) i tada bilo koji drugi proces iz liste spremnih procesa dobija procesor.

UI instrukcija takodje prekida niz STANDARD instrukcija sto znaci da se broj prethodno izvrsenih STANDARD instrukcija
za dati proces prilikom nailaska na UI instrukciju resetuje na 0.

Kada UI podsistem obradi zahtev, proces koji je cekao obradu zahteva moze da odmah postane aktivan (ukoliko je
procesor slobodan) ili se uvezuje u listu spremnih procesa (ukoliko procesor koristi neki drugi proces).

Operacija UI uredjaja traje slucajan period izmedju 100 i 300 ms.
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>

using namespace std;
using namespace chrono;


enum INS_TYPE {STANDARD = 0, UI};

const int maxVremeStandardne = 50;
const int maxVremeUI = 300;

struct Proces {
	vector<INS_TYPE> instrukcije;
	int id;
	Proces(vector<INS_TYPE> ins, int i): instrukcije(ins), id(i) {}
};

struct Zahtev {
    int id;
    bool obradjen;
    condition_variable cv_zahtev;

    Zahtev(int i) : id(i), obradjen(false) {}
};

class OS {
    private:
        int maxBrInstrukcija;
        int aktivniProces;
        mutex m;
        condition_variable ready;
        queue<Zahtev*> zahtevi;
        condition_variable ui_uredjaj;
	public:
		OS(int maxB);
		void izvrsi_proces(Proces& p);
		void obradi_zahteve();
};

OS::OS(int maxB) {
    maxBrInstrukcija = maxB;
    aktivniProces = -1;
}

void OS::obradi_zahteve() {
    while(true) {
        unique_lock<mutex> lock(m);
        while(zahtevi.empty()) {
            cout << "UI podsistem ceka na zahteve" << endl;
            ui_uredjaj.wait(lock);
        }
        Zahtev *z = zahtevi.front();
        zahtevi.pop();
        cout << "UI uredjaj obradjuje zahtev procesa " << z->id << endl;
        lock.unlock();
        this_thread::sleep_for(milliseconds(rand()%maxVremeUI + 100));
        lock.lock();
        cout << "UI uredjaj obradio zahtev procesa " << z->id << endl;
        z->obradjen = true;
        z->cv_zahtev.notify_one();
    }


}

void OS::izvrsi_proces(Proces& p) {
    int rbrInstrukcije = 0;
    int rbrZaRedom = 0;
    for(auto it = p.instrukcije.begin(); it != p.instrukcije.end(); it++) {
        unique_lock<mutex> lock(m);
        while(p.id != aktivniProces && aktivniProces != -1) {
            cout << "proces " << p.id << " se uvezuje u listu spremnih" << endl;
            ready.wait(lock);
        }
        aktivniProces = p.id;
        if(*it == STANDARD) {
            cout << "proces " << p.id << " poceo sa izvrsavanjem svoje STANDARD instrukcije " << rbrInstrukcije << endl;
            lock.unlock();
            this_thread::sleep_for(milliseconds(rand()%maxVremeStandardne + 10));
            lock.lock();
            cout << "proces " << p.id <<" zavrsio izvrsavanje STANDARD instrukcije " << rbrInstrukcije << endl;
            rbrInstrukcije++;
            rbrZaRedom++;
            if(rbrZaRedom == 3 || rbrInstrukcije == 10) {
                rbrZaRedom = 0;
                cout << "proces " << p.id <<" oslobadja procesor" << endl;
                aktivniProces = -1;
                ready.notify_one();
                lock.unlock();
                this_thread::sleep_for(milliseconds(100));
            }
        }   else {
                rbrInstrukcije++;
                Zahtev *z = new Zahtev(p.id);
                zahtevi.push(z);
                cout << "proces " << p.id << " odlazi u cekanje obrade ui uredjaja za instrukciju " << rbrInstrukcije << endl;
                ui_uredjaj.notify_one();
                while(!z->obradjen) {
                    rbrZaRedom = 0;
                    aktivniProces = -1;
                    ready.notify_one();
                    z->cv_zahtev.wait(lock);
                    cout << "proces " << p.id << " probudjen od strane ui uredjaja" << endl;
                }
                delete z;
            }
    }
}

mutex term_m;

void p(OS& s, int id) {
	vector<INS_TYPE> instrukcije;
	for (int i = 0; i < 10; i++) {
		instrukcije.push_back((INS_TYPE)(rand()%5 == 1)); //9:1 sanse da ce biti STANDARD instrukcija
	}
	Proces p(instrukcije, id);
	s.izvrsi_proces(p);
	unique_lock<mutex> l(term_m);
	cout << "Proces " << id << " se zavrsio." << endl;
}

void ui_podsistem(OS& s) {
	s.obradi_zahteve();
}

int main() {

	srand(time(NULL));

	OS OS(3);
	thread procesi[5];

	thread ui_nit = thread(ui_podsistem, ref(OS));
	ui_nit.detach();

	for (int i = 0; i < 5; i++) {
		procesi[i] = thread(p, ref(OS), i+1);
	}

	for (int i = 0; i < 5; i++) {
		procesi[i].join();
	}
}
