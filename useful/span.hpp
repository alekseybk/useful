#pragma once
#include "meta.hpp"
#include "utils.hpp"

namespace uf
{
    template<typename Tp>
    class span
    {
        static_assert(std::is_same_v<std::remove_volatile_t<std::remove_reference_t<Tp>>, Tp>, "Span must have a non-reference, non-volatile value_type");

    public:
        using value_type = Tp;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        pointer data_ = nullptr;
        u64 size_ = 0;

    public:
        span() noexcept = default;

        span(const span&) noexcept = default;

        span& operator=(const span&) noexcept = default;

        template<typename T1>
        span(T1&& begin, u64 count) : data_(get_base_ptr(begin)), size_(count) { }

        template<typename T1, typename T2, disif<std::is_integral_v<std::decay_t<T2>>> = sdef>
        span(T1&& begin, T2&& end) : data_(get_base_ptr(begin))
        {
            static_assert (std::is_same_v<std::remove_reference_t<T1>, std::remove_reference_t<T2>>);
            static_assert (mt::is_random_access_iterator_v<std::remove_reference_t<T1>>);

            auto bptr = get_base_ptr(end);
            if (bptr < data_)
                throw std::out_of_range("span::span: End less then begin");
            if ((reinterpret_cast<u64>(bptr) - reinterpret_cast<u64>(data_)) % sizeof(Tp))
                throw std::out_of_range("span::span: Invalid end ptr");
            size_ = bptr - data_;
        }

        template<typename C>
        span(C&& container) : span(std::begin(container), std::end(container))
        {
            static_assert (mt::is_random_access_container_v<std::remove_reference_t<C>>);
        }

        template<typename T>
        span(std::initializer_list<T> l) : span(l.begin(), l.end()) { }

        template<typename T, u64 N>
        span(T(&arr)[N]) : span(arr, N) { }

        void clear() noexcept
        {
            data_ = nullptr;
            size_ = 0;
        }

        u64 size() const noexcept
        {
            return size_;
        }

        bool empty() const noexcept
        {
            return !size_;
        }

        span subspan(u64 begin = 0, u64 count = std::numeric_limits<u64>::max()) const
        {
            if (count == std::numeric_limits<u64>::max())
                return span(data_ + begin, data_ + size_);
            return span(data_ + begin, data_ + begin + count);
        }

        reference front() const
        {
            return data_[0];
        }

        reference back() const
        {
            return data_[size - 1];
        }

        pointer data() const noexcept
        {
            return data_;
        }

        reference operator[](u64 i) const
        {
            return data_[i];
        }

        reference at(u64 i) const
        {
            if (i >= size_)
                throw std::out_of_range("span: Out of range");
            return operator[](i);
        }

        iterator begin() const noexcept
        {
            return data_;
        }

        iterator end() const noexcept
        {
            return data_ + size_;
        }

        const_iterator cbegin() const noexcept
        {
            return data_;
        }

        const_iterator cend() const noexcept
        {
            return data_ + size_;
        }

        reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(data_ + size_);
        }

        reverse_iterator rend() const noexcept
        {
            return reverse_iterator(data_);
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator(data_ + size_);
        }

        const_reverse_iterator crend() const noexcept
        {
            return reverse_iterator(data_);
        }
    };

    template<typename T1>
    span(T1&&, u64) -> span<std::remove_pointer_t<decltype(get_base_ptr(std::declval<T1&&>()))>>;

    template<typename T1, typename T2>
    span(T1&&, T2&&) -> span<std::remove_pointer_t<decltype(get_base_ptr(std::declval<T1&&>()))>>;

    template<typename C, sfinae = sdef>
    span(C&&) -> span<std::remove_const_t<std::remove_reference_t<decltype(*std::begin(std::declval<C&&>()))>>>;

    template<typename T>
    span(std::initializer_list<T>) -> span<const T>;

    template<typename T, u64 N>
    span(T(&)[N]) -> span<T>;
}
// namespace uf
