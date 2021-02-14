#pragma once


namespace wh::mt
{
    namespace detail
    {
        template<u64>
        struct id_to_type_impl;

        template<typename>
        struct type_to_id_impl;
    }

    template<u64 N>
    using id_to_type = typename detail::id_to_type_impl<N>::type;

    template<typename Tp>
    constexpr u64 type_to_id = detail::type_to_id_impl<Tp>::value;
}
// namespace wh::mt

#define REGISTER_TYPEID(tp, id) \
    namespace wh::mt::detail \
    { \
        template<> \
        struct type_to_id_impl<tp> { static constexpr u64 value = id; }; \
        template<> \
        struct id_to_type_impl<id> { using type = tp; }; \
    } \


REGISTER_TYPEID(int, 1);
REGISTER_TYPEID(double, 2);
// ...


#undef REGISTER_TYPEID