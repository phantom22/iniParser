#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <type_traits>

namespace iniParser {

    // a class to
    class ini {

        private:

            std::string PATH = "";
            std::vector<std::tuple<std::string, std::vector<std::tuple<std::string, std::string>>>> PARSED = {};

            std::string removeAllSpacesFromString (std::string str) {
                std::string output = "";

                for (char c : str) {
                    if (c != ' ') {
                        output += c;
                    }
                }

                return output;
            }

            void hasValidStringDeclaration (std::string str, bool& validStringDeclaration, size_t& strDeclStar, size_t& strDeclEnd) {
                strDeclStar = str.find ('"');

                // if first '"' was found
                if (strDeclStar != std::string::npos) {
                    strDeclEnd = str.substr (strDeclStar + 1).find ('"');

                    // check if there is a second one
                    if (strDeclEnd != std::string::npos) {
                        validStringDeclaration = true;
                    }
                }
            }

            void findIllegalChars (std::vector<char> validChars, std::string val, bool& isValid) {

                isValid = true;

                for (size_t i = 0; i < val.length(); i++) {
                    if (!std::count (validChars.begin(), validChars.end(), val[i])) {
                        isValid = false;
                    }
                }
            }

            template <typename T, typename = typename std::conditional<std::is_arithmetic<T>::value, T, std::conditional<std::is_same<bool, T>::value, bool, std::string>>::type>
            void checkIfValid (std::string val, bool& isValid) {
                std::vector<char> validChars = { '+', '-', '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
                findIllegalChars (validChars, val, isValid);
            };

            template<typename T> void fallbackIfInvalid (std::string cat, std::string prop, T def, std::string& val, bool& isValid, std::string type) {
                try {
                    val = getStringValue (cat, prop);
                } catch (std::invalid_argument e) {
                    isValid = false;
                    std::cout << "{" + PATH + "} Category [" + cat + "] for [" + type + " " + cat + "." + prop + "] not found! fallback value=[" + std::to_string (def) + "]" << std::endl;
                    return;
                }

                checkIfValid<T> (val, isValid);

                if (!isValid) {
                    std::cout << "{" + PATH + "} Invalid assignment for [" + type + " " + cat + "." + prop + "=" + val + "] fallback value=[" << def << "]" << std::endl;
                }
            }

            int findCategory (std::string category) {
                int res = -1;

                for (size_t i = 0; i < PARSED.size(); i++) {
                    auto cat = std::get<0> (PARSED[i]);

                    if (cat == category) {
                        res = i;
                        break;
                    }
                }

                return res;
            }

            std::string getStringValue (std::string category, std::string property) {
                std::string res = "";
                int catI = findCategory (category);

                if (catI != -1) {
                    auto props = std::get<1> (PARSED[catI]); // store adress of first character of the string

                    for (int i = 0; i < props.size(); i++) {
                        std::string name = std::get<0> (props[i]);

                        if (name == property) {
                            res = std::get<1> (props[i]);
                        }
                    }

                    if (res != "") {
                        return res;
                    } else {
                        throw std::invalid_argument ("{" + PATH + "} Property [" + category + "." + property + "] not found!");
                    }
                } else {
                    throw std::invalid_argument (""); // all done at line 69
                }

            }

        public:

            ini (std::string path) {

                PATH = path;
                std::string contents;

                std::ifstream file (path);

                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    contents = buffer.str();
                    file.close();
                } else {
                    std::cout << "{" + path + "} Specified file path has not been found!" << std::endl;
                    return;
                }

                bool isSplitted = false;
                size_t position = 0;

                // split into categories
                while (!isSplitted) {

                    std::string cut = contents.substr (position);
                    size_t bracket1 = cut.find ("[");
                    size_t bracket2 = cut.find ("]");
                    std::string category = cut.substr (bracket1 + 1, bracket2 - bracket1 - 1);

                    size_t blobStart = position + bracket2 + 2;
                    size_t blobEnd = cut.substr (bracket2 + 1).find ("[");

                    if (blobEnd == std::string::npos) {
                        blobEnd = contents.length() - blobStart + 1;
                        isSplitted = true;
                    } else {
                        blobEnd += (position + bracket2) - blobStart;
                        position = blobStart + blobEnd;
                    }

                    std::string blob = contents.substr (blobStart, blobEnd);

                    std::vector<std::tuple<std::string, std::string>> pairs;

                    // read line by line
                    std::stringstream b (blob);
                    std::string line;

                    while (getline (b, line, '\n')) {
                        // trim comments
                        const char comment = ';';
                        size_t commentI = line.find (comment);

                        if (commentI != std::string::npos) {
                            line = line.substr (0, commentI);
                        }

                        // find assignment
                        const char assignmentDelimiter = '=';
                        size_t assignmentI = line.find (assignmentDelimiter);
                        std::string propName = "";
                        std::string propVal = "";

                        // get assignment value
                        if (assignmentI != std::string::npos) {

                            std::string tmpName = line.substr (0, assignmentI);
                            propName = removeAllSpacesFromString (tmpName);


                            std::string tmpVal = line.substr (assignmentI + 1);
                            size_t declBegin;
                            size_t declEnd;
                            bool validDecl = false;

                            hasValidStringDeclaration (tmpVal, validDecl, declBegin, declEnd);

                            // if there is no string declaration
                            if (!validDecl) {
                                propVal = removeAllSpacesFromString (tmpVal);
                            } else
                                // get the value inside the two ""
                            {
                                propVal = tmpVal.substr (declBegin + 1, declEnd);
                            }

                        }
                        // if no '=', skip line
                        else {
                            continue;
                        }

                        pairs.push_back (std::make_tuple (propName, propVal));
                    }

                    PARSED.push_back (make_tuple (category, pairs));

                }

            }

            template<typename T> T get (const char cat[], const char prop[], T def);

    };

    template<> void ini::checkIfValid<bool> (std::string val, bool& isValid) {
        std::vector<std::string> validValues = { "0", "1", "true", "false" };
        isValid = true;

        if (!std::count (validValues.begin(), validValues.end(), val)) {
            isValid = false;
        }
    }

    template<> int ini::get<int> (const char cat[], const char prop[], int def) {
        std::string val;
        bool isValid;
        fallbackIfInvalid (cat, prop, def, val, isValid, "int");
        return isValid ? atoi (&val[0]) : def;
    }

    template<> float ini::get<float> (const char cat[], const char prop[], float def) {
        std::string val;
        bool isValid;
        fallbackIfInvalid (cat, prop, def, val, isValid, "float");
        char* e;
        return isValid ? strtof (&val[0], &e) : def;
    }

    template<> double ini::get<double> (const char cat[], const char prop[], double def) {
        std::string val;
        bool isValid;
        fallbackIfInvalid (cat, prop, def, val, isValid, "double");

        return isValid ? atof (&val[0]) : def;
    }

    template<> bool ini::get<bool> (const char cat[], const char prop[], bool def) {
        std::string val;
        bool isValid;
        fallbackIfInvalid (cat, prop, def, val, isValid, "bool");
        return !isValid ? def : (val == "1" || val == "true" ? true : false);
    }

    template<> std::string ini::get<std::string> (const char cat[], const char prop[], std::string def) {
        std::string val;

        try {
            val = getStringValue (cat, prop);
        } catch (std::invalid_argument e) {
            std::cout << "{" + PATH + "} Invalid assignment for: [string " << cat << "." << prop << "=<empty or missing string>] fallback value=[" + def + "]" << std::endl;
            return def;
        }

        return val;
    }

}