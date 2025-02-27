// Copyright (c) 2020 by Robert Bosch GmbH. All rights reserved.
// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "iceoryx_hoofs/cxx/newtype.hpp"
#include "iceoryx_hoofs/testing/compile_test.hpp"
#include "test.hpp"

namespace
{
using namespace ::testing;
using namespace iox;
using namespace iox::cxx;

template <typename T>
struct Sut : public T
{
    using T::T;
    using T::operator=; // bring all operator= into scope

    // implement ctors and assignment operators when they are implemented by the base class
    // this is necessary to prevent warnings from some compilers
    Sut() noexcept = default;
    Sut(const Sut&) noexcept = default;
    Sut(Sut&&) noexcept = default;
    Sut& operator=(const Sut&) noexcept = default;
    Sut& operator=(Sut&&) noexcept = default;
};

static CompileTest compileTest(R"(
    #include \"iceoryx_hoofs/cxx/newtype.hpp\"

    template <typename T>
    struct Sut : public T
    {
        using T::T;
        using T::operator=; // bring all operator= into scope
    };

    using namespace iox;
    using namespace iox::cxx;
)",
                               {"iceoryx_hoofs/include"});


TEST(NewType, ComparableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::Comparable>> a(123), b(456);
    EXPECT_TRUE(a != b);
    EXPECT_FALSE(a == b);
}

TEST(NewType, DISABLED_NoComparableDoesNotCompile)
{
    const char* p = R"(
        Sut<NewType<int, newtype::ConstructByValueCopy>> a(123), b(456);
        if ( a == b ) {}
    )";
    EXPECT_FALSE(compileTest.verify(p));
}

TEST(NewType, SortableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, cxx::newtype::Sortable>> a(456), b(789);
    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a <= b);
    EXPECT_FALSE(a > b);
    EXPECT_FALSE(a >= b);
}

TEST(NewType, DefaultConstructableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::DefaultConstructable>> a;
}

TEST(NewType, CopyConstructableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::CopyConstructable, newtype::Comparable>> a(91), b(92),
        c(a);
    EXPECT_TRUE(a == c);
}

TEST(NewType, CopyAssignableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::CopyAssignable, newtype::Comparable>> a(491), b(492),
        c(423);

    b = a;
    EXPECT_TRUE(a == b);
}

TEST(NewType, MoveConstructableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::MoveConstructable, newtype::Comparable>> b(92), c(92),
        d(std::move(c));
    EXPECT_TRUE(b == d);
}

TEST(NewType, MoveAssignableDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::MoveAssignable, newtype::Comparable>> b(912), c(912),
        d(123);
    d = std::move(c);
    EXPECT_TRUE(b == d);
}

TEST(NewType, ConversionDoesCompile)
{
    Sut<cxx::NewType<int, newtype::ConstructByValueCopy, newtype::Convertable>> a(911);
    int b = static_cast<int>(a);
    EXPECT_THAT(b, Eq(911));
}

TEST(NewType, AssignByValueCopyDoesCompile)
{
    Sut<cxx::NewType<int, newtype::AssignByValueCopy, newtype::ConstructByValueCopy, newtype::Comparable>> a(8791),
        b(651);

    int blubb = 651;
    a = blubb;

    EXPECT_TRUE(a == b);
}

TEST(NewType, AssignByValueMoveDoesCompile)
{
    Sut<cxx::NewType<int, newtype::AssignByValueMove, newtype::ConstructByValueCopy, newtype::Comparable>> a(8791),
        b(651);

    int blubb = 651;
    a = std::move(blubb);

    EXPECT_TRUE(a == b);
}
} // namespace
