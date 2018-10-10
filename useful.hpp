#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

using namespace std;

// https://github.com/elseecay/useful

#define UFER(what, ...) uf::err(what, __PRETTY_FUNCTION__, ##__VA_ARGS__)

namespace uf
{
    namespace internal
    {
        template<class>
        struct is_string_type_helper : public std::false_type {};

        template<>
        struct is_string_type_helper<char*> : public std::true_type {};

        template<>
        struct is_string_type_helper<const char*> : public std::true_type {};

        template<>
        struct is_string_type_helper<std::string> : public std::true_type {};

        template<>
        struct is_string_type_helper<std::string_view> : public std::true_type {};
    }

    using namespace internal;

    template<class Tp>
    struct is_string_type : public is_string_type_helper<std::decay_t<Tp>> {};

    template<class Tp>
    inline constexpr bool is_string_type_v = is_string_type<Tp>::value;

    namespace internal
    {
        template<size_t Index = 0, class... Errors>
        std::string tupleToMessage(const std::tuple<Errors...>& t)
        {
            std::string name_str = std::get<Index>(t), value_str;
            if constexpr (is_string_type_v<std::tuple_element_t<Index + 1, std::tuple<Errors...>>>)
                value_str = std::get<Index + 1>(t);
            else
                value_str = std::to_string(std::get<Index + 1>(t));
            if constexpr ((Index + 2) >= sizeof...(Errors))
                return name_str + " = " + value_str + "\n";
            else
                return name_str + " = " + value_str + "\n" + tupleToMessage<Index + 2, Errors...>(t);
        }
    }

    template<class Exception = std::runtime_error, class... Errors>
    void err(string_view what, Errors&&... p)
    {
        if constexpr (sizeof...(Errors) % 2)
        {
            static_assert(is_string_type_v<decltype(std::get<0>(std::tuple<Errors...>(p...)))>, "Second parameter should be function name");
            std::tuple<Errors...> t(std::forward<Errors>(p)...);
            if constexpr (sizeof...(Errors) > 1)
                throw Exception(string(what) + "\n" + string("in function: ") + string(get<0>(t)) + "\n" + internal::tupleToMessage<1, Errors...>(t));
            else
                throw Exception(string(what) + "\n" + string(" in function: ") + string(get<0>(t)) + "\n");
        }
        else
        {
            if constexpr (sizeof...(Errors))
                throw Exception(string(what) + "\n" + internal::tupleToMessage<0, Errors...>(std::tuple<Errors...>(std::forward<Errors>(p)...)));
            else
                throw Exception(string(what) + "\n");
        }
    }

    template<class Tp, bool = std::is_default_constructible_v<std::decay_t<Tp>>>
    class checkable
    {
    protected:

        using value_type = std::decay_t<Tp>;
        using reference = value_type&;
        using const_reference = const value_type&;
        using rvalue_reference = value_type&&;

        value_type m_val;
        bool m_flag = false;

    public:

        checkable() = default;

        template<class Tp_>
        checkable(Tp_&& value) : m_val(std::forward<Tp_>(value)), m_flag(true) {}

        template<class Tp_>
        const_reference operator=(Tp_&& value)
        {
            m_val = std::forward<Tp_>(value);
            m_flag = true;
        }

        operator reference() noexcept { return m_val; }

        operator const_reference() const noexcept { return m_val; }

        reference get() noexcept { return m_val; }

        const_reference get() const noexcept { return m_val; }

        bool check() const noexcept { return m_flag; }

        void set() noexcept { m_flag = true; }

        void unset() noexcept { m_flag = false; }
    };

    // partial specialization for false
    template<class Tp>
    class checkable<Tp, false> : public checkable<Tp, true>
    {
        using base = checkable<Tp, true>;

    public:

        using base::operator=;
        using base::operator typename base::reference;
        using base::operator typename base::const_reference;
        using base::get;
        using base::check;
        using base::set;
        using base::unset;

        template<class Tp_>
        checkable(Tp_&& value) : base(std::forward<Tp_>(value)) {}
    };

    template<class Tp>
    checkable(Tp&&) -> checkable<Tp>;

    template<class InputIterator, class Function>
    void for_each_fast(InputIterator b, InputIterator e, Function f)
    {
        typedef decltype(std::async(std::launch::async, std::for_each<InputIterator, Function>,
        std::declval<InputIterator>(), std::declval<InputIterator>(),std::declval<Function>())) future_type;

        const auto hc = thread::hardware_concurrency();
        const auto dst = distance(b, e);
        const auto per_thr = dst / hc;
        const auto mod = dst % hc;

        auto last = b;
        vector<future_type> handle;
        for (unsigned i = 0; i < hc - 1; ++i)
        {
            advance(last, per_thr + ((mod > i) ? 1 : 0));
            handle.push_back(async(std::launch::async, for_each<InputIterator, Function>, b, last, f));
            b = last;
        }
        for_each<InputIterator, Function>(b, e, f);
        for (auto& h : handle)
            h.wait();
    }
}
