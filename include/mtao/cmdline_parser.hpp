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
    bool parse(int argc, char *argv[]);
    const std::vector<std::string> &args() const { return m_args; }
    const std::string &arg(size_t idx) const { return m_args[idx]; }

    struct AntiBool {
        std::string target_name;
    };

    using OptVar = std::variant<bool, int, double, std::string, AntiBool>;
    struct Option : public OptVar {
        Option() = default;
        Option(const Option &) = default;
        Option(Option &&) = default;
        Option &operator=(const Option &) = default;
        Option &operator=(Option &&) = default;
        Option &operator=(OptVar &&v) {
            OptVar::operator=(v);
            return *this;
        }
        Option(const OptVar &ov, const std::string &hint) : OptVar(ov), hint(hint) {}

        std::string hint = "";
    };

    template<typename T>
    const T &optT(const std::string &optname) const;

    //add_option explicitly doesnt overwrite oexisting options
    void add_option(const std::string &optname, const OptVar &default_value = false, const std::string &hint = "");
    void set_option(const std::string &optname, const OptVar &default_value = true);

    const std::map<std::string, Option> &opts() const { return m_opts; }
    const OptVar &opt(const std::string &) const;
    std::string opt_str(const std::string &) const;
    std::string opt_type(const std::string &) const;
    std::string dealias(const std::string &str) const;

    void add_alias(const std::string &name, const std::string &alias);

  private:
    std::vector<std::string> m_args;
    std::map<std::string, Option> m_opts;
    std::map<std::string, std::string> m_aliases;


  private:
    void parse_short_arg(std::vector<std::string>::const_iterator &it) { parse_arg(it, 1); }
    void parse_long_arg(std::vector<std::string>::const_iterator &it) { parse_arg(it, 2); }
    void parse_arg(std::vector<std::string>::const_iterator &it, int prefix);
};
template<typename T>
const T &CommandLineParser::optT(const std::string &optname) const {
    return std::get<T>(opt(optname));
}
}// namespace mtao
