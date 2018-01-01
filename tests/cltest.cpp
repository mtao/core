#include "mtao/cmdline_parser.hpp"


int main(int argc, char * argv[]) {

    mtao::CommandLineParser clp;
    clp.add_option("name", std::string("tom"));
    clp.add_option("age", 18);
    clp.add_option("cage");
    clp.add_option("happiness", 5.5);
    clp.add_alias("age","a");

    clp.parse(argc,argv);


}
