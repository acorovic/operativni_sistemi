/*Napraviti sekvencijalni program koji izraÄunava
sumu svih elemenata vektora sekvencijalno, koristeÄ‡i funkciju f():

f(ci begin, ci end, double& zbir) pri cemu je
ci typedef vector<double>::const_iterator

*/


#include <iostream>
#include <vector>

using namespace std;

typedef vector<double>::const_iterator ci;

void f(ci a_begin, ci a_end, double& zbir) {
    for( ; a_begin != a_end; a_begin++)
        zbir += *a_begin;
}

int main()
{
    vector<double> a = {1,2,3,4,5};
    double zbir = 0;
    f(a.begin(), a.end(), zbir);

    cout << "zbir je " << zbir << endl;
    return 0;
}
