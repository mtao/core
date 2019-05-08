#include "mtao/cmdline_parser.hpp"
#include <algorithm>
#include <type_traits>
#include <stdexcept>
#include <iostream>
#include <set>
#include <iterator>
#include <cstdlib>
#include <sstream>
#include "mtao/type_utils.h"
#include "mtao/logging/logger.hpp"

using namespace mtao::logging;

namespace mtao {

    namespace clp_internal {
        void clp_inline_test(const CommandLineParser& clp) {
            {
                auto t = trace();
                t << "Arguments(" << clp.args().size() << "): ";
                int count = 0;
                for(auto&& s: clp.args()) {
                    t<< "[" <<count++ << ":{" << s << "}] ";
                }
            }

            {
                auto t  = trace();

                t << "Options(" << clp.opts().size() << "): ";
                for(auto&& opt: clp.opts()) {
                    t << "[" << opt.first <<"(" 
                        << clp.opt_type(opt.first) <<"):{" ;
                    t << clp.opt_str(opt.first);
                    t << "}] ";

                }
            }
        }
    }

    CommandLineParser::CommandLineParser() {
        m_opts["help"] = Option(false,"Print help");
    }

    bool CommandLineParser::parse(int argc, char * argv[]) {

        std::vector<std::string> toks(argc);
        std::copy(argv,argv+argc,toks.begin());
        //std::transform(argv,argv+argc,toks,std::bind(std::string,std::placceholders::_1));
        //TODO: do i need to check for empty strings?
        for(std::vector<std::string>::const_iterator tokit = toks.begin()+1; tokit != toks.end(); ++ tokit) {
            auto&& tok = *tokit;
            if(tok[0] == '-') {
                if(toks.size() > 1 && tok[1] == '-') {
                    parse_long_arg(tokit);
                } else {
                    parse_short_arg(tokit);
                }
            } else {
                m_args.push_back(tok);
            }
        }
#ifndef NDEBUG
        clp_internal::clp_inline_test(*this);
#endif
        if(optT<bool>("help")) {
            std::map<std::string,std::set<std::string>> aliased_values;
            for(auto&& [a,b]: m_aliases) {
                aliased_values[b].insert(a);
            }
            for(auto&& [a,_]: m_opts) {
                aliased_values[a].insert(a);
            }
            for(auto&& [oc,opt]: m_opts) {
                auto&& av = aliased_values[oc];
                std::copy(av.begin(),av.end(), std::ostream_iterator<std::string>(std::cout, ","));
                std::cout << "(" << opt_type(oc)  << "): " << opt.hint << std::endl;
            }
            return false;
        } else {
            return true;
        }
    }

    void CommandLineParser::parse_arg(std::vector<std::string>::const_iterator& it, int prefix) {
        const std::string optname = dealias(it->substr(prefix));
        if(auto argit = m_opts.find(optname); argit != m_opts.end()) {
            std::visit([&](auto& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr(std::is_same_v<T,bool>) {
                        v = true;
                    } else if constexpr(std::is_same_v<T,AntiBool>) {
        if(auto argit = m_opts.find(v.target_name); argit != m_opts.end()) {
        assert(std::holds_alternative<bool>(argit->second));
            argit->second = false;
        }
                    } else {
                        ++it;
                        const std::string& value = *it;
                        try {
                            if constexpr(std::is_same_v<T,int>) {
                                v = std::stoi(value);
                            } else if constexpr(std::is_same_v<T,double>) {
                                v = std::stod(value);
                            } else if constexpr(std::is_same_v<T,std::string>) {
                                v = value;
                            }
                        } catch (const std::invalid_argument& e) {
                            error() << "Invalid argument: " << argit->first << "("<< opt_type(argit->first) <<"): [" << value <<"]";
                        }
                    }
                    },static_cast<OptVar&>(argit->second));
        } else {
            //TODO: Throw error
            throw std::invalid_argument( "Option " + optname + " not recognized." );
        }

    }
    std::string CommandLineParser::opt_type(const std::string& optname) const {
        return std::visit([&](auto&& v) -> std::string {
                using T = std::decay_t<decltype(v)>;
                if constexpr(std::is_same_v<T,std::string>) {
                return "string";
                } else {
                return  types::getTypeName<T>();
                }
                }, opt(optname));
    }

    void CommandLineParser::add_option(const std::string& optname, const OptVar& opt, const std::string& hint) {
        if(hint.empty()) {
            debug() << "Adding option: " << optname;
        } else {
            debug() << "Adding option: " << optname << ": " << hint;
        }
        if(auto it = m_opts.find(optname); it == m_opts.end()) {
            m_opts[optname] = Option(opt,hint);

            std::visit([&](auto&& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T,bool>) {
                    add_option(std::string("no-")+optname,AntiBool{optname},hint.empty()?("No " + hint):"");
                    }
                    },opt);
        }
    }
    void CommandLineParser::add_alias(const std::string& name_raw, const std::string& alias) {
        std::string name = dealias(name_raw);
        m_aliases[alias] = name;
    }

    std::string CommandLineParser::dealias(const std::string& str) const {
        if(auto it = m_aliases.find(str); it != m_aliases.end()) {
            return it->second;
        } else {
            return str;
        }
    }



    auto CommandLineParser::opt(const std::string& optname_raw) const -> const OptVar & {
        std::string optname = dealias(optname_raw);
        if(auto argit = m_opts.find(optname); argit == m_opts.end()) {
            throw std::invalid_argument( "Option " + optname + " not recognized." );
        } else {
            return argit->second;
        }
    }
    std::string CommandLineParser::opt_str(const std::string& optname) const {
        return std::visit([&](auto&& v) -> std::string {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T,bool>) {
                return v?"true":"false";
                } else if constexpr(std::is_same_v<T,AntiBool>) {
                auto bopt = opt(v.target_name);
                assert(std::holds_alternative<bool>(bopt));
                bool val = std::get<bool>(bopt);
                return v.target_name + ((val)?"=true":"=false");
                } else {
                std::stringstream ss;
                ss << v;
                return ss.str();
                }
                }, opt(optname));
    }
}
