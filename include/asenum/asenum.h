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

/// Declares Associated Enum with 'name' and associates it with 'enum'
#define ASENUM_DECLARE(name, enum) ASENUM_DECLARE_IMPL(name, enum)

/// Defines Associated Enum internal stuff. Must be placed first line inside 'ASENUM_DECLARE' (see example below)
#define ASENUM_DEFINE_STRUCTORS() ASENUM_DEFINE_STRUCTORS_IMPL()

/// Defines association between enum 'case' and 'type' bound to this case
#define ASENUM_CASE(case, type) ASENUM_CASE_IMPL(case, type)

/// Defines association between enum 'case' and void type bound to this case
#define ASENUM_CASE_VOID(case) ASENUM_CASE_VOID_IMPL(case)


/**
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
 void LogError(const AnyError& event)
 {
     event.doSwitch()
     .asCase<ErrorCode::Unknown>([] (const std::string& value) {
        std::cout << "Unknown error: " << value << "\n";
     })
     .asCase<ErrorCode::Success>([] {
        std::cout << "Success\n";
     })
     .asCase<ErrorCode::Timeout>([] (const std::chrono::seconds& value) {
        std::cout << "Timed out after: " << value.count() << "\n";
     })
     .asDefault([] {
        std::cout << "Default\n";
     });
 
     //    === vs ===
 
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
 
     // === vs ===
 
     if (event.is<ErrorCode::Unknown>())
     {
        std::cout << "Unknown error: " << event.asUnknown() << "\n";
     }
     else if (event.is<ErrorCode::Success>())
     {
        std::cout << "Success\n";
     }
     else if (event.is<ErrorCode::Timeout>())
     {
        std::cout << "Timed out after: " << event.asTimeout().count() << "\n";
     }
 }
 
 int main()
 {
     // ===== CREATION =====
     LogError(AnyError::createUnknown("test.api.com"));
     LogError(AnyError::createSuccess());
     LogError(AnyError::createTimeout(std::chrono::seconds(1)));
 
     //    === vs ===
 
     LogError(AnyError::create<ErrorCode::Unknown>("test.api.com"));
     LogError(AnyError::create<ErrorCode::Success>());
     LogError(AnyError::create<ErrorCode::Timeout>(std::chrono::seconds(1)));
 
     return 0;
 }
 */



// Private implementation details


#define ASENUM_DECLARE_IMPL(name, enum) \
class name: protected asenum::impl::AsEnum<enum, name>


#define ASENUM_DEFINE_STRUCTORS_IMPL() \
private: \
    using AsEnum::AsEnum; \
    \
    template <typename T> \
    struct AssociatedType { using Type = T; }; \
    \
    template <AssociatedEnum T_type> \
    struct CaseCast; \
    \
public: \
    template <AssociatedEnum T_type> \
    using UnderlyingType = typename CaseCast<T_type>::Type; \
    \
    template <AssociatedEnum T_type> \
    static ThisType create(UnderlyingType<T_type> value) \
    { \
        return ThisType(T_type, std::move(value)); \
    } \
    \
    template <AssociatedEnum T_type> \
    static ThisType create() \
    { \
        return ThisType(T_type); \
    } \
    \
    template <AssociatedEnum T_type> \
    const UnderlyingType<T_type>& as() const \
    { \
        return validatedValueOfType<typename CaseCast<T_type>::Type>(T_type); \
    } \
    \
    asenum::impl::AsSwitch<AssociatedEnum, ThisType, CaseCast> doSwitch() const \
    { \
        return asenum::impl::AsSwitch<AssociatedEnum, ThisType, CaseCast>(*this); \
    } \
    \
    using AsEnum::type; \
    using AsEnum::is


#define ASENUM_CASE_IMPL(case, type) \
public: \
    static ThisType create##case(type value) \
    { \
        return create<AssociatedEnum::case>(std::move(value)); \
    } \
    \
    const type& as##case() const \
    { \
        return validatedValueOfType<type>(AssociatedEnum::case); \
    } \
    \
    bool is##case() const \
    { \
        return is<AssociatedEnum::case>(); \
    } \
    \
    template <> \
    struct CaseCast<AssociatedEnum::case> : AssociatedType<type> {}


