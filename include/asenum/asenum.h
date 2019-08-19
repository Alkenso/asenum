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
#include <functional>

namespace asenum
{
    /// Case descriptor of single Associated Enum case.
    template <typename T_Enum, T_Enum T_Code, typename T>
    struct Case11
    {
        using Enum = T_Enum;
        static constexpr Enum Code = T_Code;
        using Type = T;
    };
    
#if __cplusplus > 201402L
    ///Case descriptor of single Associated Enum case. Convenient use with C++17 compiler.
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
        
        template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
        class AsMap;
        
        template <typename ConcreteAsEnum, template <typename T> class Cmp, typename... T_Cases>
        struct Comparator;
    }
    
    /**
     Associated Enum type.
     AsEnum should be specialized with single or multiple 'Case/Case11' types that represent associations.
     */
    template <typename... T_Cases>
    class AsEnum
    {
        static_assert(sizeof...(T_Cases) > 0, "Failed to instantiate AsEnum with no cases.");
        
    public:
        /// Related enum type.
        using Enum = typename details::CaseSet<T_Cases...>::Enum;
        
        /// Specific 'using' that allows to get accociated type with specific enum case.
        template <Enum C>
        using UnderlyingType = typename details::UnderlyingTypeResolver<Enum, C, T_Cases...>::type;
        
        /// Array of all cases associated with concrete AsEnum.
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
         @warning Usually ou don't want to use this method. Use safer 'ifCase'.
         Force unwraps AsEnum and provides direct access to value that it holds.
         
         @return Const reference to underlying value.
         @throws std::invalid_argument exception if 'Case' doesn't correspond to stored case.
         */
        template <Enum Case, typename R = UnderlyingType<Case>, typename = typename std::enable_if<!std::is_same<R, void>::value>::type>
        const R& forceAsCase() const;
        
        /**
         Performs switch-like action allowing to wotk with values of different cases.
         */
        details::AsSwitch<Enum, AsEnum<T_Cases...>> doSwitch() const;
        
        /**
         Maps (converts) AsEnum value depends on stored case to type 'T'.
         */
        template <typename T>
        details::AsMap<T, Enum, AsEnum<T_Cases...>> doMap() const;
        
        /**
         Check for equality two AsEnum instances. Instance meant to be equal if and only if
         1) Underlying enum cases are equal;
         2) Underlying values are equal.
         */
        bool operator==(const AsEnum& other) const;
        bool operator!=(const AsEnum& other) const;
        
        /**
         Compares two AsEnum instances.
         1) First compare types. If types differ, applies comparison to type itself;
         2) If types equal, compares underlying values.
         */
        bool operator<(const AsEnum& other) const;
        bool operator<=(const AsEnum& other) const;
        bool operator>(const AsEnum& other) const;
        bool operator>=(const AsEnum& other) const;
        
    private:
        AsEnum(const Enum relatedCase, std::shared_ptr<void> value);
        
        template <Enum Case, typename T>
        static AsEnum createImpl(T&& value);
        
        template <typename T, typename Handler>
        static typename std::enable_if<std::is_same<T, void>::value, void>::type call(const void*, const Handler& handler);
        
        template <typename T, typename Handler>
        static typename std::enable_if<!std::is_same<T, void>::value, void>::type call(const void* value, const Handler& handler);
        
    private:
        Enum m_enumCase;
        std::shared_ptr<void> m_value;
    };
    
    
    // Private details
    
    namespace details
    {
        template <typename T, size_t N>
        constexpr size_t ArraySize(T (&array)[N])
        {
            return sizeof(array) / sizeof(array[0]);
        }
        
        template <bool IsFinalStep, typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
        struct AsMapResultMaker;
        
        template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
        struct AsMapResultMaker<true, T, Enum, ConcreteAsEnum, Types...>
        {
            template <Enum T_type>
            static T
            makeResult(const ConcreteAsEnum&, std::unique_ptr<T> result)
            {
                if (!result)
                {
                    throw std::logic_error("Unexpected empty result. Please contact author and attach an example.");
                }
                
                return std::move(*result);
            }
        };
        
        template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
        struct AsMapResultMaker<false, T, Enum, ConcreteAsEnum, Types...>
        {
            template <Enum T_type>
            static AsMap<T, Enum, ConcreteAsEnum, T_type, Types...>
            makeResult(const ConcreteAsEnum& asEnum, std::unique_ptr<T> result)
            {
                return AsMap<T, Enum, ConcreteAsEnum, T_type, Types...>(asEnum, std::move(result));
            }
        };
        
        
        template<typename Enum, Enum...> struct Contains;
        template<typename Enum, Enum T_type> struct Contains<Enum, T_type> { static const bool value = false; };
        template<typename Enum, Enum T_type1, Enum T_type2> struct Contains<Enum, T_type1, T_type2> { static const bool value = T_type1 == T_type2; };
        
        template<typename Enum, Enum T_type1, Enum T_type2, Enum... T_other>
        struct Contains<Enum, T_type1, T_type2, T_other...> { static const bool value = Contains<Enum, T_type1, T_type2>::value || Contains<Enum, T_type2, T_other...>::value; };
        
        
        template <typename Enum, typename ConcreteAsEnum, Enum... Types>
        class AsSwitch
        {
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
        
        template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
        class AsMap
        {
            static constexpr size_t AllCaseCount = ArraySize(ConcreteAsEnum::AllCases);
            static constexpr size_t CurrentCaseCount = sizeof...(Types);
            static constexpr bool IsPreLastCase = AllCaseCount == CurrentCaseCount + 1;
            using ResultMaker = AsMapResultMaker<IsPreLastCase, T, Enum, ConcreteAsEnum, Types...>;
            
            template <Enum T_type>
            using IfCaseResult = typename std::conditional<IsPreLastCase, T, AsMap<T, Enum, ConcreteAsEnum, T_type, Types...>>::type;
            
            template <Enum T_type>
            using UnderlyingType = typename ConcreteAsEnum::template UnderlyingType<T_type>;
            
        public:
            AsMap(const ConcreteAsEnum& asEnum, std::unique_ptr<T> result);
            explicit AsMap(const ConcreteAsEnum& asEnum);
            
            template <Enum T_type, typename Handler, typename UT = UnderlyingType<T_type>>
            static typename std::enable_if<std::is_same<UT, void>::value, void>::type
            ifCaseCall(const ConcreteAsEnum& asEnum, std::unique_ptr<T>& result, const Handler& handler);
            
            template <Enum T_type, typename Handler, typename UT = UnderlyingType<T_type>>
            static typename std::enable_if<!std::is_same<UT, void>::value, void>::type
            ifCaseCall(const ConcreteAsEnum& asEnum, std::unique_ptr<T>& result, const Handler& handler);
            
            template <typename Handler>
            T ifDefault(const Handler& handler);
            
            template <Enum T_type, typename CaseHandler, typename R = IfCaseResult<T_type>>
            R ifCase(const CaseHandler& handler);
            
        private:
            std::unique_ptr<T> m_result;
            const ConcreteAsEnum& m_asEnum;
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
        
        
        template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case>
        struct Comparator<ConcreteAsEnum, Cmp, T_Case>
        {
            template <typename T>
            static typename std::enable_if<!std::is_same<T, void>::value, bool>::type
            compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second);
            
            template <typename T>
            static typename std::enable_if<std::is_same<T, void>::value, bool>::type
            compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second);
            
            static bool compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second, bool& finalVerdict);
            static bool compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second);
        };
        
        template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case, typename... T_Cases>
        struct Comparator<ConcreteAsEnum, Cmp, T_Case, T_Cases...>
        {
            static bool compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second);
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
template <typename asenum::AsEnum<T_Cases...>::Enum Case, typename R, typename>
const R& asenum::AsEnum<T_Cases...>::forceAsCase() const
{
    if (!isCase<Case>())
    {
        throw std::invalid_argument("Unwrapping case does not correspond to stored case.");
    }
    
    return *reinterpret_cast<const UnderlyingType<Case>*>(m_value.get());
}

