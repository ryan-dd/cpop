# Cpop

Do you ever get annoyed at parsing XML files? Do you just want a nice clean C++ struct that matches the XML structure of your config file? If so, this tiny library is for you. 

CPop (Config Populator) is a header only library that will populate your struct based on an xml file.

Check out the tests to see usage examples.

# Dependencies

Depends on header-only [__boost::pfr__](https://github.com/boostorg/pfr) for reflection capabilities.

Also boost ptree for the built in xml parser. Although you can implement your own, outputting a cpop::Tree.

Make sure you have boost installed on your system before you build.

# Build

```
cmake -S . -B build
cmake --build build
```

## Dev mode (use static analyzers, use warnings, build tests, etc.)

```
cmake -S . -B build -DCPOP_DEV_MODE=ON
cmake --build build
```

# Install and use

To install onto system after building (linux / osx):

``` 
sudo cmake --install build
```

Then to include in another cmake project:

```
find_package(cpop)
target_link_libraries(YourProject cpop::cpop)
```
