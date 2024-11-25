# Reflection

This is a work in progress
cpp20 reflection system for classes & structs based on templates & tuples. Supports Clang 16 & CL

## Project Health


[![CMake on multiple platforms](https://github.com/paulbaudy/cpp-reflection/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/paulbaudy/cpp-reflection/actions/workflows/cmake-multi-platform.yml)

## Usage

```cpp
struct FooStruct
{
	double X = 0.f;
	double Y = 0.f;
	double Z = 0.f;
	std::uint32_t W = 0;
};

RF_BEGIN_LAYOUT(FooStruct)
	RF_ENTRY(X),
	RF_ENTRY(Y),
	RF_ENTRY(Z),
	RF_ENTRY(W)
RF_END_LAYOUT()
```
Then implement a field iterator to visit class members
```cpp
template<class field_t, class parent_field_t>
Reflection::EFieldIterator FooIterator(const parent_field_t& InParentField, const field_t& InField, const Rf::FLayoutFieldView& InViewer)
{
	using T = typename field_t::Type;
	return FReflectionStateHelper<T>::FooIterator(InParentField, InField, InViewer);
}
```

## Build and Install

* Clone the repository
```shell
git clone https://github.com/paulbaudy/cpp-reflection.git
```
* Build using CMake

```shell
cd /path/to/repository
cmake \
    -B build .

cmake --build build -- all
cmake --build build -- install
```
* Or add this repository as a subdirectory in your CMakeLists file :
  
```cmake
add_subdirectory("/path/to/repository")
```



## Release notes

### cpp-reflection 0.0.1 - Work in progress

Features : 
- Main release of the library

