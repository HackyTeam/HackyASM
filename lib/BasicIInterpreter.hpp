#pragma once

#include "FunctionStack.hpp"
#include <Io.hpp>

namespace hasm
{
    class basic_iinterpreter
    {
    private:
        function_stack stack{};
        hsd::io file;

    public:
        basic_iinterpreter(const hsd::string_view& fiename)
            : file{hsd::io::load_file(fiename.data(), hsd::io::options::text::read).unwrap()}
        {}

        void run()
        {
            while (file.is_eof() == false)
            {
                auto& line = file.read_line().unwrap();
                
                auto* line_end = hsd::cstring::find_or_end(line.data(), '\n');
                auto line_size = static_cast<hsd::usize>(line_end - line.data());
                line.data()[line_size] = '\0';

                stack.add({line.data(), line_size});
            }

            stack.execute().unwrap();
        }
    };
} // namespace hasm
