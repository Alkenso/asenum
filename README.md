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

## Usage & Install
Simply copy file 'include/asenum/asenum.h' to your project.

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

## Some usage examples
### Square equation roots
```
#include <asenum/asenum.h>

#include <string>
#include <chrono>
#include <iostream>
#include <cmath>


enum class RootsType
{
    None,
    Single,
    Pair
};

using Roots = asenum::AsEnum<
asenum::Case11<RootsType, RootsType::None, void>,
asenum::Case11<RootsType, RootsType::Single, double>,
asenum::Case11<RootsType, RootsType::Pair, std::pair<double, double>>
>;


Roots FindRoots(const double a, const double b, const double c)
{
    const double d = (b * b) - (4 * a * c);
    if (d < 0)
    {
        return Roots::create<RootsType::None>();
    }
    
    const double x1 = (-b + sqrt(d)) / (2 * a);
    const double x2 = (-b - sqrt(d)) / (2 * a);
    
    return x1 == x2 ? Roots::create<RootsType::Single>(x1) : Roots::create<RootsType::Pair>(std::make_pair(x1, x2));
}

double ReadK(const std::string& name)
{
    double k = 0;
    std::cout << "Input " << name << ": ";
    std::cin >> k;
    
    return k;
}

int main()
{
    const double a = ReadK("a");
    const double b = ReadK("b");
    const double c = ReadK("c");
	
    const Roots roots = FindRoots(a, b, c);
    roots.doSwitch()
    .ifCase<RootsType::None>([] {
        std::cout << "No roots\n";
    })
    .ifCase<RootsType::Single>([] (const double x) {
        std::cout << "Single (equal) roots: " << x;
    })
    .ifCase<RootsType::Pair>([] (const std::pair<double, double>& roots) {
        std::cout << "Different roots. x1 = " << roots.first << "; x2 = " << roots.second << ".\n";
    });
    
    return 0;
}
```

### BarCode convertor
```
#include <asenum/asenum.h>

#include <string>
#include <vector>


enum class BarCodeType
{
    UPC,
    QrCode
};

using BarCode = asenum::AsEnum<
asenum::Case11<BarCodeType, BarCodeType::UPC, std::vector<uint8_t>>,
asenum::Case11<BarCodeType, BarCodeType::QrCode, std::vector<uint8_t>>>
>;

std::string ConvertUPCToLink(const std::vector<uint8_t>& photo)
{
    std::string link;
    // perform some computations...
    
    return link;
}

std::string ConvertQrCodeToLink(const std::vector<uint8_t>& photo)
{
    std::string link;
    // perform some computations...
    
    return link;
}

std::string ConvertBarCodeToLink(const BarCode& barCode)
{
    return barCode.doMap<std::string>()
    .ifCase<BarCodeType::UPC>(ConvertUPCToLink)
    .ifCase<BarCodeType::QrCode>(ConvertQrCodeToLink);
}
```

### Downloader
```
enum class DownloadResultType
{
    Data,
    Error,
    Canceled
};

using DownloadResult = asenum::AsEnum<
asenum::Case11<DownloadResultType, DownloadResultType::Data, std::string>,
asenum::Case11<DownloadResultType, DownloadResultType::Error, int>,
asenum::Case11<DownloadResultType, DownloadResultType::Canceled, void>
>;

DownloadResult DownloadFileFromLink(const std::string& link)
{
    // perform HTTP(s) request
    
    // return response data on success, ...
    return DownloadResult::create<DownloadResultType::Data>("...");
    
    // ... or return server error, ...
    return DownloadResult::create<DownloadResultType::Error>(404);
    
    // ... or inform caller that request has been canceled.
    return DownloadResult::create<DownloadResultType::Canceled>();
}

int main()
{
    const DownloadResult result = DownloadFileFromLink("http://test.api.com/download/file1.txt");
    
    result.doSwitch()
    .ifCase<DownloadResultType::Data>([] (const std::string& content) {
        std::cout << "Downloaded file content: " << content << "\n";
    })
    .ifCase<DownloadResultType::Error>([] (const int errorCode) {
        std::cout << "Request did fail with error: " << errorCode << "\n";
    })
    .ifCase<DownloadResultType::Canceled>([] {
        std::cout << "Request has been canceled by user\n";
    });
    
    return 0;
}
```
