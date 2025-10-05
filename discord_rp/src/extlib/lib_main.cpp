#include <iostream>

#include "lib_recomp.hpp"

extern "C" {
    DLLEXPORT uint32_t recomp_api_version = 1;
}

RECOMP_DLL_FUNC(native_lib_test) {
    std::string mod_text = RECOMP_ARG_STR(0);

    std::cout << mod_text << "\n";
    RECOMP_RETURN(int, 0);
}
