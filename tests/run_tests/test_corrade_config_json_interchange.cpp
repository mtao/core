#include <iostream>
#include <mtao/utils/corrade_configuration_json_interchange.hpp>


int main(int argc, char *argv[]) {


    Corrade::Utility::Configuration config{ argv[1] };

    {
        auto js = mtao::utils::corrade_configuration_to_json(config);
        std::cout << js.dump(2) << std::endl;

        auto config2 = mtao::utils::json_to_corrade_configuration(js);


        config2.save(std::cout);
    }
    {
        auto js = mtao::utils::corrade_configuration_to_json(config, true);
        std::cout << js.dump(2) << std::endl;

        auto config2 = mtao::utils::json_to_corrade_configuration(js);


        config2.save(std::cout);
    }

    return 0;
}

