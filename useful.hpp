#include <thread>
#include <future>
#include <vector>
#include <algorithm>

using namespace std;


namespace useful
{
    namespace meta
    {
        template<class Tp>
        struct is_string_type_helper : public std::false_type {};

        template<>
        struct is_string_type_helper<std::string> : public std::true_type {};

        template<>
        struct is_string_type_helper<const char*> : public std::true_type {};

        template<>
        struct is_string_type_helper<std::string_view> : public std::true_type {};

        template<class Tp>
        struct is_string_type : public is_string_type_helper<std::remove_cv_t<Tp>> {};

        template<class Tp>
        inline constexpr bool is_string_type_v = is_string_type<Tp>::value;
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
                std::string nameStr = std::get<Index>(t), valueStr;
                if constexpr (meta::is_string_type_v<std::tuple_element_t<Index + 1, std::tuple<Errors...>>>)
                    valueStr = std::get<Index + 1>(t);
                else
                    valueStr = std::to_string(std::get<Index + 1>(t));
                if constexpr ((Index + 2) >= sizeof...(Errors))
                    return nameStr + ": " + valueStr + "\n";
                else
                    return nameStr + ": " + valueStr + "\n" + tupleToMessage<Index + 2, Errors...>(t);
            }
        }

        template<class Exception = std::runtime_error, class... Errors>
        void err(const std::string& what, Errors... p)
        {
            static_assert(sizeof...(Errors) % 2 == 0);
            if constexpr (sizeof...(Errors))
                throw Exception(std::move(what) + "\n" + internal::tupleToMessage<0, Errors...>(std::tuple<Errors...>(p...)));
            else
                throw Exception(std::move(what) + "\n");
        }
    }
}
