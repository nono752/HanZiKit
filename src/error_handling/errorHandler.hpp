#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <string>
#include "errorTypes.hpp"

class ErrorHandler
{
    private:
        std::string buffer;
        Errors& errors;
        size_t fatalErrors = 0;
        size_t warningErrors = 0;
        std::string prefix = "";

    private:
        void lexerPhaseHandler(const Error& err);
        void parserPhaseHandler(const Error& err);
        void enricherPhaseHandler(const Error& err);
        void generatorPhaseHandler(const Error& err);
        void exporterPhaseHandler(const Error& err);
    
    public:
        ErrorHandler(Errors& err) : errors(err) {}
        void makeErrorBuffer();
        bool printErrorBufferInLogFile(const std::string& file = "log.txt") const;
        void printErrorBufferInTerminal() const;
        void clear() { fatalErrors = warningErrors = 0; buffer.clear(); errors.clear(); prefix.clear(); }
        size_t getFatalErrors() const { return fatalErrors; }
        size_t getWarningErrors() const { return warningErrors; }
};

#endif