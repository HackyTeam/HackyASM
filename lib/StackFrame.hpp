#pragma once

#include <String.hpp>
#include <UnorderedMap.hpp>
#include <Functional.hpp>

#include "Registers.hpp"

namespace hasm
{
    // Forgive me for the following code.
    using namespace hsd::string_view_literals;

    enum class sinstruction_type
        : hsd::uchar
    {
        nop, ret, call, 
        jmp, jnz, push,
        pop, cmp_le,
        cmp_gt, cmp_eq,
    };

    struct stack_frame
    {
        stack_frame() = default;
        stack_frame(const stack_frame&) = delete;
        stack_frame& operator=(const stack_frame&) = delete;

        stack_frame(stack_frame&&) = default;
        stack_frame& operator=(stack_frame&&) = default;

        static inline hsd::unordered_map<
            hsd::string_view, hsd::function<void(hsd::vector<hsd::u64>&)>
        > extern_funcs;

        hsd::unordered_map<hsd::string_view, register_storage> registers =
        {{
            {"rax"_sv  , register_storage{0ull}},
            {"rbx"_sv  , register_storage{0ull}},
            {"rcx"_sv  , register_storage{0ull}},
            {"rdx"_sv  , register_storage{0ull}},
            {"rsi"_sv  , register_storage{0ull}},
            {"rdi"_sv  , register_storage{0ull}},
            {"r8"_sv   , register_storage{0ull}},
            {"r9"_sv   , register_storage{0ull}},
            {"r10"_sv  , register_storage{0ull}},
            {"r11"_sv  , register_storage{0ull}},
            {"r12"_sv  , register_storage{0ull}},
            {"r13"_sv  , register_storage{0ull}},
            {"r14"_sv  , register_storage{0ull}},
            {"r15"_sv  , register_storage{0ull}},
            {"rbp"_sv  , register_storage{0ull}},
            {"rip"_sv  , register_storage{0ull}},
            {"rsp"_sv  , register_storage{0ull}},
            {"zf"_sv   , register_storage{0ull}},
            {"xmm0"_sv , register_storage{0.00}},
            {"xmm1"_sv , register_storage{0.00}},
            {"xmm2"_sv , register_storage{0.00}},
            {"xmm3"_sv , register_storage{0.00}},
            {"xmm4"_sv , register_storage{0.00}},
            {"xmm5"_sv , register_storage{0.00}},
            {"xmm6"_sv , register_storage{0.00}},
            {"xmm7"_sv , register_storage{0.00}},
            {"xmm8"_sv , register_storage{0.00}},
            {"xmm9"_sv , register_storage{0.00}},
            {"xmm10"_sv, register_storage{0.00}},
            {"xmm11"_sv, register_storage{0.00}},
            {"xmm12"_sv, register_storage{0.00}},
            {"xmm13"_sv, register_storage{0.00}},
            {"xmm14"_sv, register_storage{0.00}},
            {"xmm15"_sv, register_storage{0.00}}
        }};

        static inline hsd::unordered_map<hsd::string_view, instruction_storage> instructions =
        {{
            {"addi"_sv, instruction_storage{add_iregister   }},
            {"addf"_sv, instruction_storage{add_fregister   }},
            {"subi"_sv, instruction_storage{sub_iregister   }},
            {"subf"_sv, instruction_storage{sub_fregister   }},
            {"muli"_sv, instruction_storage{mul_iregister   }},
            {"mulf"_sv, instruction_storage{mul_fregister   }},
            {"divi"_sv, instruction_storage{div_iregister   }},
            {"divf"_sv, instruction_storage{div_fregister   }},
            {"modi"_sv, instruction_storage{mod_iregister   }},    
            {"mov"_sv , instruction_storage{mov_register    }},
            {"shr"_sv , instruction_storage{lshift_iregister}},
            {"shl"_sv , instruction_storage{rshift_iregister}},
            {"and"_sv , instruction_storage{and_iregister   }},
            {"or"_sv  , instruction_storage{or_iregister    }},
            {"xor"_sv , instruction_storage{xor_iregister   }},
            {"not"_sv , instruction_storage{not_iregister   }},
            {"neg"_sv , instruction_storage{neg_iregister   }},
            {"inci"_sv, instruction_storage{inc_iregister   }},
            {"deci"_sv, instruction_storage{dec_iregister   }},
            {"incf"_sv, instruction_storage{inc_fregister   }},
            {"decf"_sv, instruction_storage{dec_fregister   }}
        }};

        static inline hsd::unordered_map<hsd::string_view, sinstruction_type> sinstructions =
        {{
            {"pop"_sv , sinstruction_type::pop   },
            {"push"_sv, sinstruction_type::push  },
            {"call"_sv, sinstruction_type::call  },
            {"nop"_sv , sinstruction_type::nop   },
            {"ret"_sv , sinstruction_type::ret   },
            {"jmp"_sv , sinstruction_type::jmp   },
            {"jnz"_sv , sinstruction_type::jnz   },
            {"cmpeq"  , sinstruction_type::cmp_eq},
            {"cmple"  , sinstruction_type::cmp_le},
            {"cmpgt"  , sinstruction_type::cmp_gt}
        }};
    };
} // namespace hasm
