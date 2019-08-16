# asenum
**AssociatedEnum** is a header-only library for C++ for enumerations with associated values

asenum is C++ implementation of very neat enums from Swift language (https://docs.swift.org/swift-book/LanguageGuide/Enumerations.html, chapter 'Associated Values').

asenum combines Enum and Variant: it allows to create lighweight wrapper around plain C++ enum and add ability to assign different value types to different cases.

## Features
- each enum case can be associated with any type
- values are immutable: totally thread-safe
- simple interface
- lightweight, header-only, single file library
- requires only C++11
- convenient switch, map, equality, comparison

## Simple example
```
// More examples you can find in tests
#include <asenum/asenum.h>

#include <string>
#include <chrono>
#include <iostream>

// ===== DECLARATION =====
enum class ErrorCode
{
    Unknown,
    Success,
    Timeout
};

// Associate enum 'ErrorCode' with AsEnum 'AnyError'
using AnyError = asenum::AsEnum<
asenum::Case11<ErrorCode, ErrorCode::Unknown, std::string>,
asenum::Case11<ErrorCode, ErrorCode::Success, void>,
asenum::Case11<ErrorCode, ErrorCode::Timeout, std::chrono::seconds>
>;

// ===== USAGE =====
void LogError(const AnyError& error)
{
    error.doSwitch()
    .ifCase<ErrorCode::Unknown>([] (const std::string& value) {
        std::cout << "Unknown error: " << value << "\n";
    })
    .ifCase<ErrorCode::Success>([] {
        std::cout << "Success\n";
    })
    .ifCase<ErrorCode::Timeout>([] (const std::chrono::seconds& value) {
        std::cout << "Timed out after: " << value.count() << "\n";
    })
    .ifDefault([] {
        std::cout << "Default\n";
    });
}

int main()
{
    // ===== CREATION =====
    LogError(AnyError::create<ErrorCode::Unknown>("test.api.com"));
    LogError(AnyError::create<ErrorCode::Success>());
    LogError(AnyError::create<ErrorCode::Timeout>(std::chrono::seconds(1)));
    
    return 0;
}
```

## Mapping
AsEnum supports mapping to any desired type
*Note: if all cases covered by mapping, it doesn't require 'default' case*
```
// ===== Mapping ======
std::string ErrorToString(const AnyError& error)
{
    // All cases covered
    const auto stringRepresentation = error.doMap<std::string>()
    .ifCase<ErrorCode::Unknown>([] (const std::string& value) {
        return value;
    })
    .ifCase<ErrorCode::Success>([] {
        return "Success";
    })
    .ifCase<ErrorCode::Timeout>([] (const std::chrono::seconds& value) {
        return "Timed out after: " + std::to_string(value.count());
    });
    
    return stringRepresentation;
}

// ===== Partial Mapping ======
std::string ErrorToString2(const AnyError& error)
{
    // All cases covered
    const auto stringRepresentation = error.doMap<std::string>()
    .ifCase<ErrorCode::Success>([] {
        return "Success";
    })
    .ifDefault([] {
        return "Unknown error";
    });
    
    return stringRepresentation;
}
```

## Equality and Comparison
AsEnum provides native way of comparing values
```
void Equality(const AnyError& error1, const AnyError& error2)
{
    // We can check if error1 == error2
    // Cases differ => NOT Equal
    // Cases same AND underlying values same => Equal
    
    const bool equal = error1 == error2;
}

void Comparability(const AnyError& error1, const AnyError& error2)
{
    // We can check how error1 relates to error2
    // Cases of error1 and error2 differ => Compare case ordering
    // Cases same => Compare underlying values
    
    const bool less = error1 < error2;
    const bool lessEqual = error1 <= error2;
    const bool greater = error1 > error2;
    const bool greaterEqual = error1 >= error2;
}
```
