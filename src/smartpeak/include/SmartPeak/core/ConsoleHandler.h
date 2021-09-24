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
// $Maintainer: Krzysztof Abram $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------
#pragma once
#include <iostream>
#include <chrono>
#include <algorithm>
#include <memory>

#include <plog/Log.h>
#include <plog/Appenders/ConsoleAppender.h>
#include <plog/Appenders/ColorConsoleAppender.h>

#include <SmartPeak/ui/GuiAppender.h>


namespace SmartPeak {

/**
 * ConsoleHandler class is a wrapper around plog initialization, 
 * i.e. encapsulates log configuration and initialization.
*/
class ConsoleHandler {
public:
    /**
     * Returns a reference to singleton instance of the ConsoleHandler class.
    */
    static ConsoleHandler& get_instance()
    {
        static ConsoleHandler instance;
        return instance;
    }

    ConsoleHandler(const ConsoleHandler&) = delete;
    ConsoleHandler& operator= (const ConsoleHandler&) = delete;

    /**
     * Initializes plog instance, configures logging to console and file, 
     * generates an unique file name and prints out first wecome message.
     * 
     * @param[in] welcome_msg The very first message in logs (info level)
    */
    virtual void initialize(const std::string& welcome_msg);

public:
    /**
     * Sets severity level, which is passed to plog instance. 
     * Has effect only before calling initialize() method.
     * 
     * @param[in] severity plog severity level
    */
    virtual void set_severity(const plog::Severity& severity) { m_severity = severity; }

    /**
     * Gets severity level used for plog initialization.
     * 
     * @return plog severity level
    */
    virtual plog::Severity get_severity() const { return m_severity; }

    /**
     * Sets a flag for coloring an output of logs (used only for console).
     * Has effect only before calling initialize() method.
     * 
     * @param[in] flag true for colored output, false otherwise
    */
    virtual void use_colors(bool flag = true) { m_enable_colors = flag; }

    /**
     * Return coloring flag, used for plog initialization.
     * 
     * @return true for colored output, false otherwise
    */
    virtual bool colored_console() const { return m_enable_colors; }

    /**
     * Overrides default location for logs output.
     * Has effect only before calling initialize() method.
     * 
     * @see SmartPeak::Utilities::getLogFilepath()
     * @see initialize()
     * @param[in] dirpath path to directory
    */
    virtual void set_log_directory(const std::string& dirpath) { m_log_dirpath = dirpath; }

    /**
     * Return log directory path.
     * @return path as string
    */
    virtual std::string get_log_directory() const { return m_log_dirpath; }

    /**
     * Returns name of log file.
    */
    virtual std::string get_log_filename() const { return m_log_filename; }

    /**
     * Returns a full path to log file.
     * @return path as string
     */
    virtual std::string get_log_filepath() const { return m_log_filepath; }

    /**
     * Inform whether directory was created for log file.
     * @return true if new directory created, false otherwise
    */
    virtual bool log_directory_created() const { return m_logdir_created; }

    /**
     * Returns true if plog instance was initialized.
     */
    virtual bool is_initialized() const { return m_is_initialized; }

protected:
    /**
     * Generates an unique log filename.
     * @return unique name of log file
    */
    virtual std::string _get_log_filename() const;

    /**
     * Inititlizes log file location, either the default location, defined in 
     * SmartPeak::Utilities::getLogFilepath(), or location given by set_log_directory().
     * 
     * @see SmartPeak::Utilities::getLogFilepath()
     * @see set_log_directory()
     * @return true if initialization correct, false otherwise
    */
    virtual bool _initialize_log_location(const std::string& filename, std::string& error_msg);

    /**
     * Inititlizes plog instance, configures console and file appender.
     * Colored console appender is used if use_colors() set the flag to true.
     * If init_file_appender is set to false, it will only use console appender.
     * 
     * @see use_colors()
     * @param init_file_appender set to true if location is correct, false otherwise
    */
    virtual void _initialize_log_console(bool init_file_appender = true);

private:
    /* ctor/dtor */
    ConsoleHandler() 
        : m_severity {plog::info}, m_enable_colors {false},
            m_is_initialized {false}
    {}

    virtual ~ConsoleHandler() = default;

private:
    std::string m_log_filename;
    std::string m_log_filepath;
    std::string m_log_dirpath;
    bool m_logdir_created;
    plog::Severity m_severity;
    bool m_enable_colors;
    bool m_is_initialized;

    std::shared_ptr<plog::RollingFileAppender<plog::CsvFormatter>> m_file_appender;
    std::shared_ptr<plog::ConsoleAppender<plog::TxtFormatter>> m_console_appender;
  
public:
    GuiAppender gui_appender_;
};

} /* namespace SmartPeak */
