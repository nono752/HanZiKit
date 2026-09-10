#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include "errorTypes.hpp"

class ErrorHandler
{
    private:
        std::string buffer;
        Errors& errors;

    private:
        void lexerPhaseHandler(const Error& err);
        void parserPhaseHandler(const Error& err);
        void enricherPhaseHandler(const Error& err);
        void generatorPhaseHandler(const Error& err);
        void exporterPhaseHandler(const Error& err);

    public:
        ErrorHandler(Errors& err) : errors(err) {}
        void flushErrorInBuffer();
        bool printErrorInLogFile(const std::string& file = "log.txt") const;
        void printErrorInTerminal() const;
};



#endif