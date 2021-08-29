#pragma once

#include <Variant.hpp>
namespace hasm
{
    struct independed_instruction {};

    static constexpr void mov_register(hsd::u64 src, hsd::u64& dest, independed_instruction)
    {
        dest = src;
    }

    static constexpr void add_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest += src;
    }

    static constexpr void add_fregister(hsd::f64 src, hsd::f64& dest)
    {
        dest += src;
    }

    static constexpr void sub_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest -= src;
    }

    static constexpr void sub_fregister(hsd::f64 src, hsd::f64& dest)
    {
        dest -= src;
    }

    static constexpr void mul_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest *= src;
    }

    static constexpr void mul_fregister(hsd::f64 src, hsd::f64& dest)
    {
        dest *= src;
    }

    static constexpr void div_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest /= src;
    }

    static constexpr void div_fregister(hsd::f64 src, hsd::f64& dest)
    {
        dest /= src;
    }

    static constexpr void mod_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest %= src;
    }
    
    static constexpr void lshift_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest <<= src;
    }

    static constexpr void rshift_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest >>= src;
    }

    static constexpr void and_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest &= src;
    }

    static constexpr void or_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest |= src;
    }

    static constexpr void xor_iregister(hsd::u64 src, hsd::u64& dest)
    {
        dest ^= src;
    }

    static constexpr void not_iregister(hsd::u64& dest)
    {
        dest = ~dest;
    }

    static constexpr void neg_iregister(hsd::u64& dest)
    {
        dest = -dest;
    }

    static constexpr void inc_iregister(hsd::u64& dest)
    {
        dest++;
    }

    static constexpr void dec_iregister(hsd::u64& dest)
    {
        dest--;
    }

    static constexpr void inc_fregister(hsd::f64& dest)
    {
        dest++;
    }

    static constexpr void dec_fregister(hsd::f64& dest)
    {
        dest--;
    }

    using instruction_storage = hsd::variant<
        void (*)(hsd::u64, hsd::u64&), 
        void (*)(hsd::f64, hsd::f64&), 
        void (*)(hsd::u64, hsd::u64&, independed_instruction),
        void (*)(hsd::u64&, independed_instruction), 
        void (*)(hsd::u64&), void (*)(hsd::f64&)
    >;
    
    using register_storage = hsd::variant<hsd::u64, hsd::f64>;
} // namespace hasm