template <typename... T_Cases>
asenum::details::AsSwitch<typename asenum::AsEnum<T_Cases...>::Enum, asenum::AsEnum<T_Cases...>> asenum::AsEnum<T_Cases...>::doSwitch() const
{
    return details::AsSwitch<Enum, AsEnum<T_Cases...>>(*this);
}

template <typename... T_Cases>
template <typename T>
asenum::details::AsMap<T, typename asenum::AsEnum<T_Cases...>::Enum, asenum::AsEnum<T_Cases...>> asenum::AsEnum<T_Cases...>::doMap() const
{
    return details::AsMap<T, Enum, AsEnum<T_Cases...>>(*this);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator==(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::equal_to, T_Cases...>::compare(*this, other);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator!=(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::not_equal_to, T_Cases...>::compare(*this, other);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator<(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::less, T_Cases...>::compare(*this, other);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator<=(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::less_equal, T_Cases...>::compare(*this, other);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator>(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::greater, T_Cases...>::compare(*this, other);
}

template <typename... T_Cases>
bool asenum::AsEnum<T_Cases...>::operator>=(const AsEnum& other) const
{
    return details::Comparator<AsEnum, std::greater_equal, T_Cases...>::compare(*this, other);
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

// Private details - AsSwitch

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
    static_assert(!Contains<Enum, T_type, Types...>::value, "Duplicated switch case.");
    
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

// Private details - AsMap

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::AsMap(const ConcreteAsEnum& asEnum, std::unique_ptr<T> result)
: m_result(std::move(result))
, m_asEnum(asEnum)
{}

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::AsMap(const ConcreteAsEnum& asEnum)
: m_asEnum(asEnum)
{}

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
template <Enum T_type, typename Handler, typename UT>
typename std::enable_if<std::is_same<UT, void>::value, void>::type
asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::ifCaseCall(const ConcreteAsEnum& asEnum, std::unique_ptr<T>& result, const Handler& handler)
{
    asEnum.template ifCase<T_type>([&] {
        result.reset(new T(handler()));
    });
}

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
template <Enum T_type, typename Handler, typename UT>
typename std::enable_if<!std::is_same<UT, void>::value, void>::type
asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::ifCaseCall(const ConcreteAsEnum& asEnum, std::unique_ptr<T>& result, const Handler& handler)
{
    asEnum.template ifCase<T_type>([&] (const UT& value) {
        result.reset(new T(handler(value)));
    });
}

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
template <typename Handler>
T asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::ifDefault(const Handler& handler)
{
    return m_result ? std::move(*m_result) : handler();
}

template <typename T, typename Enum, typename ConcreteAsEnum, Enum... Types>
template <Enum T_type, typename CaseHandler, typename R>
R asenum::details::AsMap<T, Enum, ConcreteAsEnum, Types...>::ifCase(const CaseHandler& handler)
{
    static_assert(!Contains<Enum, T_type, Types...>::value, "Duplicated map case");
    
    if (!m_result)
    {
        ifCaseCall<T_type>(m_asEnum, m_result, handler);
    }
    
    return ResultMaker::template makeResult<T_type>(m_asEnum, std::move(m_result));
}

// Private details - Comparator

template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case>
bool asenum::details::Comparator<ConcreteAsEnum, Cmp, T_Case>::compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second, bool& finalVerdict)
{
    // If AsEnums cases are NOT equal.
    if (first.enumCase() != second.enumCase())
    {
        finalVerdict = true;
        
        static constexpr Cmp<typename T_Case::Enum> s_comparator;
        return s_comparator(first.enumCase(), second.enumCase());
    }
    
    // Both AsEnums cases are equal. Check if they are equal to Comparator's case.
    if (first.enumCase() == T_Case::Code)
    {
        finalVerdict = true;
        
        return compare<typename T_Case::Type>(first, second);
    }
    
    return false;
}

template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case>
bool asenum::details::Comparator<ConcreteAsEnum, Cmp, T_Case>::compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second)
{
    bool finalVerdict = false;
    return compare(first, second, finalVerdict);
}

template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case>
template <typename T>
typename std::enable_if<!std::is_same<T, void>::value, bool>::type
asenum::details::Comparator<ConcreteAsEnum, Cmp, T_Case>::compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second)
{
    using UT = typename ConcreteAsEnum::template UnderlyingType<T_Case::Code>;
    static constexpr Cmp<UT> s_comparator;
    return s_comparator(first.template forceAsCase<T_Case::Code>(), second.template forceAsCase<T_Case::Code>());
}

template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case>
template <typename T>
typename std::enable_if<std::is_same<T, void>::value, bool>::type
asenum::details::Comparator<ConcreteAsEnum, Cmp, T_Case>::compare(const ConcreteAsEnum&, const ConcreteAsEnum&)
{
    static constexpr Cmp<bool> s_comparator;
    return s_comparator(true, true);
}

template <typename ConcreteAsEnum, template <typename T> class Cmp, typename T_Case, typename... T_Cases>
bool asenum::details::Comparator<ConcreteAsEnum, Cmp, T_Case, T_Cases...>::compare(const ConcreteAsEnum& first, const ConcreteAsEnum& second)
{
    bool finalVerdict = false;
    const bool result = Comparator<ConcreteAsEnum, Cmp, T_Case>::compare(first, second, finalVerdict);
    if (finalVerdict)
    {
        return result;
    }
    
    return Comparator<ConcreteAsEnum, Cmp, T_Cases...>::compare(first, second);
}
