#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "ini.cpp"
using namespace std;

int main() {

    string iniPath = "input/settings.ini";
    ini::file settings = ini::file(iniPath);

    string num1 = settings.get<string>("main","a", "asdf");
    int num2 = settings.get<int>("main","b", 0);
    
    cout << num1 << endl;
    return 0;

}