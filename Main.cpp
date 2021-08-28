#include "lib/BasicIInterpreter.hpp"

int main()
{
    using namespace hsd::string_view_literals;
    hasm::basic_iinterpreter interpreter = "TestInter.hasm"_sv;
    
    interpreter.add_extern_func(
        "printlnf"_sv, [](hsd::vector<hsd::u64>& args)
        {
            hasm::call([](hsd::f32 arg) { hsd_println("{}", arg); }, args);
        }
    );

    interpreter.run();
}