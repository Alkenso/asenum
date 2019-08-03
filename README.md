# asenum
**AssociatedEnum** is a header-only library for C++ for enumerations with associated values

asenum is C++ implementation of very neat enums from Swift language (https://docs.swift.org/swift-book/LanguageGuide/Enumerations.html, chapter 'Associated Values').

asenum combines Enum and Variant: it allows to create lighweight wrapper around plain C++ enum and add ability to assign different value types to different cases.

## Features
- each enum case can be associated with different type
- values are immutable: totally thread-safe
- simple interface
- lightweight, header-only, single file library
- requires only C++11

## Example
```
// For more examples see tests
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
void LogSetting(const AnyError& event)
{
    event.doSwitch()
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
    LogSetting(AnyError::create<ErrorCode::Unknown>("test.api.com"));
    LogSetting(AnyError::create<ErrorCode::Success>());
    LogSetting(AnyError::create<ErrorCode::Timeout>(std::chrono::seconds(1)));
    
    return 0;
}
```