#define ASENUM_CASE_VOID_IMPL(case) \
public: \
    static ThisType create##case() \
    { \
        return create<AssociatedEnum::case>(); \
    } \
    \
    bool is##case() const \
    { \
        return is<AssociatedEnum::case>(); \
    } \
    \
    template <> \
    struct CaseCast<AssociatedEnum::case> : AssociatedType<void> {}


namespace asenum
{
    namespace impl
    {
        template <typename Enum, typename ConcreteType>
        class AsEnum
        {
        public:
            using AssociatedEnum = Enum;
            using ThisType = ConcreteType;
            
            template <typename T>
            AsEnum(const AssociatedEnum type, T&& value)
            : m_type(type)
            , m_value(new T(std::forward<T>(value)), [] (void* ptr) {
                if (ptr)
                {
                    delete reinterpret_cast<T*>(ptr);
                }
            })
            {}
            
            AsEnum(const AssociatedEnum type)
            : m_type(type)
            {}
            
            AssociatedEnum type() const
            {
                return m_type;
            }
            
            template <AssociatedEnum T_type>
            bool is() const
            {
                return type() == T_type;
            }
            
            template <typename T>
            const T& validatedValueOfType(const AssociatedEnum type) const
            {
                if (m_type != type)
                {
                    throw std::invalid_argument("Trying to get value of invalid type.");
                }
                
                if (!m_value)
                {
                    throw std::logic_error("Trying to get value of void associated type.");
                }
                
                return *reinterpret_cast<const T*>(m_value.get());
            }
            
        private:
            const AssociatedEnum m_type;
            const std::shared_ptr<void> m_value;
        };
        
        template <typename Enum, typename ConcreteType, template <Enum t_type> class CaseCast, Enum... types>
        class AsSwitch
        {
            template<Enum...> struct Contains;
            template<Enum T_type> struct Contains<T_type> { static const bool value = false; };
            template<Enum T_type1, Enum T_type2> struct Contains<T_type1, T_type2> { static const bool value = T_type1 == T_type2; };
            
            template<Enum T_type1, Enum T_type2, Enum... T_other>
            struct Contains<T_type1, T_type2, T_other...> { static const bool value = Contains<T_type1, T_type2>::value || Contains<T_type2, T_other...>::value; };
            
            template <Enum T_type, typename Handler, bool isVoid>
            struct HandlerCaller;
            
            template <Enum T_type, typename Handler>
            struct HandlerCaller<T_type, Handler, true>
            {
                static void call(const ConcreteType&, const Handler& handler)
                {
                    handler();
                }
            };
            
            template <Enum T_type, typename Handler>
            struct HandlerCaller<T_type, Handler, false>
            {
                static void call(const ConcreteType& asEnum, const Handler& handler)
                {
                    handler(asEnum.template as<T_type>());
                }
            };
            
        public:
            AsSwitch(const ConcreteType& asEnum, const bool handled) : m_asEnum(asEnum), m_handled(handled) {}
            explicit AsSwitch(const ConcreteType& asEnum) : AsSwitch(asEnum, false) {}
            
            template <Enum T_type, typename CaseHandler>
            AsSwitch<Enum, ConcreteType, CaseCast, T_type, types...> asCase(const CaseHandler& handler)
            {
                static_assert(!Contains<T_type, types...>::value, "Duplicated switch case");
                
                if (!m_handled && T_type == m_asEnum.type())
                {
                    m_handled = true;
                    
                    constexpr bool isVoid = std::is_same<typename CaseCast<T_type>::Type, void>::value;
                    HandlerCaller<T_type, CaseHandler, isVoid>::call(m_asEnum, handler);
                }
                
                return AsSwitch<Enum, ConcreteType, CaseCast, T_type, types...>(m_asEnum, m_handled);
            }
            
            template <typename Handler>
            void asDefault(const Handler& handler)
            {
                if (!m_handled)
                {
                    m_handled = true;
                    handler();
                }
            }
            
        private:
            const ConcreteType& m_asEnum;
            bool m_handled;
        };
    }
}
