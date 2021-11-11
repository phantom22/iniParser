# iniParser

This is a header that allows you to parse the data from an .ini file.

# Usage


- First instantiate with `iniParser::file ini = iniParser::(string path_to_file);`
- Then get a value with `ini.get<T>(const char category[], const char property[], T fallbackValue);`

```cpp
  iniParser::file settings = iniParser::file ("<relative path>");

  /*
   * let's assume that `settings` has parsed some file that has this structure
   * [category]
   * propertyName=1352
   */
  auto x = settings.get("category", "propertyName", 0);
  // in this situation `x` is automatically considered an int, because the third argument is the fallback value
  // (used only if the targeted file doesn't have the specified category/category property)
   
```

# supported default types

```int```, ```float```, ```double```, ```bool``` and the jolly ```string``` which can be used to at least get the wanted string value
and then parse it to a different type.

# how a .ini file is parsed?

Comments (```;```) are supported, the line gets trimmed from ```line start``` to ```comment start```.
Assignments such as ```  my Prop  =315``` (with a lot of spaces before the equals sign) will be interpreted as ```myProp=315``` but if you have ```myProp= 315 3 2```
the value that will be returned from the property ```myProp``` will be " 315 3 2"

# What is the third argument?

It's the fallback value which replaces the original value if it's not a valid one (example: expected type ```int```, original value ```2 5 1``` => ```fallback value``` because the original one cannot be parsed to int; but if it original value ```251``` => ```251``` because it's a valid integer number) 
