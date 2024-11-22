# Reflection

This is a work in progress
cpp20 reflection system for classes & structs based on templates & tuples

## Project Health


[![CMake on multiple platforms](https://github.com/paulbaudy/cpp-maths/actions/workflows/cmake-multi-platform.yml/badge.svg)](https://github.com/paulbaudy/cpp-maths/actions/workflows/cmake-multi-platform.yml)

## Usage

```
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

## Build and Install

Todo

## Release notes

Todo

