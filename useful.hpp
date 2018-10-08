#include <thread>
#include <future>
#include <vector>
#include <algorithm>
#include <memory>
#include <iostream>

using namespace std;

// https://github.com/elseecay/useful

#define UFER(name, ...) (uf::fn::err(name, __PRETTY_FUNCTION__, __VA_ARGS__));

namespace uf
{
    namespace meta
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

        template<class Tp>
        struct is_string_type : public is_string_type_helper<std::decay_t<Tp>> {};

        template<class Tp>
        inline constexpr bool is_string_type_v = is_string_type<Tp>::value;
    }

    namespace cls
    {
        template<class Tp>
        class StackMemory
        {
            uint8_t m_memory[sizeof(Tp)];
        public:
            Tp& ref = *static_cast<Tp*>(static_cast<void*>(m_memory));
        };

        template<class Tp>
        class Nullable
        {
            using value_type = std::decay_t<Tp>;
            bool m_isset{false};
            uint8_t m_memory[sizeof(value_type)]{};
            value_type& m_ref = *static_cast<value_type*>(static_cast<void*>(m_memory));
        public:
            Nullable() = default;
            template<class Tp_>
            Nullable(Tp_&& value) : m_isset(true)
            {
                m_ref = std::forward<Tp_>(value);
            }
            template<class Tp_>
            value_type& operator=(Tp_&& value)
            {
                m_isset = true;
                m_ref = std::forward<Tp_>(value);
                return m_ref;
            }
            value_type& operator()() const noexcept
            {
                return m_ref;
            }
            void operator-() noexcept
            {
                m_isset = false;
            }
            void operator+() noexcept
            {
                m_isset = true;
            }
            bool operator~() const noexcept
            {
                return m_isset;
            }
            operator value_type&() const noexcept
            {
                return m_ref;
            }
        };

        template<class Tp>
        Nullable(Tp&&) -> Nullable<std::decay_t<Tp>>;
    }

    namespace fn
    {
        template<class InputIterator, class Function>
        void for_each_fast(InputIterator b, InputIterator e, Function f)
        {
            typedef decltype(std::async(std::launch::async, std::for_each<InputIterator, Function>,
                                        std::declval<InputIterator>(), std::declval<InputIterator>(),
                                        std::declval<Function>())) Future;

            const auto hc = thread::hardware_concurrency();
            const auto dst = distance(b, e);
            const auto per_thr = dst / hc;
            const auto mod = dst % hc;

            auto last = b;
            vector<Future> handle;
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

        namespace internal
        {
            template<size_t Index = 0, class... Errors>
            std::string tupleToMessage(const std::tuple<Errors...>& t)
            {
                std::string name_str = std::get<Index>(t), value_str;
                if constexpr (meta::is_string_type_v<std::tuple_element_t<Index + 1, std::tuple<Errors...>>>)
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
                static_assert(meta::is_string_type_v<decltype(std::get<0>(std::tuple<Errors...>(p...)))>, "Second parameter should be function name");
                std::tuple<Errors...> t(std::forward<Errors>(p)...);
                if constexpr (sizeof...(Errors) > 1)
                    throw Exception(string(what) + string(" in function: ") + string(get<0>(t)) + "\n" + internal::tupleToMessage<1, Errors...>(t));
                else
                    throw Exception(string(what) + string(" in function: ") + string(get<0>(t)) + "\n");
            }
            else
            {
                if constexpr (sizeof...(Errors))
                    throw Exception(string(what) + "\n" + internal::tupleToMessage<0, Errors...>(std::tuple<Errors...>(std::forward<Errors>(p)...)));
                else
                    throw Exception(string(what) + "\n");
            }
        }
    }
}
