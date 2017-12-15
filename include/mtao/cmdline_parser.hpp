#pragma once
#include <vector>
#include <map>
#include <sstream>
#include <cassert>
#include <variant>



namespace mtao {

    class CommandLineParser {
        public:
            CommandLineParser();
            void parse(int argc, char * argv[]);
            const std::vector<std::string>& args() const {return m_args;}
            const std::string& args(size_t idx) const {return m_args[idx];}


            using Option = std::variant<bool,int,double,std::string>;

            /*
            class Option {
                public:
                    enum class ValueType {Flag, Integer, Float, String};
                    Option(ValueType t = ValueType::Integer): m_type(t) {}
                    const std::string& value() const {return m_value;}
                    template <typename T>
                        struct type_to_vt {};
                    template <typename T>
                        T valueT() const {
                            assert(type_to_vt<T>::value == m_type);
                            std::stringstream ss(m_value);
                            T v;
                            ss >> v;
                            return v;
                        }
                    bool active() const {return m_active;}
                    void activate() {m_active = true;}
                    void set_value(const std::string& str) {
                        m_active = true;
                        m_value = str;
                    }
                    ValueType type() const {return m_type;}
                private:
                    bool m_active = false;
                    ValueType m_type = ValueType::Integer;
                    std::string m_value = "";
            };
            */
            template <typename T>
                const T& optT(const std::string& optname) const;

            void add_option(const std::string& optname, const Option& default_value = false);
            const std::map<std::string,Option>& opts() const {return m_opts;} 
            const Option& opt(const std::string&) const;
            std::string opt_str(const std::string&) const;
            std::string opt_type(const std::string&) const;
            std::string dealias(const std::string& str) const;

            void add_alias(const std::string& name, const std::string& alias);

        private:
            std::vector<std::string> m_args;
            std::map<std::string, Option> m_opts;
            std::map<std::string,std::string> m_aliases;


        private:
            void parse_short_arg(std::vector<std::string>::const_iterator& it) {parse_arg(it,1);}
            void parse_long_arg(std::vector<std::string>::const_iterator& it) {parse_arg(it,2);}
            void parse_arg(std::vector<std::string>::const_iterator& it, int prefix) ;
    };
    template <typename T>
        const T& CommandLineParser::optT(const std::string& optname) const {
            return std::get<T>(opt(optname));
        }
}
