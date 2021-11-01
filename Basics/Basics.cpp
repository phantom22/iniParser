#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "ini.cpp"
using namespace std;

int main() {

    string path = "input/test.ini";

    //cout << "Enter file to read: ";
    //cin >> path; 
    ini::file x = ini::file(path);
    string test = "[abc]\nx=1\n\n[a]y=3";
    //cout << test.substr(0+1, 4-1) << endl;
    return 0;

}