#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main()
{
    cout<< "unesite brojeve"<<endl;
    vector<int> brojevi;
    for(int i = 0; i < 5; i++) {
        int br;
        cin >> br;
        brojevi.push_back(br);
    }

    cout << "maksimalni elemenat je "<< *max_element(brojevi.begin(), brojevi.end()) << endl;
    return 0;
}
