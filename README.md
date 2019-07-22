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
#include <string>
#include <chrono>

 enum class Setting
 {
     Host,
     Port,
     Timeout
 };
 
 // Declare associated enum 'AnySetting' with style of CamelCase without 'get' word in getters.
 ASENUM_DECLARE(AnySetting, Setting)
 {
     ASENUM_DEFINE_STRUCTORS();
     
     ASENUM_CASE_CC(Host, std::string);
     ASENUM_CASE_CC(Port, uint16_t);
     ASENUM_CASE_CC(Timeout, std::chrono::seconds);
 };
 
 
 //===== USAGE =====
 void LogSetting(const AnySetting& setting)
 {
     switch (setting.type())
     {
         case Setting::Host:
             std::cout << "Host: " << setting.Host() << "\n";
             break;
         case Setting::Port:
             std::cout << "Port: " << setting.Port()<< "\n";
             break;
         case Setting::Timeout:
             std::cout << "Timeout: " << setting.Timeout().count() << "\n";
             break;
         default:
             break;
     }
 }
 
 
 // ===== CREATION =====
 LogSetting(AnySetting::CreateHost("test.api.com"));
 LogSetting(AnySetting::CreatePort(65535));
 LogSetting(AnySetting::CreateTimeout(std::chrono::seconds(1));
```

