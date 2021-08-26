#include "lib/BasicIInterpreter.hpp"

int main()
{
    using namespace hsd::string_view_literals;
    hasm::basic_iinterpreter interpreter = "TestInter.hasm"_sv;
    interpreter.run();
}