#include <iostream>

using namespace std;

#include <string>
#include <vector>

class Racunar {
    private:
        string hard;
        string proc;
    public:
        void setHard(string x) {
        hard = x;}

        void setProc(string x) {
        proc = x;}

        string getHard() {
        return hard;}

        string getProc() {
        return proc;}

};

class Ucionica{
    private:
        string naziv;
        double kvadrati;
        vector<Racunar> racunari;
    public:
        void setNaziv(string x) {
        naziv = x;}

        void setKvadrati(double x) {
        kvadrati = x;}

        string getNaziv() {
        return naziv;}

        double getKvadrati() {
        return kvadrati;}

        void dodajRacunar(Racunar x) {
        racunari.push_back(x);}

        vector<Racunar> getRacunari() {
        return racunari;
        }
};

int main()
{
    Racunar r1,r2;
    r1.setHard("wd");
    r1.setProc("i5");
    r2.setHard("sandisk");
    r2.setProc("i7");

    Ucionica u;
    u.setNaziv("a1");
    u.setKvadrati(100);
    u.dodajRacunar(r1);
    u.dodajRacunar(r2);

    cout<<"naziv ucionice je "<<u.getNaziv()<<" i ima "<<u.getKvadrati()<<" kvadrata"<<endl<<"racunari:"<<endl;
    cout<<"mrs";
    vector<Racunar>::iterator it;
    vector<Racunar> vektor= u.getRacunari();
    it=vektor.begin();
    it++;
    Racunar r3 ;
    r3=*it;
    cout<<"proslo";
    cout<<r3.getHard();
    for(it = (u.getRacunari()).begin(); it != vektor.end(); ++it) {
        cout<<it->getHard()<<endl;
        }


    return 0;
}
