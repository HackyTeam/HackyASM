#include "lib/BasicIInterpreter.hpp"

int main()
{
    using namespace hsd::string_view_literals;
    hasm::basic_iinterpreter interpreter = "TestInter.hasm"_sv;
    
    interpreter.add_extern_func(
        "malloc"_sv, [](hsd::vector<hsd::u64>& args)
        {
            hasm::call(
                [&](hsd::u64 arg)
                {
                    args.push_back(reinterpret_cast<hsd::u64>(malloc(arg)));
                }, args
            );
        }
    );

    interpreter.add_extern_func(
        "free"_sv, [](hsd::vector<hsd::u64>& args)
        {
            hasm::call(
                [](hsd::u64 arg)
                {
                    free(reinterpret_cast<void*>(arg));
                }, args
            );
        }
    );

    interpreter.add_extern_func(
        "printlnf"_sv, [](hsd::vector<hsd::u64>& args)
        {
            hasm::call([](hsd::f64 arg) { hsd_println("{}", arg); }, args);
        }
    );

    interpreter.add_extern_func(
        "printlni"_sv, [](hsd::vector<hsd::u64>& args)
        {
            hasm::call([](hsd::u64 arg) { printf("%#0x\n", arg); }, args);
        }
    );

    interpreter.run();
}