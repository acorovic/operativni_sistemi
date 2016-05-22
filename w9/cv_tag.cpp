#include "cv_tag.h"

cv_tag::cv_tag() {  
}

cv_tag::~cv_tag() {				//destruktor mape, brisu se sve vrednosti jer su dinamicki genreisane (vidi metodu wait)
    for (auto i = waiting.begin(); i!=waiting.end(); ++i)
        delete (*i).second;
}

void cv_tag::wait(unique_lock<mutex>& l, size_t tag) {		//cekanje na CV sa privescima, prosledjuje se lock po referenci i privezak
    waiting.insert(pair<size_t,cv_data*>{tag,new cv_data});	//ubacivanje novog para kljuc vrednost u mapu (kljuc privezak, vrednost cv_data)
				while (!waiting[tag]->fulfilled)			//dogod se nije ispunio uslov cekanja na cv_data sa tim priveskom ceka se
						waiting[tag]->c.wait(l);

    delete waiting[tag];				//kada se ispunio uslov brise se cv_data iz memorije kao i pair struktura iz mape
    waiting.erase(tag);
}

void cv_tag::notify(size_t tag) {			//metoda notifikacije na CV sa privescima, prolsedjuje se privezak koji se obavestava
    if (waiting.find(tag) != waiting.end()) {	//prolazi se kroz mapu i trazi se kljuc (privezak)  
        waiting[tag]->fulfilled = true;			//kada se nadje privezak, polje fulfilled cv_data strutkure (vrednosti) se postavlja na true
        waiting[tag]->c.notify_one();			//i obavesava se nit koja ceka na datoj cv_data
    }
}

void cv_tag::notify_all() {				//notifikacija svih na CV sa privescima, prolazi se kroz mapu svih i obavestavaju se jedan po jedan
	for (auto i = waiting.begin(); i!=waiting.end(); ++i) {
            (*i).second->fulfilled = true;
            (*i).second->c.notify_one();
    }
}


