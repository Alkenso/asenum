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

namespace asenum
{
    /**
     @class Case descriptor of single Associated Enum case.
     */
    template <typename T_Enum, T_Enum T_Code, typename T>
    struct Case11
    {
        using Enum = T_Enum;
        static constexpr Enum Code = T_Code;
        using Type = T;
    };
    
#if __cplusplus > 201402L
    /**
     @class Case descriptor of single Associated Enum case. Convenient use with C++17 compiler.
     */
    template <auto T_Code, typename T>
    using Case = Case11<decltype(T_Code), T_Code, T>;
#endif
    
    namespace details
    {
        template <typename... Cases>
        struct CaseSet;
        
        template <typename Enum, Enum Value, typename... Cases>
        struct UnderlyingTypeResolver;
        
        template <typename Enum, typename ConcreteAsEnum, Enum... types>
        class AsSwitch;
    }
    
    /**
     @class Associated Enum type.
     AsEnum should be specialized with single or multiple 'Case/Case11' types that represent associations.
     */
    template <typename... T_Cases>
    class AsEnum
    {
    public:
        /// Related enum type.
        using Enum = typename details::CaseSet<T_Cases...>::Enum;
        
        /// Specific 'using' that allows to get accociated type with specific enum case.
        template <Enum C>
        using UnderlyingType = typename details::UnderlyingTypeResolver<Enum, C, T_Cases...>::type;
        
        /// Array of all cases used associated with concrete AsEnum.
        static constexpr Enum AllCases[] = { T_Cases::Code... };
        
        /**
         Creates AsEnum instance of specific case.
         
         @param value Value related to specified enum case.
         @return AsEnum instance holding value of specified case.
         */
        template <Enum Case, typename U = typename std::enable_if<!std::is_same<UnderlyingType<Case>, void>::value>::type>
        static AsEnum create(UnderlyingType<Case> value) { return createImpl<Case, UnderlyingType<Case>>(std::move(value)); }
        
        /**
         Creates AsEnum instance of specific case with 'void' associated type.
         
         @return AsEnum instance holding value of specified case.
         */
        template <Enum Case, typename T = typename std::enable_if<std::is_same<UnderlyingType<Case>, void>::value>::type>
        static AsEnum create();
        
        /**
         @return enum case of current instance of AsEnum.
         */
        Enum enumCase() const;
        
        /**
         @return Boolean indicates if current instance of AsEnum holds exactly specified case...or not.
         */
        template <Enum Case>
        bool isCase() const;
        
        /**
         Unwraps AsEnum and provides access to value that it holds.
         
         @param handler Function or functional object of signature void(UnderlyingType<Case>)
         that accepts value/reference of type associated with specified case.
         @return Boolean indicates if handler has been called.
         */
        template <Enum Case, typename Handler>
        bool ifCase(const Handler& handler) const;
        
        /**
         Performs switch-like action allowing to wotk with values of different cases.
         */
        details::AsSwitch<Enum, AsEnum<T_Cases...>> doSwitch() const;
        
    private:
        AsEnum(const Enum relatedCase, std::shared_ptr<void> value);
        
        template <Enum Case, typename T>
        static AsEnum createImpl(T&& value);
        
        template <typename T, typename Handler>
        static typename std::enable_if<std::is_same<T, void>::value, void>::type call(const void*, const Handler& handler);
        
        template <typename T, typename Handler>
        static typename std::enable_if<!std::is_same<T, void>::value, void>::type call(const void* value, const Handler& handler);
        
    private:
        const Enum m_enumCase;
        const std::shared_ptr<void> m_value;
    };
    
    
    // Private details
    
    namespace details
    {
        template <typename Enum, typename ConcreteAsEnum, Enum... Types>
        class AsSwitch
        {
            template<Enum...> struct Contains;
            template<Enum T_type> struct Contains<T_type> { static const bool value = false; };
            template<Enum T_type1, Enum T_type2> struct Contains<T_type1, T_type2> { static const bool value = T_type1 == T_type2; };
            
            template<Enum T_type1, Enum T_type2, Enum... T_other>
            struct Contains<T_type1, T_type2, T_other...> { static const bool value = Contains<T_type1, T_type2>::value || Contains<T_type2, T_other...>::value; };
            
        public:
            AsSwitch(const ConcreteAsEnum& asEnum, const bool handled);
            explicit AsSwitch(const ConcreteAsEnum& asEnum);
            
            template <Enum T_type, typename CaseHandler>
            AsSwitch<Enum, ConcreteAsEnum, T_type, Types...> ifCase(const CaseHandler& handler);
            
            template <typename Handler>
            void ifDefault(const Handler& handler);
            
        private:
            const ConcreteAsEnum& m_asEnum;
            bool m_handled;
        };
        
        
        template <typename Case>
        struct CaseSet<Case>
        {
            using Enum = typename Case::Enum;
            static_assert(std::is_enum<Enum>::value, "All cases must relate to enum values.");
        };
        
        template <typename Case, typename... Cases>
        struct CaseSet<Case, Cases...>
        {
            static_assert(std::is_same<typename CaseSet<Case>::Enum, typename CaseSet<Cases...>::Enum>::value, "All cases must relate to the same enum.");
            using Enum = typename CaseSet<Cases...>::Enum;
        };
        
        
        template <typename Enum, Enum Value, typename... Cases>
        struct UnderlyingTypeResolver
        {
            template <Enum E, typename Default, typename... Args>
            struct TypeMap;
            
