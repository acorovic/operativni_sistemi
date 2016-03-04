#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

int main()
{
    cout<<"unesite 5 brojeva"<<endl;

    int br;
    vector<int> brojevi;

    for(int i = 0; i < 5; i++) {
        cin >> br;
        brojevi.push_back(br);
    }

    sort(brojevi.begin(), brojevi.end());
    reverse(brojevi.begin(), brojevi.end());

    cout<<"sortirano opadajuci : "<<endl;
    for(auto it : brojevi)
        cout<<it<<endl;
    return 0;
}
