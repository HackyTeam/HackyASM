#pragma once

#include <Vector.hpp>
#include "Registers.hpp"

namespace hasm
{
    template <typename... Ts> 
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    static inline void push_stack(hsd::vector<hsd::u64>& stack, register_storage& reg_val)
    {
        stack.push_back(
            reg_val.visit(
                [](auto& value)
                {
                    return hsd::bit_cast<hsd::u64>(value);
                } 
            )
        );
    }

    static inline void pop_stack(hsd::vector<hsd::u64>& stack, register_storage& reg_val)
    {
        if (stack.size() == 0)
        {
            hsd_fprint_check(stderr, "Stack is empty.\n");
            abort();
        }

        reg_val.visit(
            [&]<typename T>(T& value)
            {
                value = hsd::bit_cast<T>(stack.back());
            } 
        );

        stack.pop_back();
    }

    static constexpr hsd::Result<void, hsd::runtime_error> exec_instruction(
        instruction_storage& instr, register_storage& r1)
    {
        return instr.visit(
            overloaded {
                [&](void (*func)(hsd::u64&)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    auto dest = r1.get<hsd::u64>();

                    if (dest.is_ok())
                    {
                        func(dest.unwrap());
                        return {};
                    }
                    else
                    {
                        return hsd::runtime_error{"Invalid register type"};
                    }
                },
                [&](void (*func)(hsd::u64&, independed_instruction)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    return r1.visit(
                        [&](auto& value) 
                            -> hsd::Result<void, hsd::runtime_error>
                        {
                            using value_t = hsd::remove_cvref_t<decltype(value)>;
                            auto dest = hsd::bit_cast<hsd::u64>(value);
                            func(dest, {});
                            value = hsd::bit_cast<value_t>(dest);
                            return {};
                        }
                    );
                },
                [&](void (*func)(hsd::f64&)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    auto dest = r1.get<hsd::f64>();

                    if (dest.is_ok())
                    {
                        func(dest.unwrap());
                        return {};
                    }
                    else
                    {
                        return hsd::runtime_error{"Invalid register type"};
                    }
                },
                [&](auto&&) 
                -> hsd::Result<void, hsd::runtime_error>
                {
                    return hsd::runtime_error{"Invalid instruction"};
                }
            }
        );
    } 

    static constexpr hsd::Result<void, hsd::runtime_error> exec_instruction(
        instruction_storage& instr, register_storage& r1, register_storage& r2)
    {
        return instr.visit(
            overloaded {
                [&](void (*func)(hsd::u64, hsd::u64&)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    auto src = r1.get<hsd::u64>();
                    auto dest = r2.get<hsd::u64>();

                    if (src.is_ok() && dest.is_ok())
                    {
                        func(src.unwrap(), dest.unwrap());
                        return {};
                    }
                    else
                    {
                        return hsd::runtime_error{"invalid register type"};
                    }
                },
                [&](void (*func)(hsd::u64, hsd::u64&, independed_instruction)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    return r2.visit(
                        [&](auto& value) 
                            -> hsd::Result<void, hsd::runtime_error>
                        {
                            using value_t = hsd::remove_cvref_t<decltype(value)>;
                            
                            auto src = r1.visit(
                                [](auto&& value)
                                {
                                    return hsd::bit_cast<hsd::u64>(value);
                                }
                            );

                            auto dest = hsd::bit_cast<hsd::u64>(value);
                            func(src, dest, {});
                            value = hsd::bit_cast<value_t>(dest);
                            return {};
                        }
                    );
                },
                [&](void (*func)(hsd::f64, hsd::f64&)) 
                    -> hsd::Result<void, hsd::runtime_error>
                {
                    auto src = r1.get<hsd::f64>();
                    auto dest = r2.get<hsd::f64>();

                    if (src.is_ok() && dest.is_ok())
                    {
                        func(src.unwrap(), dest.unwrap());
                        return {};
                    }
                    else
                    {
                        return hsd::runtime_error{"invalid register type"};
                    }
                },
                [&](auto&&) -> hsd::Result<void, hsd::runtime_error>
                {
                    return hsd::runtime_error{"invalid instruction"};
                }
            }
        );
    } 
} // namespace hasm
