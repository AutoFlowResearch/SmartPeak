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
#include <algorithm>
#include <vector>

#include <SmartPeak/cli/Parser.h>


namespace SmartPeak {
namespace cli {

/**
 * Stores and handles program parameters passed from command line.
 */
struct ApplicationSettings {

    /**
     * Initializes ApplicationSettings object.
     * @param parser instance of cli::Parser
     */
    explicit ApplicationSettings(Parser& parser) : m_parser {parser} {}
    virtual ~ApplicationSettings() = default;

    /**
     * Defines required and optional program options.
     * The help option which displays a summary about  available by default.
     */
    virtual void define_options();

    /**
     * Reads program options from parser.
     */
    virtual void load_options();

    /**
     * Transfrom raw options, e.g. converts strings to upper case.
     */
    virtual void process_options();

    /**
     * Check values against criteria.
     */
    virtual void validate() const;

    /**
     * Return a string containing a full description of program parameters.
     * @return Full description of program options
     */
    virtual std::string usage() const { return m_parser.usage(); }

    virtual Parser& get_parser() { return m_parser; }
    virtual const Parser& get_parser() const { return m_parser; }

    /**
     * Check if an option is included in a list.
     * 
     * @param list list of strings
     * @param option option name
     * @param log_msg log message on success
     */
    static bool contains_option(
        const std::vector<std::string>& list, 
        const std::string& option, 
        std::string log_msg="");

public:
    /* options */
    std::string load_session;
    std::string load_session_from_dir;
    std::vector<std::string> report;
    std::vector<std::string> report_sample_types;
    std::vector<std::string> report_metadata;
    std::vector<std::string> workflow;
    std::vector<std::string> integrity;
    bool allow_inconsistent;
    bool verbose;
    bool disable_colors;
    bool disable_progressbar;
    std::string log_dir; 
    std::string out_dir;

public:
    void validate_report() const;
    void validate_report_sample_types() const;
    void validate_report_metadata() const;
    void validate_workflow() const;
    void validate_integrity() const;

private:
    Parser& m_parser;
};

} /* namespace cli */
} /* namespace SmartPeak */