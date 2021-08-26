#pragma once

#include <Vector.hpp>
#include <String.hpp>
#include <Functional.hpp>
#include <UnorderedMap.hpp>

#include "Registers.hpp"

namespace hasm
{
    template<class... Ts> 
    struct overloaded : Ts...
    {
        using Ts::operator()...;
    };

    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    struct stack_frame
    {
        stack_frame() = default;
        stack_frame(const stack_frame&) = delete;
        stack_frame& operator=(const stack_frame&) = delete;

        stack_frame(stack_frame&&) = default;
        stack_frame& operator=(stack_frame&&) = default;

        hsd::unordered_map<hsd::string_view, register_storage> registers =
        {{
            hsd::pair{hsd::string_view{"rax"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rbx"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rcx"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rdx"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rsi"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rdi"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r8"}    , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r9"}    , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r10"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r11"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r12"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r13"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r14"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"r15"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rbp"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rip"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rsp"}   , register_storage{0ull}},
            hsd::pair{hsd::string_view{"rflags"}, register_storage{0ull}},
            hsd::pair{hsd::string_view{"xmm0"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm1"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm2"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm3"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm4"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm5"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm6"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm7"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm8"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm9"}  , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm10"} , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm11"} , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm12"} , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm13"} , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm14"} , register_storage{0.}},
            hsd::pair{hsd::string_view{"xmm15"} , register_storage{0.}}
        }};

