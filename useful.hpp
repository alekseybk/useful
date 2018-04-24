#include <thread>
#include <future>
#include <vector>
#include <algorithm>

using namespace std;


namespace useful
{
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
    }

    namespace cls
    {
        class AbstractValue
        {
        protected:
            bool m_isset;
        public:
            void set() noexcept
            {
                m_isset = true;
            }
            void unset() noexcept
            {
                m_isset = false;
            }
            bool isset() const noexcept
            {
                return m_isset;
            }
            AbstractValue(bool isset) noexcept : m_isset(isset)
            {

            }
        };

        template<class Tp>
        class Value : public AbstractValue
        {
            Tp m_value;
        public:
            Value() : AbstractValue(false)
            {

            }
            Value(const Tp& value) : AbstractValue(true), m_value(value)
            {

            }
            template<class V>
            void set(V&& value) noexcept
            {
                m_isset = true;
                m_value = std::forward<V>(value);
            }
            const Tp& get() const noexcept
            {
                return m_value;
            }
        };
    }

    namespace meta
    {
        template<class Tp>
        struct is_string_type : public std::false_type {};

        template<>
        struct is_string_type<std::string> : public std::true_type {};

        template<>
        struct is_string_type<char const*> : public std::true_type {};

        template<>
        struct is_string_type<std::string_view> : public std::true_type {};

        template <class Tp>
        inline constexpr bool is_string_type_v = is_string_type<Tp>::value;
    }
}
