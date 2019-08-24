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

        constexpr span(Tp* begin, u64 count) : data_(begin), size_(count) { }

        constexpr span(Tp* begin, Tp* end) : data_(begin)
        {
            if (end < begin)
                throw std::out_of_range("span::span: Invalid [begin, end) range");
            if ((reinterpret_cast<u64>(end) - reinterpret_cast<u64>(begin)) % sizeof(Tp))
                throw std::out_of_range("span::span: Invalid [begin, end) range");
            size_ = end - begin;
        }

        template<typename Container>
        constexpr span(Container& container) : span(container.data(), container.size()) { }

        template<typename T>
        constexpr span(std::initializer_list<T> l) : span(l.begin(), l.end()) { }

        template<u64 N>
        constexpr span(Tp(&arr)[N]) : span(arr, N) { }

        constexpr void clear() noexcept
        {
            data_ = nullptr;
            size_ = 0;
        }

        constexpr u64 size() const noexcept
        {
            return size_;
        }

        constexpr bool empty() const noexcept
        {
            return !size_;
        }

        constexpr span subspan(u64 begin, u64 count = std::numeric_limits<u64>::max()) const
        {
            if (count == std::numeric_limits<u64>::max())
                return span(data_ + begin, data_ + size_);
            return span(data_ + begin, data_ + begin + count);
        }

        constexpr reference front() const
        {
            return data_[0];
        }

        constexpr reference back() const
        {
            return data_[size_ - 1];
        }

        constexpr pointer data() const noexcept
        {
            return data_;
        }

        constexpr reference operator[](u64 i) const
        {
            return data_[i];
        }

        constexpr reference at(u64 i) const
        {
            if (i >= size_)
                throw std::out_of_range("span: Out of range");
            return operator[](i);
        }

        constexpr iterator begin() const noexcept
        {
            return data_;
        }

        constexpr iterator end() const noexcept
        {
            return data_ + size_;
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return data_;
        }

        constexpr const_iterator cend() const noexcept
        {
            return data_ + size_;
        }

        constexpr reverse_iterator rbegin() const noexcept
        {
            return reverse_iterator(data_ + size_);
        }

        constexpr reverse_iterator rend() const noexcept
        {
            return reverse_iterator(data_);
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return reverse_iterator(data_ + size_);
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return reverse_iterator(data_);
        }
    };

    template<typename Container>
    span(Container& container) -> span<std::remove_pointer_t<decltype(container.data())>>;

    template<typename T>
    span(std::initializer_list<T>) -> span<const T>;
}
// namespace uf
