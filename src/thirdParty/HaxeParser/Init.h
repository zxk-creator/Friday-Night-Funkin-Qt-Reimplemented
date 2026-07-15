//
// Created by kkplay on 7/14/26.
//

#pragma once
#include "ast/Interpreter.h"
#include "ast/Parser.h"
#include "lib/ScriptLibs.h"
#include "token/Scanner.h"


namespace HxParser
{
    void initInterpreter(QString& source, Interpreter* interpreter)
    {
        Scanner scanner(source);
        auto tokens = scanner.scanTokens();

        Parser parser(tokens);
        auto statements = parser.parse();

        interpreter->registerNativeFunc("trace", [](const std::vector<Dynamic>& args) -> Dynamic {
            if (!args.empty()) {
                qDebug().noquote() << args[0].toString();
            } else {
                qDebug().noquote() << "";
            }
            return Dynamic();
        });

        interpreter->registerNativeFunc("now",[](const std::vector<Dynamic>& args) -> Dynamic
        {
           return ScriptLibs::now();
        });

        interpreter->interpret(statements);
    }
}
