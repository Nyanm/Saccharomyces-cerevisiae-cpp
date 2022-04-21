//
// Created by nyanm on 2022/4/21.
//

#include "logger.h"

std::shared_ptr<spdlog::logger> getFileLogger() {
    std::fstream timber("timber.log", std::ios::out);
    timber.close();
    std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("file_logger", "timber.log");
    logger->set_level(spdlog::level::debug);
    logger->set_pattern("[%H:%M:%S.%e] %L: %v");
    logger->info("FileLogger initialization complete.");
    return logger;
}

std::shared_ptr<spdlog::logger> getConsoleLogger(){
    std::shared_ptr<spdlog::logger> logger = spdlog::stdout_color_mt("console_logger");
    logger->set_level(spdlog::level::info);
    logger->set_pattern("[%l] %v");
    logger->info("ConsoleLogger initialization complete.");
    return logger;
}

std::shared_ptr<spdlog::logger> fileLogger = getFileLogger();
std::shared_ptr<spdlog::logger> consoleLogger = getConsoleLogger();

void broadCastInfo(const std::string& msg){
    fileLogger->info(msg);
    consoleLogger->info(msg);
}

void broadCastWarn(const std::string& msg){
    fileLogger->warn(msg);
    consoleLogger->warn(msg);
}

void broadCastError(const std::string& msg){
    fileLogger->error(msg);
    consoleLogger->error(msg);
}