# idl2cpp
Convert MS IDL to COleDispatchDriver based C++

## Building

```shell
git clone https://github.com/BenHanson/lexertl17
git clone https://github.com/BenHanson/parsertl17
git clone https://github.com/BenHanson/idl2cpp
```

* Use the `Makefile` when building on Linux
* Use the `.sln` file when building with Visual Studio

## Usage

`idl2cpp <pathname.idl> [--enums | --events_h | --events_cpp | --fwd_decls |
  --h [--no_afx] | --cpp] [--name <interface name>]`

## Switches

| Switch                  | Description |
| ----------------------- | ----------- |
| --enums                 | Output all enums as C++ code |
| --events_h              | Output all events as C++ code (header file) |
| --events_cpp            | Output all events as C++ code (Source file) |
| --fwd_decls             | Output forward declarations as C++ code |
| --h                     | Output interfaces as C++ code (header file) with AFX_EXT_CLASS |
| --h --no_afx            | Output interfaces as C++ code (header file) without AFX_EXT_CLASS |
| --cpp                   | Output interfaces as C++ code (source file) |
| --name <interface name> | Only output the interface of the supplied name |
