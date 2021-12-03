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
// $Maintainer: Douglas McCloskey, Ahmed Khalil $
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/UIUtilities.h>
#include <SmartPeak/core/Utilities.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <chrono>
#include <plog/Log.h>

namespace SmartPeak
{
  void UIUtilities::makeHumanReadable(ImEntry& directory_entry)
  {
    // prettify size [1]
    std::string entry_size_string = directory_entry.entry_contents[1];
    if (directory_entry.entry_contents[2] != "Directory" && !directory_entry.entry_contents[1].empty()
        && entry_size_string.find("Bytes") == std::string::npos
        && entry_size_string.find("KB") == std::string::npos
        && entry_size_string.find("MB") == std::string::npos
        && entry_size_string.find("GB") == std::string::npos
        && entry_size_string.find("TB") == std::string::npos
        )
    {
      double entry_size = std::stod(entry_size_string);
      std::string size_human_readable = "";
      std::stringstream size_human_readable_stream;
      
      if (entry_size >= 0 && entry_size < 1e3)
      {
        size_human_readable_stream << entry_size;
        size_human_readable = size_human_readable_stream.str() + " Bytes";
      }
      if (entry_size >= 1e3 && entry_size < 1e6)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e3);
        auto size_human_readable_str = size_human_readable_stream.str();
        // Utilities::removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " KB";
      }
      else if (entry_size >= 1e6 && entry_size < 1e9)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e6);
        auto size_human_readable_str = size_human_readable_stream.str();
        // Utilities::removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " MB";
      }
      else if (entry_size >= 1e9 && entry_size < 1e12)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e9);
        auto size_human_readable_str = size_human_readable_stream.str();
        // Utilities::removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " GB";
      }
      else if (entry_size >= 1e12 && entry_size < 1e18)
      {
        size_human_readable_stream << std::fixed << std::setprecision(2) << (entry_size / 1e12);
        auto size_human_readable_str = size_human_readable_stream.str();
        // Utilities::removeTrailing(size_human_readable_str, ".00");
        size_human_readable = size_human_readable_str + " TB";
      }
    
      directory_entry.entry_contents[1] = size_human_readable;
    }
    else if (directory_entry.entry_contents[2] == "Directory")
    {
      directory_entry.entry_contents[1] = entry_size_string + " Item(s)";
    }
    
    // prettify extension [2]
    std::string* extension = &directory_entry.entry_contents[2];
    extension->erase(std::remove_if(extension->begin(), extension->end(),
                                   [](unsigned char c){ return std::ispunct(c); }), extension->end());
    
    // prettify date [3]
    std::tm * current_time_tm, entry_date_tm;
    char date_time_buffer[80];
    std::string date_time_buf;
    
    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    current_time_tm = std::localtime(&current_time);
    
    std::istringstream entry_date_ss(directory_entry.entry_contents[3]);
    entry_date_ss >> std::get_time(&entry_date_tm, "%Y-%m-%d %T");
    
    entry_date_tm.tm_isdst = current_time_tm->tm_isdst;    
    time_t entry_date_time = std::mktime(&entry_date_tm);
        
    if (entry_date_tm.tm_mday == current_time_tm->tm_mday
        && entry_date_tm.tm_mon == current_time_tm->tm_mon
        && entry_date_tm.tm_year == current_time_tm->tm_year)
    {
      std::strftime(date_time_buffer, sizeof date_time_buffer, "%R", std::localtime(&entry_date_time));
      directory_entry.entry_contents[3] = "Today at " + std::string(date_time_buffer);
    }
    else
    {
      std::strftime(date_time_buffer, sizeof date_time_buffer, "%c", std::localtime(&entry_date_time));
      directory_entry.entry_contents[3] = date_time_buffer;
    }
  }

}

