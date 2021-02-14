#include "testing.hpp"

#include "../wh/utils.hpp"
#include "../wh/span.hpp"
#include "../wh/benchmark.hpp"

using namespace wh;

TEST(stf_or)
{
    {
        char c = 'a';
        assert_true(stf_or(c, 'a'));
        assert_true(stf_or(c, [](char c){ return c == 'a'; }));
        assert_true(stf_or(c, [](char){ return false; }, 'a'));
        assert_false(stf_or(c, [](char){ return false; }));
    }

    {
        int i = 5;
        assert_true(stf_or(i, 5));
        assert_true(stf_or(i, [](int i){ return i == 5; }));
        assert_true(stf_or(i, [](int){ return false; }, 5));
        assert_false(stf_or(i, [](int){ return false; }));
    }

    {
        std::string s = "12345";
        assert_true(stf_or(s, "12345"));
        assert_true(stf_or(s, [](const std::string& s){ return s == "12345"; }));
        assert_true(stf_or(s, [](const std::string&){ return false; }, [](const std::string& s){ return s == "12345"; }));
        assert_false(stf_or(s, [](const std::string&){ return false; }));
    }
}

TEST(get_base_ptr)
{
    {
        std::vector<int> v{1, 2, 3};
        auto i = v.begin() + 2;
        auto ri = v.rbegin();
        assert_eq(get_base_ptr(i), get_base_ptr(ri));
    }
}

TEST(span)
{
    {
        std::vector<int> v{2, 5};
        span s(v);
        assert_eq(s.size(), 2);
        assert_eq(s[0], 2);
        assert_eq(s[1], 5);
    }

    {
        std::string str("12");
        span s(str.data(), 2);
        const span<const char> cs(str);
        assert_eq(cs.size(), 2);
        assert_eq(cs[0], '1');
        assert_eq(cs[1], '2');
        s[0] = 'e';
        assert_eq(cs[0], 'e');
    }

    {
        std::vector<double> v{1.35, 1.112, 5.55};
        span s(v);
        auto j = s.crbegin();
        for (i64 i = 2; j != s.crend(); --i, ++j)
            assert_eq(v[i], *j);
    }

    {
        const int a1[5]{1, 2, 3, 4, 5};
        span s1(a1);
        assert_eq(s1.size(), 5);
        assert_eq(s1[3], 4);
    }

    {
        std::array<int, 5> a2{{1, 2, 3, 4, 5}};
        span s2(a2);
        assert_eq(s2.size(), 5);
        assert_eq(s2[3], 4);
    }

    {
        std::vector<int> v{1, 2, 3};
        span s1(v);
        span<const int> s2(s1);
        assert_true(s2[0] == 1 && s2[1] == 2 && s2[2] == 3);
    }

    {
        std::vector<int> v{1, 2, 3};
        span s = v;
        s = s.subspan(1);
        s = s.subspan(1);
        s = s.subspan(1);
        assert_eq(s.size(), 0);
        assert_eq(s.data(), v.data() + 3);
    }

    {
        const std::vector<int> v{2, 5};
        span s(v);
        static_assert (std::is_same_v<decltype(s)::value_type, const int>);
        assert_eq(s.size(), 2);
        assert_eq(s[0], 2);
        assert_eq(s[1], 5);
    }

    {
        auto f = [](span<const std::string>){ };
        span<std::string> s;
        f(s);
        span<const std::string> s2;
        s2 = s;
    }

    {
        std::vector<int> v{1};
        span s(v);
        for (int i = 0; i < 10; ++i)
            s = s.subspan(1);
        assert_eq(s.data(), v.data() + 1);
        assert_eq(s.size(), 0);
    }
}

TEST(subtuple)
{
    struct X
    {
        int a;
        X(int a) : a(a) { }
        X(X&&) = default;
        X(const X&){ assert(false); }
    };

    std::tuple<X, X, X> t(1, 2, 3);
    std::tuple<X, X> st = subtuple<1>(std::move(t));
    std::tuple<X> sst = subtuple<1>(std::move(st));
    assert_eq(std::get<0>(sst).a, 3);
}

