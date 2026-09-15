#include "errorHandler.hpp"
#include <fstream>
#include <iostream>
#include <format>

void ErrorHandler::flushErrorInBuffer()
{
    for (Error& err : errors)
    {
        switch (err.phase)
        {
            case ErrorPhase::LEXER: 
                lexerPhaseHandler(err); 
                break;
            case ErrorPhase::PARSER: 
                parserPhaseHandler(err); 
                break;
            case ErrorPhase::ENRICHER: 
                enricherPhaseHandler(err); 
                break;
            case ErrorPhase::JSON_GENERATION: 
            case ErrorPhase::LATEX_GENERATION: 
                generatorPhaseHandler(err); 
                break;
            case ErrorPhase::HTML_EXPORTER: 
                exporterPhaseHandler(err);
                break;
            default: 
                break;
        }
    }

    errors.clear();
}

void ErrorHandler::lexerPhaseHandler(const Error& err)
{
    std::string message;

    switch (err.code) 
    {
        case ErrorCode::UNKNOWN_CHAR_ENCOUNTERED:
            message = std::format("unknown char '{}' encountered.", err.details);
            break;
        case ErrorCode::SOURCE_FILE_NOPEN:
            message = std::format("couldn't open source file '{}'.", err.details);
            break;
        case ErrorCode::STRING_BUFFER_WRITING_FAILED:
            message = "string buffer is empty writing failed";
            break;
        default:
            message = "DEBUG: missing errorCode case!";
            break;
    }

    buffer += std::format("[LEXER ERROR] at ({}, {}):\n{}\n", err.line, err.col, message);
}
void ErrorHandler::parserPhaseHandler(const Error& err)
{
    std::string message;

    switch (err.code) 
    {
        case ErrorCode::MULTIPLE_MAIN_PAGES:
            message = "More than one main page defined. Only one '#' is allowed.";
            break;
        case ErrorCode::MISSING_TITLE:
            message = "Missing section or module title after marker.";
            break;
        case ErrorCode::UNEXPECTED_SYMBOL:
            message = std::format("Unexpected symbol encountered: '{}'.", err.details);
            break;
        case ErrorCode::VOCAB_OUTSIDE_MODULE:
            message = std::format("Vocabulary entry '{}' defined outside of a module (##).", err.details);
            break;
        case ErrorCode::MISSING_SEPARATOR:
            message = std::format("Missing separator '|' for the entry '{}'.", err.details);
            break;
        case ErrorCode::MISSING_TRADUCTION:
            message = std::format("Missing translation for the entry '{}'.", err.details);
            break;
        case ErrorCode::NO_INSTRUCTION:
            message = "Empty instruction or missing data.";
            break;
        case ErrorCode::UNKNOWN_PARSER_ERROR:
        default:
            message = "DEBUG: missing errorCode case!";
            break;
    }

    buffer += std::format("[PARSER ERROR] at ({}, {}):\n{}\n", err.line, err.col, message);
}
void ErrorHandler::enricherPhaseHandler(const Error& err)
{
    std::string message;

    switch (err.code) 
    {
        case ErrorCode::UNKWNOWN_HANZI_ENTRY:
            message = std::format("unknown hanzi to cedict '{}' encountered.", err.details);
            break;
        case ErrorCode::INVALID_PINYIN_TONE:
            message = std::format("invalid pinyin tone '{}' encountered.", err.details);
            break;
        default:
            message = "DEBUG: missing errorCode case!";
            break;
    }

    buffer += std::format("[ENRICHER ERROR] at ({}, {}):\n{}\n", err.line, err.col, message);
}
void ErrorHandler::generatorPhaseHandler(const Error& err)
{
    std::string message;
    std::string generatorKind = err.phase == ErrorPhase::JSON_GENERATION ? "JSON" : "LATEX";

    switch (err.code) 
    {
        case ErrorCode::JSON_IS_EMPTY:
            message = "json file is empty.";
            break;
        default:
            message = "DEBUG: missing errorCode case!";
            break;
    }

    buffer += std::format("[{}_GENERATOR ERROR] at ({}, {}):\n{}\n", generatorKind, err.line, err.col, message);
}
void ErrorHandler::exporterPhaseHandler(const Error& err)
{
    std::string message;

    switch (err.code) 
    {
        case ErrorCode::NO_JSON_TAG_IN_HTML:
            message = "Html file have no json tag.";
            break;
        case ErrorCode::MULTIPLE_JSON_TAG_IN_HTML:
            message = "Html file have multiple json tag.";
            break;
        case ErrorCode::HTML_OUTPUT_NOPEN:
            message = std::format("output file '{}' not opened", err.details);
            break;
        case ErrorCode::HTML_BUFFER_WRITING_FAILED:
            message = "failed to write in html buffer, is empty.";
            break;
        default:
            message = "DEBUG: missing errorCode case!";
            break;
    }

    buffer += std::format("[EXPORTER ERROR] at ({}, {}):\n{}\n", err.line, err.col, message);
}

bool ErrorHandler::printErrorInLogFile(const std::string& file) const
{
    std::ofstream out("log.txt");
    if (!out) return false;

    out << buffer << std::flush;

    if (out.fail())
    {
        out.close();
        return false;
    }

    out.close();
    return true;
}
void ErrorHandler::printErrorInTerminal() const
{
    std::cerr << buffer << std::flush;
}