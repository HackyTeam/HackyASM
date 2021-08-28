#pragma once

#include <Vector.hpp>
#include <Functional.hpp>
#include "Registers.hpp"

namespace hasm
{
    namespace args_helper
    {
        template <typename>
        struct arg_type;

        template <typename T>
        requires (!std::is_class_v<T>)
        struct arg_type<T>
        {
            T value;
        };

        template <typename T>
        requires (std::is_class_v<T>)
        struct arg_type<T>
        {
            T* value;

            constexpr operator T&()
            {
                return *value;
            }
        };


        template <typename T>
        struct arg_type<T&>
        {
            T* value;

            constexpr operator T&()
            {
                return *value;
            }
        };

        template <typename T>
        struct arg_type<T&&>
        {
            T* value;

            constexpr operator T&()
            {
                return *value;
            }
        };

        template <typename>
        struct as_args_tup;

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_args_tup<Res(Scope::*)(Args...) noexcept(Case)>
        {
            using type = hsd::tuple<arg_type<Args>...>;
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_args_tup<Res(Scope::*)(Args...)& noexcept(Case)>
        { 
            using type = hsd::tuple<arg_type<Args>...>;
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_args_tup<Res(Scope::*)(Args...) const noexcept(Case)>
        { 
            using type = hsd::tuple<arg_type<Args>...>;
        };

        template <typename Res, typename Scope, bool Case, typename... Args>
        struct as_args_tup<Res(Scope::*)(Args...) const& noexcept(Case)>
        { 
            using type = hsd::tuple<arg_type<Args>...>;
        };

        static inline void as_arg(auto& arg, hsd::u64 value)
        {
            if constexpr (hsd::is_pointer<decltype(arg.value)>::value)
            {
                arg.value = reinterpret_cast<decltype(arg.value)>(value);
            }
            else
            {
                if constexpr (sizeof(decltype(arg.value)) == 8)
                {
                    arg.value = hsd::bit_cast<decltype(arg.value)>(value);
                }
                else if constexpr (sizeof(decltype(arg.value)) == 4)
                {
                    arg.value = hsd::bit_cast<decltype(arg.value)>(
                        static_cast<hsd::u32>(value)
                    );
                }
                else if constexpr (sizeof(decltype(arg.value)) == 2)
                {
                    arg.value = hsd::bit_cast<decltype(arg.value)>(
                        static_cast<hsd::u16>(value)
                    );
                }
                else
                {
                    arg.value = hsd::bit_cast<decltype(arg.value)>(
                        static_cast<hsd::u8>(value)
                    );
                }
            }
        }
    } // namespace args_helper
    
    template <typename... Ts> 
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    template <typename Func>
    static inline void call(Func&& func, hsd::vector<hsd::u64>& args)
    {
        if constexpr (requires {&Func::operator();})
        {
            typename args_helper::as_args_tup<
                decltype(&Func::operator())
            >::type args_tup;

            [&]<hsd::usize... Idx>(hsd::index_sequence<Idx...>)
            {
                ((
                    args_helper::as_arg(
                        args_tup.template get<Idx>(), args.back()
                    ), args.pop_back()
                ), ...);

                func(args_tup.template get<Idx>().value...);
            }(hsd::make_index_sequence<args_tup.size()>{});
        }
        else
        {
            typename args_helper::as_args_tup<Func>::type args_tup;

            [&]<hsd::usize... Idx>(hsd::index_sequence<Idx...>)
            {
                ((
                    args_helper::as_arg(
                        args_tup.template get<Idx>(), args.back()
                    ), args.pop_back()
                ), ...);

                func(args_tup.template get<Idx>().value...);
            }(hsd::make_index_sequence<args_tup.size()>{});
        }
    }

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
