#pragma once

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>
#include <fmt/format.h>

template <typename T>
struct AtLeastMatcher : Catch::Matchers::MatcherGenericBase
{
    AtLeastMatcher(T atLeast, T tolerance)
        : m_atLeast{atLeast}, m_tolerance(tolerance)
    {
    }

    bool match(T value) const
    {
        return value >= m_atLeast && value <= m_atLeast + m_tolerance;
    }

    std::string describe() const override
    {
        return fmt::format("Is at least {}, with absolute tolerance {}",
                           m_atLeast, m_tolerance);
    }

  private:
    T m_atLeast;
    T m_tolerance;
};

template <typename T> auto AtLeast(T atLeast, T tolerance) -> AtLeastMatcher<T>
{
    return AtLeastMatcher<T>(atLeast, tolerance);
}