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

#include <assenum/assenum.h>

#include <gtest/gtest.h>

#include <string>

using namespace ::testing;

namespace
{
    enum class TestEnum
    {
        Unknown,
        StringOpt,
        BoolOpt
    };
    
    ASSENUM_DECLARE(TestAssEnum, TestEnum)
    {
        ASSENUM_DEFINE_STRUCTORS();
        
        ASSENUM_CASE_CC(Unknown, int);
        ASSENUM_CASE_CC(StringOpt, std::string);
        ASSENUM_CASE_CC(BoolOpt, bool);
    };
}

TEST(AssEnum, SimpleTest)
{
    const TestAssEnum value1 = TestAssEnum::CreateStringOpt("test");
    const TestAssEnum value2 = TestAssEnum::CreateBoolOpt(true);
    const TestAssEnum value3 = TestAssEnum::CreateUnknown(-100500);
    
    EXPECT_EQ(value1.type(), TestEnum::StringOpt);
    EXPECT_EQ(value2.type(), TestEnum::BoolOpt);
    EXPECT_EQ(value3.type(), TestEnum::Unknown);
    
    EXPECT_EQ(value1.StringOpt(), "test");
    EXPECT_THROW(value1.Unknown(), std::exception);
    EXPECT_THROW(value1.BoolOpt(), std::exception);
    
    EXPECT_EQ(value2.BoolOpt(), true);
    EXPECT_THROW(value2.Unknown(), std::exception);
    EXPECT_THROW(value2.StringOpt(), std::exception);
    
    EXPECT_EQ(value3.Unknown(), -100500);
    EXPECT_THROW(value3.StringOpt(), std::exception);
    EXPECT_THROW(value3.BoolOpt(), std::exception);
    
}
