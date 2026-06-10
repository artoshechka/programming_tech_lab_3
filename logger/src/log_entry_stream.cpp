/// @file
/// @brief Определение внутреннего потокового helper для логирования
/// @author Artemenko Anton

#include <log_entry_stream.hpp>

using logger::detail::LogEntryStream;

LogEntryStream::LogEntryStream(const std::shared_ptr<logger::ILogger>& logger, logger::LogLevel level, const char* file,
                               int line, const char* function)
    : logger_(logger), level_(level), file_(file), line_(line), function_(function)
{
}

LogEntryStream::~LogEntryStream()
{
    if (logger_ != nullptr)
    {
        logger_->Log(level_, stream_.str(), file_, line_, function_);
    }
}
