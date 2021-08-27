#pragma once

#include "Handlers.hpp"
#include "StackFrame.hpp"

namespace hasm
{
    class function_stack
    {
    private:
        stack_frame stack{};
        static inline hsd::vector<hsd::u64> args_stack;
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
                auto pos = exec_instructions[index].find(' ');

                if (pos == hsd::string_view::npos)
                {
                    auto instr = stack
                        .sinstructions
                        .at(static_cast<hsd::string_view>(exec_instructions[index]))
                        .unwrap();

                    switch (instr)
                    {
                        case sinstruction_type::ret:
                        {
                            if (index != exec_instructions.size() - 1) [[unlikely]]
                            {
                                return hsd::runtime_error{
                                    "ret instruction must be the last instruction"
                                };
                            }

                            return {};
                        }
                        case sinstruction_type::nop:
                        {
                            index++;
                            continue;
                        }
                        default:
                        {
                            return hsd::runtime_error{"Invalid instruction"};
                        }
                    }
                }

                exec_instructions[index][pos] = '\0';
                hsd::string_view instruction = {exec_instructions[index].data(), pos};
                auto src_pos = exec_instructions[index].find(' ', pos + 1);

                if (src_pos == hsd::string_view::npos)
                {
                    auto dst = hsd::string_view{
                        exec_instructions[index].data() + pos + 1, 
                        exec_instructions[index].size() - pos - 1
                    };

                    auto instr = stack.instructions.at(instruction);

                    if (instr.is_ok() == true)
                    {
                        exec_instruction(
                            instr.unwrap(), 
                            stack.registers.at(dst).unwrap()
                        ).unwrap();
                    }
                    else
                    {
                        switch (stack.sinstructions.at(instruction).unwrap())
                        {
                            case sinstruction_type::pop:
                            {
                                pop_stack(args_stack, stack.registers.at(dst).unwrap());
                                break;
                            }
                            case sinstruction_type::push:
                            {
                                push_stack(args_stack, stack.registers.at(dst).unwrap());
                                break;
                            }
                            default:
                            {
                                return hsd::runtime_error{"Invalid instruction"};
                            }
                        }
                    }
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
                            [&]<typename T>(T&)
                            {
                                hsd::u64 dest = 0;
                                sscanf(src.data(), "%llx", &dest);
                                _storage = hsd::bit_cast<T>(dest);
                            }
                        );

                        exec_instruction(
                            stack.instructions
                            .at(instruction).unwrap(),
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

            return hsd::runtime_error{"Unexpected end of instructions"};
        }
    };
} // namespace hasm
