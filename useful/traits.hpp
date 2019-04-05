#pragma once
#include "meta.hpp"

#define DECLARE_META_BOOL_CONSTANT(name) \
    static constexpr bool name##_dc = mt::name<decayed>::value; \
    static constexpr bool name##_rr = mt::name<ref_removed>::value; \
    static constexpr bool name = mt::name<type>::value

#define DECLARE_STD_BOOL_CONSTANT(name) \
    static constexpr bool name##_dc = std::name<decayed>::value; \
    static constexpr bool name##_rr = std::name<ref_removed>::value; \
    static constexpr bool name = std::name<type>::value

#define DECLARE_META_TYPE(name) \
    using name##_dc = typename mt::name<decayed>::type; \
    using name##_rr = typename mt::name<ref_removed>::type; \
    using name = typename mt::name<type>::type

#define DECLARE_STD_TYPE(name) \
    using name##_dc = typename std::name<decayed>::type; \
    using name##_rr = typename std::name<ref_removed>::type; \
    using name = typename std::name<type>::type

namespace uf
{
    template<typename Tp>
    struct traits
    {
        using type = Tp;
        using ref_removed = typename std::remove_reference<type>::type;
        using decayed = typename std::remove_cv<ref_removed>::type;

        DECLARE_META_BOOL_CONSTANT(is_iterable);
        DECLARE_META_BOOL_CONSTANT(is_iterator);
        DECLARE_META_BOOL_CONSTANT(is_random_access_iterator);
        DECLARE_META_BOOL_CONSTANT(is_random_access_container);
        DECLARE_META_BOOL_CONSTANT(is_positive_sequence);

        //DECLARE_META_TYPE()

        DECLARE_STD_BOOL_CONSTANT(is_integral);
        DECLARE_STD_BOOL_CONSTANT(is_floating_point);
        DECLARE_STD_BOOL_CONSTANT(is_array);
        DECLARE_STD_BOOL_CONSTANT(is_reference);
        DECLARE_STD_BOOL_CONSTANT(is_lvalue_reference);
        DECLARE_STD_BOOL_CONSTANT(is_rvalue_reference);
        DECLARE_STD_BOOL_CONSTANT(is_const);

        DECLARE_STD_TYPE(remove_reference);
        DECLARE_STD_TYPE(remove_const);
        DECLARE_STD_TYPE(remove_cv);
    };
}
// namespace uf

#undef DECLARE_META_BOOL_CONSTANT
#undef DECLARE_STD_BOOL_CONSTANT
#undef DECALARE_META_TYPE
#undef DECLARE_STD_TYPE