            template <Enum E, typename Default, typename T, typename... Args>
            struct TypeMap<E, Default, T, Args...>
            {
                using type = typename std::conditional<E == T::Code, typename T::Type, typename TypeMap<E, Default, Args...>::type>::type;
            };
            
            template <Enum E, typename Default, typename T>
            struct TypeMap<E, Default, T>
            {
                using type = typename std::conditional<E == T::Code, typename T::Type, Default>::type;
            };
            
            
            struct Dummy {};
            using type = typename TypeMap<Value, Dummy, Cases...>::type;
            static_assert(!std::is_same<type, Dummy>::value, "Type is missing for specified enum value.");
        };
        
    }
}


// AsEnum public

template <typename... T_Cases>
constexpr typename asenum::AsEnum<T_Cases...>::Enum asenum::AsEnum<T_Cases...>::AllCases[];

template <typename... T_Cases>
template <typename asenum::AsEnum<T_Cases...>::Enum Case, typename T>
asenum::AsEnum<T_Cases...> asenum::AsEnum<T_Cases...>::createImpl(T&& value)
{
    std::shared_ptr<void> internalValue(new T(std::forward<T>(value)), [] (void* ptr) {
        if (ptr)
        {
            delete reinterpret_cast<T*>(ptr);
        }
    });
    
    return asenum::AsEnum<T_Cases...>(Case, internalValue);
}

template <typename... T_Cases>
template <typename asenum::AsEnum<T_Cases...>::Enum Case, typename T>
asenum::AsEnum<T_Cases...> asenum::AsEnum<T_Cases...>::create()
{
    return asenum::AsEnum<T_Cases...>(Case, nullptr);
}

template <typename... T_Cases>
typename asenum::AsEnum<T_Cases...>::Enum asenum::AsEnum<T_Cases...>::enumCase() const
{
    return m_enumCase;
}

template <typename... T_Cases>
template <typename asenum::AsEnum<T_Cases...>::Enum Case>
bool asenum::AsEnum<T_Cases...>::isCase() const
{
    return Case == m_enumCase;
}

template <typename... T_Cases>
template <typename asenum::AsEnum<T_Cases...>::Enum Case, typename Handler>
bool asenum::AsEnum<T_Cases...>::ifCase(const Handler& handler) const
{
    const bool isType = isCase<Case>();
    if (isType)
    {
        call<UnderlyingType<Case>>(m_value.get(), handler);
    }
    
    return isType;
}

template <typename... T_Cases>
asenum::details::AsSwitch<typename asenum::AsEnum<T_Cases...>::Enum, asenum::AsEnum<T_Cases...>> asenum::AsEnum<T_Cases...>::doSwitch() const
{
    return details::AsSwitch<Enum, AsEnum<T_Cases...>>(*this);
}


// AsEnum private

template <typename... T_Cases>
asenum::AsEnum<T_Cases...>::AsEnum(const Enum relatedCase, std::shared_ptr<void> value)
: m_enumCase(relatedCase)
, m_value(value)
{}

template <typename... T_Cases>
template <typename T, typename Handler>
typename std::enable_if<std::is_same<T, void>::value, void>::type asenum::AsEnum<T_Cases...>::call(const void*, const Handler& handler)
{
    handler();
}

template <typename... T_Cases>
template <typename T, typename Handler>
typename std::enable_if<!std::is_same<T, void>::value, void>::type asenum::AsEnum<T_Cases...>::call(const void* value, const Handler& handler)
{
    handler(*reinterpret_cast<const T*>(value));
}


// Private details

template <typename Enum, typename ConcreteAsEnum, Enum... Types>
asenum::details::AsSwitch<Enum, ConcreteAsEnum, Types...>::AsSwitch(const ConcreteAsEnum& asEnum, const bool handled)
: m_asEnum(asEnum)
, m_handled(handled)
{}

template <typename Enum, typename ConcreteAsEnum, Enum... Types>
asenum::details::AsSwitch<Enum, ConcreteAsEnum, Types...>::AsSwitch(const ConcreteAsEnum& asEnum)
: AsSwitch(asEnum, false)
{}

template <typename Enum, typename ConcreteAsEnum, Enum... Types>
template <Enum T_type, typename CaseHandler>
asenum::details::AsSwitch<Enum, ConcreteAsEnum, T_type, Types...> asenum::details::AsSwitch<Enum, ConcreteAsEnum, Types...>::ifCase(const CaseHandler& handler)
{
    static_assert(!Contains<T_type, Types...>::value, "Duplicated switch case");
    
    if (!m_handled)
    {
        m_handled = m_asEnum.template ifCase<T_type>(handler);
    }
    
    return AsSwitch<Enum, ConcreteAsEnum, T_type, Types...>(m_asEnum, m_handled);
}

template <typename Enum, typename ConcreteAsEnum, Enum... Types>
template <typename Handler>
void asenum::details::AsSwitch<Enum, ConcreteAsEnum, Types...>::ifDefault(const Handler& handler)
{
    if (!m_handled)
    {
        m_handled = true;
        handler();
    }
}
