#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "ini.cpp"
using namespace std;

int WinMain() {

    string path = "input/test.ini";
    //cout << "Enter file to read: ";
    //cin >> path; 
    ini::file x = ini::file(path);
    cout << x.getInt("cat1", "a") << endl;
    return 0;

}