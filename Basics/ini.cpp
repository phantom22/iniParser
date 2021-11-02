#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
using namespace std;

namespace ini {

	//enum ExpectedTypes {
	//	tInt
	//};

	class file {

		private:
			vector<tuple<string, string>> categories;

			string getStringValue(string category, string property) {
				string res = "";
				int catI = -1;
				for (size_t i = 0; i < categories.size(); i++) {
					string cat = std::get<0>(categories[i]);
					if (cat == category) {
						catI = i;
						break;
					}
				}
				if (catI != -1) {
					string &props = std::get<1>(categories[catI]);
					stringstream ss (props);
					string line;
					char n = '\n';
					// read line by line
					while (getline(ss, line, n)) {
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
						throw invalid_argument("Property [" + property + "] not found in [" + category + "]!");
				}
				else {
					throw invalid_argument("Category [" + category + "] not found!");
				}

			}

		public:

			file(string path) {

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

			int getInt(string cat, string prop) {
				string val = getStringValue(cat, prop);
				char* i = &val[0];
				return atoi(i);
			}

			float getFloat(string cat, string prop) {
				string val = getStringValue(cat, prop);
				char *s = &val[0];
				char* e;
				return strtof(s, &e);
			}

			double getDouble(string cat, string prop) {
				string val = getStringValue(cat, prop);
				char* i = &val[0];
				return atof(i);
			}

			bool getBool(const char cat[], const char prop[]) {
				string val = getStringValue(cat, prop);
				return val == "1" || val == "true" ? true : false;
			}
		
	};
}