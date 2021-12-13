// --------------------------------------------------------------------------
//   SmartPeak -- Fast and Accurate CE-, GC- and LC-MS(/MS) Data Processing
// --------------------------------------------------------------------------
// Copyright The SmartPeak Team -- Novo Nordisk Foundation
// Center for Biosustainability, Technical University of Denmark 2018-2021.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL ANY OF THE AUTHORS OR THE CONTRIBUTING
// INSTITUTIONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
// ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// --------------------------------------------------------------------------
// $Maintainer: Krzysztof Abram, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#include <SmartPeak/core/ConsoleHandler.h>

#include <SmartPeak/core/Utilities.h>
#include <fstream>
#include <filesystem>


namespace SmartPeak {

  void ConsoleHandler::initialize(const std::string& welcome_msg)
  {
    if (!is_initialized())
    {
      auto error_msg = std::string{};
      m_log_filename = _get_log_filename();
      auto log_location_correct = _initialize_log_location(m_log_filename, error_msg);
      _initialize_log_console(log_location_correct);

      // Log SmartPeak launch initiated:
      LOG_INFO << welcome_msg;
      if (error_msg.empty())
      {
        if (m_logdir_created) {
          LOG_DEBUG << "Log directory created: " << m_log_filepath;
          LOG_INFO << "Log file at: " << m_log_filepath;
        }
      }
      else
      {
        // In this case it will only use console appender
        LOG_WARNING << error_msg;
      }
      m_is_initialized = true;
    }
  }

  std::string ConsoleHandler::_get_log_filename() const
  {
    const std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char filename[128];
    strftime(filename, 128, "smartpeak_log_%Y-%m-%d_%H-%M-%S.csv", std::localtime(&t));
    return std::string{filename};
  }

  bool ConsoleHandler::_initialize_log_location(const std::string& filename, std::string& error_msg)
  {
    namespace fs = std::filesystem;
    m_logdir_created = false;
    try
    {
      auto fpath = fs::path();
      if (m_log_dirpath.empty())
      {
        std::tie(fpath, m_logdir_created) = Utilities::getLogFilepath(filename);
        m_log_filepath = fpath.string();
        m_log_dirpath = fpath.parent_path().string();
      }
      else
      {
        auto path = (fs::path(m_log_dirpath) / filename).string();
        std::ofstream file(path);
        if (!file)
        {
          std::tie(fpath, m_logdir_created) = Utilities::getLogFilepath(filename);
          m_log_filepath = fpath.string();
          m_log_dirpath = fpath.parent_path().string();
          error_msg = static_cast<std::ostringstream&&>(
            std::ostringstream()
              << "Unable to create log file '" << path
              << "', directory does not exist or has insufficient access permissions. "
              << "Log to the default location instead: '" << m_log_filepath << "'").str();
        }
        else
        {
          file.close();
          m_log_filepath = path;
        }
      }
    }
    catch (const std::runtime_error& re)
    {
      error_msg = re.what();
      return false;
    }
    return true;
  }

  void ConsoleHandler::_initialize_log_console(bool init_file_appender)
  {
    if (nullptr == m_file_appender && init_file_appender)
    {
      // Add .csv appender: 32 MiB per file, max. 100 log files
      m_file_appender = std::make_shared<
          plog::RollingFileAppender<plog::CsvFormatter>>(
              m_log_filepath.c_str(), 1024 * 1024 * 32, 100);
    }
    if (nullptr == m_console_appender)
    {
      if (m_enable_colors)
      {
        m_console_appender = std::make_shared<
            plog::ColorConsoleAppender<plog::TxtFormatter>>();
      }
      else
      {
        m_console_appender = std::make_shared<
            plog::ConsoleAppender<plog::TxtFormatter>>();
      }
    }
    if (nullptr == m_file_appender)
    {
      plog::init(plog::debug, m_console_appender.get()).addAppender(&server_appender_);
    }
    else
    {
      plog::init(plog::debug, m_file_appender.get()).addAppender(m_console_appender.get()).addAppender(&server_appender_);
    }
  }

} /* namespace SmartPeak */
