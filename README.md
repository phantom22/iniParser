# iniParser

This is a header that allows you to parse the data from an .ini file.

# Usage


- First instantiate with `iniParser::ini name = iniParser::ini(string relative_path_to_file);`
- Then get a value with `ini.get<T>(const char category[], const char property[], T fallbackValue);`

```cpp
  iniParser::ini settings = iniParser::ini ("relative_folder/settings.ini");

  /*
   * let's assume that settings.ini has this structure:
   * [category]
   * propertyName=1352
   */
  auto x = settings.get("category", "propertyName", 0);
  // in this situation `x` is automatically considered an int, because the third arguments type is the same as the return type
  // (used only if the targeted file doesn't have the specified category/category property)
  // x is equal to 1352
   
```

# supported default types

`int`, `float`, `double`, `bool` and the jolly `string` which can be used to get the wanted string value
and then parse it to a different type based on your needs.

# how a .ini file is parsed?

Comments (`;`) are supported, the line gets trimmed from `line start` to `comment start`.

Any spaces are automatically removed (for ex. `  my Var = 315` is equal to `myVar=315` but if you add "" like in this example `my var = " some str ing"` it will be parsed as `myVar= some str ing`). P.S. the string declaration rule works only for the property value!

# What is the third argument?

It's the fallback value which replaces the original value if it's not a valid one.
```cpp
  /*
   *  Processed file
   *  [numbers]
   *  numberA = 1 35
   *  numberB =  "1 35"
   */
  int myInt = settings.get("numbers", "numberA", 353); // is equal to 135
  int anotherInt = settings.get("numbers" "numberB", 1337); // is equal to 1337 because all spaces are preserved in string declaration
  
  string fallbackVal = "some cool string"; // for strings, the fallbackValue must be declared like this
  string myString = settings.get("numbers", "numberB", fallbackVal); // is equal to "1 35"
  
  string secondString = settings.get("numbers", "numberB", "some cool string");  // ERROR
  
  // a workaround could be using the string constructor
  string thirdString = settings.get("number", "numberB", string("some cool string")); // if done correctly it won't throw an error
  
```

# What about errors?

This project handles invalid ini file paths, categories and property names; every unwanted behavior is easily solved due to the built-in console suggestions which specify exactly what's wrong or missing. (no error handling is required, fallback values work even if the file is non-existent)
