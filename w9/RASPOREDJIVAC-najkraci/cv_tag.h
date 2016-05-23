#include <thread>
#include <map>
#include <condition_variable>
#include <mutex>

using namespace std;
using namespace chrono;

class cv_tag {					//klasa cv_tag simulira condition_variablu sa privescima gde je moguce obavestavati po privesku
	private:
		struct cv_data {		//da bi se ovo omogucilo stvaraju se zasebne CV za svako novo cekanje ove CV se nalaze u cv_data strukturi
			condition_variable c;
			bool fulfilled;
			cv_data(): fulfilled(false) {}
		};
		map<size_t, cv_data*> waiting;	//mapa prethodno opisanih struktura kako bi se moglo stavljati i brisati iz mape sa proizvoljnih mesta
	public:
			cv_tag();
   			~cv_tag();
			void wait(unique_lock<mutex>& ,size_t);
			void notify(size_t tag);
			void notify_all();
};

