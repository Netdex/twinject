#pragma once
#include <memory>

#include <spdlog/spdlog.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/msvc_sink.h>

class spdlog_msvc
{
	std::shared_ptr<spdlog::sinks::msvc_sink_mt> msvc_sink;
	std::shared_ptr<spdlog::logger> logger;

public:
	spdlog_msvc()
	{
		msvc_sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
		logger = std::make_shared<spdlog::logger>("msvc_logger", msvc_sink);
		spdlog::set_default_logger(logger);
	}

	std::shared_ptr<spdlog::logger> get_logger() const
	{
		return logger;
	}
};
