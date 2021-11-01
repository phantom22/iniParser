#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
using namespace std;

namespace ini {
	class file {

		private:
			string contents;

		public:
			vector<tuple<string, string>> categories; 
			/*[
			 *  ["cat", "blob"]
			 *]
			 * get<0>(get<1>(properties[0]))
			 */

			file(string path) {

				ifstream file (path);
				if (file.is_open()) {
					stringstream buffer;
					buffer << file.rdbuf();
					contents = buffer.str();
					file.close();
				}

				bool isPlitted = false;
				int beforeNextCat = 0;
				// split into categories
				while (!isPlitted) {

					string cut = contents.substr(beforeNextCat);
					size_t bracket1 = cut.find("[");
					size_t bracket2 = cut.find("]");
					const int tmp = beforeNextCat;
					string category = cut.substr(bracket1 + 1, bracket2 - bracket1 - 1);

					beforeNextCat = cut.substr(bracket2 + 1).find("[");
					if (beforeNextCat == string::npos) {
						beforeNextCat = contents.length() - (tmp + bracket2 + 1);
						isPlitted = true;
					}

					string blob = contents.substr(tmp + bracket2 + 2, beforeNextCat);

					categories.push_back(make_tuple(category, blob));
					
				}

			}

			//template<typename T>
			void get(string category/*, string property*/) {
				cout << "dio porco" << endl;
				//auto cat = find(categories.begin(), categories.end(), category);
				//if (cat != categories.end()) {
				//	//size_t i = cat - categories.begin();
				//	//cout << std::get<0>(categories[i]) << endl;
				//}
				//else {
				//	cout << "Not found!" << endl;
				//}
			}
		
	};
}