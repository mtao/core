#ifndef MTAO_LOGGING_LOGGER_HPP
#define MTAO_LOGGING_LOGGER_HPP
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

namespace mtao {
namespace logging {

    enum class Level : char { Off = 0,
                              Fatal = 1,
                              Error = 2,
                              Warn = 3,
                              Info = 4,
                              Debug = 5,
                              Trace = 6,
                              All = 7 };

    extern const std::string level_strings[int(Level::All) + 1];
    auto level_from_string(const std::string &str) -> Level;

    //WARNING! assumes default logger will always exist
    class Logger {
      public:
        struct Output {
            std::ofstream out;
            Level level;
        };
        explicit Logger(const std::string &alias, Level l);
        Logger(Logger &&l) = default;
        auto operator=(Logger &&l) -> Logger & = default;

        static auto log_type_string(Level l, bool color = false) -> const std::string &;

        void add_file(const std::string &filename, Level l, bool continueFile);

        [[nodiscard]] static auto decorator(Level l, bool humanReadable = false) -> std::string;
        template<typename... Args>
        auto process_line(Args &&... args) -> std::string {
            std::stringstream ss;
            (ss << ... << args) << std::endl;
            return ss.str();
        }
        template<typename... Args>
        void write(Level l, Args &&... args) {
            write_line(l, process_line(std::forward<Args>(args)...));
        }
        template<typename... Args>
        void write_output(Output &output, Level l, Args &&... args) {
            write_line(output, l, process_line(std::forward<Args>(args)...));
        }
        template<typename... Args>
        void write_cerr(Level l, Args &&... args) {
            write_line_cerr(l, process_line(std::forward<Args>(args)...));
        }

        void write_line(Level l, const std::string &str);
        void write_line(Output &output, Level l, const std::string &str);
        //no decorator
        static void write_line_nodec(Output &output, Level l, const std::string &str);
        void write_line_cerr(Level l, const std::string &str);
        void write_line_cerr_nodec(Level l, const std::string &str);

        [[nodiscard]] static auto current_time() -> size_t;

        class Instance {
          public:
            Instance() {}
            Instance(const Instance &) = delete;
            Instance(Instance &&) = default;
            explicit Instance(Logger *log, Level l) : logger(log), level(l) {}
            ~Instance();

            template<typename T>
            auto operator<<(const T &v) -> Instance & {
                m_ss << v;
                return *this;
            }

          private:
            std::stringstream m_ss;
            Logger *logger = nullptr;
            Level level = Level::Info;
        };

        auto instance(Level l) { return Instance(this, l); }
        void set_level(Level l) { m_level = l; }
        [[nodiscard]] auto alias() const -> const std::string & { return m_alias; }

      private:
        std::string m_alias;
        std::map<std::string, Output> m_outputs;
        Level m_level;
    };

    class LoggerContext {
      public:
        LoggerContext() {}
        explicit LoggerContext(Logger *log, Level l) : m_logger(log), m_level(l) {}

        template<typename T>
        auto operator<<(const T &v) -> LoggerContext & {
            if (m_logger) {
                m_logger->write(m_level, v);
            }
            return *this;
        }
        template<typename... Args>
        void write(Args &&... args) {
            if (m_logger) {
                m_logger->write(m_level, std::forward<Args>(args)...);
            }
        }


        auto instance() { return m_logger->instance(level()); }
        [[nodiscard]] auto level() const -> Level { return m_level; }
        [[nodiscard]] auto alias() const -> const std::string & { return m_logger->alias(); }
        [[nodiscard]] auto info() const -> std::pair<std::string, Level> { return { alias(), level() }; }

      private:
        Logger *m_logger = nullptr;
        Level m_level = Level::Info;
    };


    auto get_logger(const std::string &alias, Level l) -> LoggerContext;
    auto get_logger(const std::pair<std::string, Level> &pr) -> LoggerContext;
    auto make_file_logger(const std::string &alias, const std::string &filename, Level l, bool continueFile) -> Logger &;
    auto make_logger(const std::string &alias, Level l) -> Logger &;
    extern std::map<std::string, mtao::logging::Logger> active_loggers;
    extern std::string default_log_alias;

    auto fatal() -> Logger::Instance;
    auto error() -> Logger::Instance;
    auto warn() -> Logger::Instance;
    auto info() -> Logger::Instance;
    auto debug() -> Logger::Instance;
    auto trace() -> Logger::Instance;
    auto log(Level l) -> Logger::Instance;


}// namespace logging
}// namespace mtao


#endif// MTAO_LOGGING_LOGGER_HPP