        static inline hsd::unordered_map<hsd::string_view, instruction_storage> instructions =
        {{
            hsd::pair{hsd::string_view{"addi"}  , instruction_storage{add_iregister}},
            hsd::pair{hsd::string_view{"addf"}  , instruction_storage{add_fregister}},
            hsd::pair{hsd::string_view{"subi"}  , instruction_storage{sub_iregister}},
            hsd::pair{hsd::string_view{"subf"}  , instruction_storage{sub_fregister}},
            hsd::pair{hsd::string_view{"muli"}  , instruction_storage{mul_iregister}},
            hsd::pair{hsd::string_view{"mulf"}  , instruction_storage{mul_fregister}},
            hsd::pair{hsd::string_view{"divi"}  , instruction_storage{div_iregister}},
            hsd::pair{hsd::string_view{"divf"}  , instruction_storage{div_fregister}},
            hsd::pair{hsd::string_view{"modi"}  , instruction_storage{mod_iregister}},
            hsd::pair{hsd::string_view{"movi"}  , instruction_storage{mov_iregister}},
            hsd::pair{hsd::string_view{"movf"}  , instruction_storage{mov_fregister}},
            hsd::pair{hsd::string_view{"shr"}   , instruction_storage{lshift_iregister}},
            hsd::pair{hsd::string_view{"shl"}   , instruction_storage{rshift_iregister}},
            hsd::pair{hsd::string_view{"and"}   , instruction_storage{and_iregister}},
            hsd::pair{hsd::string_view{"or"}    , instruction_storage{or_iregister}},
            hsd::pair{hsd::string_view{"xor"}   , instruction_storage{xor_iregister}},
            hsd::pair{hsd::string_view{"not"}   , instruction_storage{not_iregister}},
            hsd::pair{hsd::string_view{"neg"}   , instruction_storage{neg_iregister}},
            hsd::pair{hsd::string_view{"inci"}  , instruction_storage{inc_iregister}},
            hsd::pair{hsd::string_view{"deci"}  , instruction_storage{dec_iregister}},
            hsd::pair{hsd::string_view{"incf"}  , instruction_storage{inc_fregister}},
            hsd::pair{hsd::string_view{"decf"}  , instruction_storage{dec_fregister}},
        }};
    };

    static constexpr hsd::Result<void, hsd::runtime_error> exec_instruction(
        instruction_storage& instruction, register_storage& register1)
    {
        return instruction.visit(
            overloaded {
                [&](void (*func)(hsd::u64&)) -> hsd::Result<void, hsd::runtime_error>
                {
                    auto dest = register1.get<hsd::u64>();

                    if (dest.is_ok())
                    {
                        func(dest.unwrap());
                        return {};
                    }
                    else
                    {
                        return hsd::runtime_error{"invalid register type"};
                    }
                },
                [&](void (*func)(hsd::f64&)) -> hsd::Result<void, hsd::runtime_error>
                {
                    auto dest = register1.get<hsd::f64>();

                    if (dest.is_ok())
                    {
                        func(dest.unwrap());
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

    static constexpr hsd::Result<void, hsd::runtime_error> exec_instruction(
        instruction_storage& instruction, register_storage& register1, register_storage& register2)
    {
        return instruction.visit(
            overloaded {
                [&](void (*func)(hsd::u64, hsd::u64&)) -> hsd::Result<void, hsd::runtime_error>
                {
                    auto src = register1.get<hsd::u64>();
                    auto dest = register2.get<hsd::u64>();

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
                [&](void (*func)(hsd::f64, hsd::f64&)) -> hsd::Result<void, hsd::runtime_error>
                {
                    auto src = register1.get<hsd::f64>();
                    auto dest = register2.get<hsd::f64>();

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

    class function_stack
    {
    private:
        stack_frame stack{};
        hsd::vector<hsd::string> exec_instructions;

    public:
        void add(const hsd::string& instructions)
        {
            exec_instructions.push_back(instructions);
        }

        function_stack() = default;
        function_stack(const function_stack&) = delete;
        function_stack& operator=(const function_stack&) = delete;

        function_stack(function_stack&&) = default;
        function_stack& operator=(function_stack&&) = default;
        
        hsd::Result<void, hsd::runtime_error> execute()
        {
            // instruct src, dst
            // instruct dst
            hsd::usize index = 0;

            while (index < exec_instructions.size())
            {
                if (exec_instructions[index].size() == 0) [[unlikely]]
                {
                    index++;
                    continue;
                }

                auto pos = exec_instructions[index].find(' ');

                if (pos == hsd::string_view::npos)
                {
                    return hsd::runtime_error{"invalid instruction"};
                }

                exec_instructions[index][pos] = '\0';
                hsd::string_view instruction = {exec_instructions[index].data(), pos};
                auto src_pos = exec_instructions[index].find(' ', pos + 1);

                if (src_pos == hsd::string_view::npos)
                {
                    auto dst = hsd::string_view{
                        exec_instructions[index].data() + pos + 1, src_pos - pos
                    };

                    exec_instruction(
                        stack.instructions.at(instruction).unwrap(), 
                        stack.registers.at(dst).unwrap()
                    ).unwrap();
                }
                else
                {
                    exec_instructions[index][src_pos] = '\0';
                    

                    auto src = hsd::string_view{
                        exec_instructions[index].data() + pos + 1, src_pos - pos
                    };
                    auto dst = hsd::string_view{
                        exec_instructions[index].data() + src_pos + 1, 
                        exec_instructions[index].size() - src_pos - 1 
                    };
                    auto src_res = stack.registers.at(src);

                    if (src_res.is_ok())
                    {
                        exec_instruction(
                            stack.instructions.at(instruction).unwrap(), 
                            src_res.unwrap(), stack.registers.at(dst).unwrap()
                        ).unwrap();
                    }
                    else if (src[0] == '0' && src[1] == 'x')
                    {
                        auto& dest = stack.registers.at(dst).unwrap();
                        register_storage _storage;

                        dest.visit(
                            overloaded {
                                [&](hsd::u64&)
                                {
                                    hsd::u64 dest = 0;
                                    sscanf(src.data(), "%llx", &dest);
                                    _storage = dest;
                                },
                                [&](hsd::f64&)
                                {
                                    hsd::u64 dest = 0;
                                    sscanf(src.data(), "%llx", &dest);
                                    _storage = hsd::bit_cast<hsd::f64>(dest);
                                }
                            }
                        );

                        exec_instruction(
                            stack.instructions[instruction],
                            _storage, dest
                        ).unwrap();
                    }
                    else if (src[0] >= '0' && src[0] <= '9')
                    {
                        auto& dest = stack.registers.at(dst).unwrap();
                        register_storage _storage;

                        dest.visit(
                            overloaded {
                                [&](hsd::u64&) -> hsd::Result<void, hsd::runtime_error>
                                {
                                    hsd::u64 dest = 0;
                                    sscanf(src.data(), "%llu", &dest);
                                    _storage = dest;
                                    return {};
                                },
                                [&](auto&) -> hsd::Result<void, hsd::runtime_error>
                                {
                                    return hsd::runtime_error{"invalid register type"};
                                }
                            }
                        ).unwrap();

                        exec_instruction(
                            stack.instructions.at(instruction).unwrap(),
                            _storage, dest
                        ).unwrap();
                    }
                    else
                    {
                        return hsd::runtime_error{src_res.unwrap_err()()};
                    }
                }

                index++;
            }

            return {};
        }
    };
} // namespace hasm
