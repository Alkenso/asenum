# asenum
**AssociatedEnum** is a header-only library for C++ for enumerations with associated values

asenum is C++ implementation of very neat enums from Swift language (https://docs.swift.org/swift-book/LanguageGuide/Enumerations.html, chapter 'Associated Values').

asenum combines Enum and Variant: it allows to create lighweight wrapper around plain C++ enum and add ability to assign different value types to different cases.

## Features
- each enum case can be associated with different type
- values are immutable: totally thread-safe
- simple and powerful interface
- lightweight header-only single file library
- requires only C++11

## Example
```
// Header contains extended usage case
#include <asenum/asenum.h>

#include <string>
#include <chrono>
#include <iostream>

enum class ErrorCode
{
    Unknown,
    Success,
    Timeout
};

// Associate enum 'ErrorCode' with AsEnum 'AnyError'
ASENUM_DECLARE(AnyError, ErrorCode)
{
    ASENUM_DEFINE_STRUCTORS();
    
    ASENUM_CASE(Unknown, std::string);
    ASENUM_CASE_VOID(Success);
    ASENUM_CASE(Timeout, std::chrono::seconds);
};

//===== USAGE =====
void LogSetting(const AnyError& event)
{
    switch (event.type())
    {
        case ErrorCode::Unknown:
            std::cout << "Unknown error: " << event.asUnknown() << "\n";
            break;
        case ErrorCode::Success:
            std::cout << "Success\n";
            break;
        case ErrorCode::Timeout:
            std::cout << "Timed out after: " << event.asTimeout().count() << "\n";
            break;
        default:
            std::cout << "Default\n";
            break;
    }
}

int main()
{
    // ===== CREATION =====
    LogSetting(AnyError::createUnknown("test.api.com"));
    LogSetting(AnyError::createSuccess());
    LogSetting(AnyError::createTimeout(std::chrono::seconds(1)));
    
    return 0;
}
```

