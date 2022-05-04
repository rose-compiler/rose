# mini-yaml
[![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://github.com/jimmiebergmann/mini-yaml#build-status)  
Single header YAML 1.0 C++11 serializer/deserializer.

## Quickstart
#### file.txt
```
key: foo bar
list:
  - hello world
  - integer: 123
    boolean: true
```
#### .cpp
```cpp
Yaml::Node root;
Yaml::Parse(root, "file.txt");

// Print all scalars.
std::cout << root["key"].As<std::string>() << std::endl;
std::cout << root["list"][0].As<std::string>() << std::endl;
std::cout << root["list"][1]["integer"].As<int>() << std::endl;
std::cout << root["list"][1]["boolean"].As<bool>() << std::endl;

// Iterate second sequence item.
Node & item = root[1];
for(auto it = item.Begin(); it != item.End(); it++)
{
    std::cout << (*it).first << ": " << (*it).second.As<string>() << std::endl;
}
```
#### Output
```
foo bar
hello world
123
1
integer: 123
boolean: true
```

See  [Best practice](https://github.com/jimmiebergmann/mini-yaml#best-practice).

## Usage
Put [/yaml](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml) in your project directory and simply #include "[yaml/Yaml.hpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/yaml/Yaml.hpp)".
See [examples/FirstExample.cpp](https://github.com/jimmiebergmann/mini-yaml/blob/master/examples/FirstExample.cpp) for additional examples.

## Best practice
Always use references when accessing node content, if not intended to make a copy. Modifying copied node wont affect the original node content.  
See example:
```cpp
Yaml::Node root;

Yaml::Node & ref = root;  // The content of "root" is not being copied.
ref["key"] = "value";     // Modifying "root" node content.

Yaml::Node copy = root;   // The content of "root" is copied to "copy".
                          // Slow operation if "root" contains a lot of content.
copy["key"] = "value";    // Modifying "copy" node content. "root" is left untouched.
```

## Build status
Builds are passed if all tests are good and no memory leaks were found.

| Branch | Status |
| ------ | ------ |
| master | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=master)](https://travis-ci.org/jimmiebergmann/mini-yaml) |
| dev | [![Build Status](https://travis-ci.org/jimmiebergmann/mini-yaml.svg?branch=dev)](https://travis-ci.org/jimmiebergmann/mini-yaml)|

## Todo
- Parse/serialize tags(!!type).
- Parse anchors.
- Parse flow sequences/maps.
- Parse complex keys.
- Parse sets.

