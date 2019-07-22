/*
 * MIT License
 *
 * Copyright (c) 2019 Alkenso (Vladimir Vashurkin)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <memory>

/**
 // ===== DECLARATION =====
 enum class Setting
 {
     Host,
     Port,
     Timeout
 };
 
 // Declare associated enum 'AnySetting' with style of CamelCase without 'get' word in getters.
 ASSENUM_DECLARE(AnySetting, Setting)
 {
     ASSENUM_DEFINE_STRUCTORS();
     
     ASSENUM_CASE_CC(Host, std::string);
     ASSENUM_CASE_CC(Port, uint16_t);
     ASSENUM_CASE_CC(Timeout, std::chrono::seconds);
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
 */


#define ASSENUM_DECLARE(name, enum) \
class name: protected assenum::impl::AssEnum<enum, name>


#define ASSENUM_DEFINE_STRUCTORS() \
public: using AssEnum::type; \
private: using AssEnum::AssEnum


/// For enums named in CamelCase style
#define ASSENUM_CASE_CC(case, type) ASSENUM_CASE_IMPL_NG(case, type, Create)
/// With 'get' prefix on getter
#define ASSENUM_CASE_CC_G(case, type) ASSENUM_CASE_IMPL_G(case, type, Create)

/// For enums named in lowerCamelCase style
#define ASSENUM_CASE_LCC(case, type) ASSENUM_CASE_IMPL_NG(case, type, create)
/// With 'get' prefix on getter
#define ASSENUM_CASE_LCC_G(case, type) ASSENUM_CASE_IMPL_G(case, type, create)

/// For enums named in snake_case style
#define ASSENUM_CASE_SC(case, type) ASSENUM_CASE_IMPL_NG(case, type, create_)
/// With 'get' prefix on getter
#define ASSENUM_CASE_SC_G(case, type) ASSENUM_CASE_IMPL_G(case, type, create_)


// Private stuff

#define ASSENUM_CASE_IMPL_NG(case, type, createToken) ASSENUM_CASE_IMPL(case, type, createToken, )
#define ASSENUM_CASE_IMPL_G(case, type, createToken) ASSENUM_CASE_IMPL(case, type, createToken, get)

#define ASSENUM_CASE_IMPL(case, type, createToken, getPrefix) \
public: \
    static ThisType createToken##case(type value) \
    { \
        return ThisType(AssociatedEnum::case, std::move(value)); \
    } \
    \
    const type& getPrefix##case() const \
    { \
        return *reinterpret_cast<const type*>(validatedValueOfType(AssociatedEnum::case)); \
    }


namespace assenum
{
    namespace impl
    {
        template <typename Enum, typename ConcreteType>
        class AssEnum
        {
        public:
            using AssociatedEnum = Enum;
            using ThisType = ConcreteType;
            
            Enum type() const 
            { 
                return m_type; 
            }
            
            template <typename T>
            AssEnum(const Enum type, T&& value)
            : m_type(type)
            , m_value(new T(std::forward<T>(value)), [] (void* ptr) {
                if (ptr)
                {
                    delete reinterpret_cast<T*>(ptr);
                }
            })
            {}
            
            const void* validatedValueOfType(const Enum type) const
            {
                if (m_type != type)
                {
                    throw std::invalid_argument("Trying to get value of invalid type.");
                }
                
                return m_value.get();
            }
            
        private:
            const Enum m_type;
            const std::shared_ptr<void> m_value;
        };
    }
}
