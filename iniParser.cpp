#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <type_traits>
using namespace std;

namespace iniParser {

	class file {

		private:

			string _path;
			
			vector<tuple<string, string>> categories;

			void findIllegalChars(vector<char> validChars, string val, bool& isValid) {	    	

	        	isValid = true;
	        	for (size_t i = 0; i < val.length(); i++) {
	        		if (!std::count(validChars.begin(), validChars.end(), val[i])) {
	        			isValid = false;
	        		}
	        	}
			}

			template<
				typename T, 
				typename = typename std::conditional<
					std::is_arithmetic<T>::value, 
						T, 
						std::conditional<
							std::is_same<bool, T>::value,
							bool, 
							string
						>
				>::type
			> 
			void checkIfValid(string val, bool& isValid) {
				vector<char> validChars = { '+', '-', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
       			findIllegalChars(validChars, val, isValid);
			};

			template<typename T> void fallbackIfInvalid(string cat, string prop, T def, string& val, bool& isValid, string type) {
	    	    try {
	    	    	val = getStringValue(cat, prop);
	        	}
	        	catch (invalid_argument e) {
                    cout << e.what() << " fallback value=" << def << endl;
	    	    }
		    	checkIfValid<T>(val, isValid);

		         if (!isValid) {
			        cout << "{" + _path + "} Invalid assignment for [" + type + " " + cat + "." + prop + " == " + val + "] fallback value=[" + def + "]" << endl; 
		        }
	        }

			int findCategory(string category) {
				int res = -1;
				for (size_t i = 0; i < categories.size(); i++) {
					string cat = std::get<0>(categories[i]);
					if (cat == category) {
						res = i;
						break;
					}
				}
				return res;
			}

			string getStringValue(string category, string property) {
				string res = "";
				int catI = findCategory(category);
				if (catI != -1) {
					string &props = std::get<1>(categories[catI]); // store adress of first character of the string
					stringstream ss (props);
					string line;
					// read line by line
					while (getline(ss, line, '\n')) {
						const char delimiter = '=';
						size_t i = line.find(delimiter);
						if (i != string::npos) {
							string pname = line.substr(0, i);
							if (pname == property) {
								string pval = line.substr(i + 1);
								res = pval;
								break;
							}
						}
						else 
							continue;
					}
					if (res != "") {
						return res;
					}
					else
						throw invalid_argument("{" + _path + "} Property [" + category + "." + property + "] not found!");
				}
				else 
					throw invalid_argument("Category [" + category + "] not found!");

			}

		public:

			file(string path) {

				_path = path;
				string contents;

				ifstream file (path);
				if (file.is_open()) {
					stringstream buffer;
					buffer << file.rdbuf();
					contents = buffer.str();
					file.close();
				}

				bool isSplitted = false;
				size_t position = 0;
				// split into categories
				while (!isSplitted) {

					string cut = contents.substr(position);
					size_t bracket1 = cut.find("[");
					size_t bracket2 = cut.find("]");
					string category = cut.substr(bracket1 + 1, bracket2 - bracket1 - 1);
	
					size_t blobStart = position + bracket2 + 2;
					size_t blobEnd = cut.substr(bracket2 + 1).find("[");

					if (blobEnd == string::npos) {
						blobEnd = contents.length() - blobStart + 1;
						isSplitted = true;
					}
					else {
						blobEnd += (position + bracket2) - blobStart;
						position = blobStart + blobEnd;
					}

					string blob = contents.substr(blobStart, blobEnd);
					categories.push_back(make_tuple(category, blob));

				}

			}

			template<typename T> T get (const char cat[], const char prop[], T def);
		
	};

	template<> void file::checkIfValid<bool> (string val, bool& isValid) {
		vector<string> validValues = { "0", "1", "true", "false" };
		isValid = true;
		if (!std::count(validValues.begin(), validValues.end(), val)) {
			isValid = false;
		}
	}

	// ---------------------------------------------------------------------------------------------

	template<> int file::get<int> (const char cat[], const char prop[], int def) {
		string val; bool isValid;
		fallbackIfInvalid(cat, prop, def, val, isValid, "int");

		return isValid ? atoi(&val[0]) : def;
	}

	template<> float file::get<float> (const char cat[], const char prop[], float def) {
		string val; bool isValid;
		fallbackIfInvalid(cat, prop, def, val, isValid, "float");

		char* e;
		return isValid ? strtof(&val[0], &e) : def;
	}

	template<> double file::get<double> (const char cat[], const char prop[], double def) {
		string val; bool isValid;
		fallbackIfInvalid(cat, prop, def, val, isValid, "double");

		return isValid ? atof(&val[0]) : def;
	}

	template<> bool file::get<bool> (const char cat[], const char prop[], bool def) {
		string val; bool isValid;
		fallbackIfInvalid(cat, prop, def, val, isValid, "bool");

		return !isValid ? def : (val == "1" || val == "true" ? true : false);
	}

	template<> string file::get<string> (const char cat[], const char prop[], string def) {
		string val;
		try {
			val = getStringValue(cat, prop);
		}
		catch (invalid_argument e) {
			cout << "{" + _path + "} Invalid assignment for: [string " << cat << "." << prop << " = <empty or missing string>] fallback value=[" + def + "]" << endl; 
			return def;
		}

		return val;
	}

}