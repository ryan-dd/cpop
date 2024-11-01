# Cpop

Do you ever get annoyed at parsing XML files? Do you just want a nice clean C++ struct that matches the XML structure of your config file? If so, this tiny library is for you. 

CPop (Config Populator) is a header only library that will populate your struct based on an xml file.

Check out the tests to see usage examples.

# Dependencies

Depends on header-only [__boost::pfr__](https://github.com/boostorg/pfr) for reflection like capabilities.

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