TEST(subtuple_ref)
{
    struct X
    {
        int a;
        X(int a) : a(a) { }
        X(X&&){ assert(false); }
        X(const X&){ assert(false); }
    };

    std::tuple<X, X, X> t(1, 2, 3);
    std::tuple<X&&, X&&> st = subtuple_ref<1>(std::move(t));
    std::tuple<X&> sst = subtuple_ref<1>(st);
    assert_eq(std::get<0>(sst).a, 3);
}

TEST(subtuple_exclude_ref)
{
    struct X
    {
        int a;
        X(int a) : a(a) { }
        X(X&&){ assert(false); }
        X(const X&){ assert(false); }
    };

     std::tuple<X, X, X> t(1, 2, 3);
     std::tuple<X&&, X&&> st = subtuple_exclude_ref<0>(std::move(t));
     std::tuple<X&> sst = subtuple_exclude_ref<0>(st);
     assert_eq(std::get<0>(sst).a, 3);
}

TEST(tuple_for_each)
{
    {
        int a = 0;
        auto t1 = std::make_tuple(1, 2, 3);
        auto t2 = std::make_tuple(4, 5, 6);
        tuple_for_each([&a](auto& x, auto& y){ a += x * y; }, t1, t2);
        assert_eq(a, 32);
    }

    {
        struct X
        {
            int a;
            X(int a) : a(a) { }
            X(X&&) = default;
            X(const X&){ assert(false); }
        };

        int a = 0;
        std::tuple<X, X, X> t1 = std::make_tuple(1, 2, 3);
        std::tuple<X, X, X> t2 = std::make_tuple(4, 5, 6);
        tuple_for_each([&a](auto x, auto y){ a += x.a * y.a; }, std::move(t1), std::move(t2));
        assert_eq(a, 32);
    }
}

TEST(tuple_transform)
{
    {
        struct X
        {
            int a;
            X(int a) : a(a) { }
            X(X&&) = default;
            X(const X&){ assert(false); }
        };

        std::tuple<X, X, X> t1 = std::make_tuple(1, 2, 3);
        std::tuple<X, X, X> t2 = std::make_tuple(4, 5, 6);
        auto result = tuple_transform([](auto x, auto y){ return x.a * y.a; }, std::move(t1), std::move(t2));
        assert_eq(std::get<0>(result), 4);
        assert_eq(std::get<1>(result), 10);
        assert_eq(std::get<2>(result), 18);
    }
}

TEST(reversed)
{
    {
        std::vector v{1, 2, 3};
        std::vector<int> a;
        for (const auto& e : reversed(v))
            a.push_back(e);
        assert_eq(a[0], 3);
        assert_eq(a[1], 2);
        assert_eq(a[2], 1);
    }

    {
        std::vector<int> a;
        for (const auto& e : reversed(std::vector{1, 2, 3}))
            a.push_back(e);
        assert_eq(a[0], 3);
        assert_eq(a[1], 2);
        assert_eq(a[2], 1);
    }

    {
        const std::list<int> l{1, 2, 3};
        auto w = reversed(l);
        static_assert (std::is_same_v<decltype(*w.begin()), const int&>);
        int i = 3;
        for (auto e : w)
            assert_eq(i--, e);
    }
}

TEST(sort_indexes)
{
    {
        std::vector v{3, 2, 1};
        auto pos = sort_indexes(v.begin(), v.end(), std::less<int>());
        assert_eq(pos[0], 2);
        assert_eq(pos[1], 1);
        assert_eq(pos[2], 0);
    }

    {
        std::vector<int> v1(1000);
        iota(v1.begin(), v1.end(), 0);
        std::random_shuffle(v1.begin(), v1.end());
        auto v2 = v1;
        auto pos = sort_indexes(v1.begin(), v1.end(), std::less<int>());
        std::stable_sort(v2.begin(), v2.end(), std::less<int>());
        for (u64 i = 0; i < v1.size(); ++i)
            assert_eq(v1[pos[i]], v2[i]);
    }
}




