//
// Created by nyanm on 2022/4/21.
//

#ifndef SDVX_CPP_LOGGER_H
#define SDVX_CPP_LOGGER_H

#include "fstream"
#include "spdlog/spdlog.h"
#include <spdlog/sinks/basic_file_sink.h>
#include "spdlog/sinks/stdout_color_sinks.h"

extern std::shared_ptr<spdlog::logger> fileLogger;
extern std::shared_ptr<spdlog::logger> consoleLogger;

void broadCastInfo(const std::string& msg);
void broadCastWarn(const std::string& msg);
void broadCastError(const std::string& msg);

#endif //SDVX_CPP_LOGGER_H
