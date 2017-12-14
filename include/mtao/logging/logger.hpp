#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <fstream>
#include <map>
#include <sstream>
#include <iostream>

namespace mtao {
    namespace logging {

        enum class Level: char { Off=0, Fatal=1, Error=2, Warn=3, Info=4, Debug=5, Trace=6, All=7 };

        extern const std::string level_strings[int(Level::All)+1];
        Level level_from_string(const std::string& str);

        //WARNING! assumes default logger will always exist
        class Logger {
            public:
                struct Output {
                    std::ofstream out;
                    Level level;
                };
                Logger(const std::string& alias="default", Level l = Level::Warn);
                Logger(Logger&& l) = default;
                Logger& operator=(Logger&& l) = default;

                static const std::string& log_type_string(Level l, bool color=false);

                void add_file(const std::string& filename, Level l, bool continueFile=false);

                std::string decorator(Level l, bool humanReadable=false) const;
                template <typename... Args>
                    std::string process_line(Args&&... args) {
                        std::stringstream ss;
                        ( ss << ... << args ) << std::endl;
                        return ss.str();
                    }
                template <typename... Args>
                    void write(Level l,  Args&&... args) {
                        write_line(l,process_line(std::forward<Args>(args)...));
                    }
                template <typename... Args>
                    void write_output(Output& output,Level l,  Args&&... args) {
                        write_line(output, l,process_line(std::forward<Args>(args)...));
                    }
                template <typename... Args>
                    void write_cout(Level l, Args&&... args) {
                        write_line_cout(l,process_line(std::forward<Args>(args)...));
                    }

                void write_line(Level l,const std::string& str);
                void write_line(Output& output, Level l,const std::string& str);
                void write_line_nodec(Output& output, Level l,const std::string& str);
                void write_line_cout(Level l,const std::string& str);
                void write_line_cout_nodec(Level l,const std::string& str);

                size_t current_time() const;

                class Instance {
                    public:
                        Instance() {}
                        Instance(const Instance&) = delete;
                        Instance(Instance&&) = default;
                        Instance(Logger* log, Level l=Level::Warn): logger(log), level(l) {}
                        ~Instance();

                        template <typename T>
                            Instance& operator<<(const T& v) {
                                m_ss << v;
                                return *this;
                            }

                    private:
                        std::stringstream m_ss;
                        Logger* logger = nullptr;
                        Level level = Level::Info;
                };

                auto instance(Level l) { return Instance(this, l); }
                void set_level(Level l) { m_level = l; }
            private:
                std::string m_alias;
                std::map<std::string,Output> m_outputs;
                Level m_level;


        };

        class LoggerContext {
            public:
                LoggerContext() {}
                LoggerContext(Logger* log, Level l=Level::Warn): logger(log), level(l) {}

                template <typename T>
                    LoggerContext& operator<<(const T& v) {
                        if(logger) {
                            logger->write(level,v);
                        }
                        return *this;
                    }
                template <typename... Args>
                    void write(Args&&... args) {
                        if(logger) {
                            logger->write(level,std::forward<Args>(args)...);
                        }
                    }


                auto instance() { return logger->instance(level); }
            private:
                Logger* logger = nullptr;
                Level level = Level::Info;
        };






        LoggerContext get_logger(const std::string& alias, Level l=Level::Info);
        Logger& make_logger(const std::string& alias="default", const std::string& filename="default.log", Level l=Level::All, bool continueFile=false);
        Logger& make_logger(const std::string& alias="default", Level l = Level::All);
        extern std::map<std::string,mtao::logging::Logger> active_loggers;
        extern std::string default_log_alias;

        Logger::Instance fatal();
        Logger::Instance error();
        Logger::Instance warn();
        Logger::Instance info();
        Logger::Instance debug();
        Logger::Instance trace();
        Logger::Instance log(Level l);


    }
}


#endif//LOGGER_HPP
