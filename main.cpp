#include <iostream>

#include <uvw_iot/shelly/ShellyThing.h>

using namespace std;

int main() {
   auto shelly =  uvw_iot::shelly::ShellyThing::from("", 23);

    cout << "Hello World!" << endl;
    return 0;
}
