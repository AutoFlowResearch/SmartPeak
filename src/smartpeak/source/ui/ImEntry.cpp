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
// $Maintainer: Ahmed Khalil $
// $Authors: Ahmed Khalil $
// --------------------------------------------------------------------------

#include <SmartPeak/ui/ImEntry.h>

namespace SmartPeak
{
  bool ImEntry::is_digit(const char ch)
  {
    return ch >= '0' && ch <= '9';
  }
    
  bool ImEntry::is_number(const std::string& s)
  {
    return !s.empty() && std::find_if(s.begin(), s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
  }
    
  int ImEntry::lexicographical_sort(const char* lhs, const char* rhs)
  {
    std::string lhs_str(lhs), rhs_str(rhs);
    std::string lhs_str_lowercased(lhs_str.length(),' ');
    std::string rhs_str_lowercased(rhs_str.length(),' ');
    std::transform(lhs_str.begin(), lhs_str.end(), lhs_str_lowercased.begin(), tolower);
    std::transform(rhs_str.begin(), rhs_str.end(), rhs_str_lowercased.begin(), tolower);
    lhs = lhs_str_lowercased.c_str();
    rhs = rhs_str_lowercased.c_str();
      
    enum SearchMode { STRING, NUMBER } search_mode = STRING;
    while (*lhs && *rhs)
    {
      if (search_mode == STRING)
      {
        char lhs_char, rhs_char;
        while ((lhs_char = *lhs) && (rhs_char = *rhs))
        {
          const bool lhs_digit = is_digit(lhs_char), rhs_digit = is_digit(rhs_char);
          if (lhs_digit && rhs_digit)
          {
            search_mode = NUMBER;
            break;
          }
  
          if (lhs_digit)
            return -1;
  
          if (rhs_digit)
            return +1;
            
          const int char_diff = lhs_char - rhs_char;
          if (char_diff != 0)
            return char_diff;
            
          ++lhs;
          ++rhs;
        }
      }
      else if (search_mode == NUMBER)
      {
        unsigned long lhs_int = 0;
        while (*lhs && is_digit(*lhs))
        {
          lhs_int = lhs_int*10 + *lhs-'0';
          ++lhs;
        }
        unsigned long rhs_int = 0;
        while (*rhs && is_digit(*rhs))
        {
          rhs_int = rhs_int*10 + *rhs-'0';
          ++rhs;
        }
      
        const long int_diff = lhs_int - rhs_int;
        if (int_diff != 0)
          return int_diff;

        search_mode = STRING;
      }
    }
  }

  int IMGUI_CDECL ImEntry::CompareWithSortSpecs(const void* lhs, const void* rhs)
  {
    const ImEntry* a = (const ImEntry*)lhs;
    const ImEntry* b = (const ImEntry*)rhs;
    for (int n = 0; n < s_current_sort_specs->SpecsCount; n++)
    {
      const ImGuiTableColumnSortSpecs* sort_spec = &s_current_sort_specs->Specs[n];
      int delta = 0;
      size_t column_id = sort_spec->ColumnIndex;
      if (column_id == 0 && !a->entry_contents.empty() && !b->entry_contents.empty())
      {
        if (a->entry_contents[0].c_str() != nullptr && b->entry_contents[0].c_str() != nullptr)
        {
          if ( is_number(a->entry_contents[0]) && is_number(b->entry_contents[0]))
          {
            delta = (std::stol(a->entry_contents[0]) - std::stol(b->entry_contents[0]));
          }
          else
          {
            delta = lexicographical_sort(a->entry_contents[0].c_str(), b->entry_contents[0].c_str());
          }
        }
      }
      else if (column_id > 0 && !a->entry_contents.empty() && !b->entry_contents.empty())
      {
        delta = lexicographical_sort(a->entry_contents[column_id].c_str(), b->entry_contents[column_id].c_str());
      }

      if (delta > 0)
        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? +1 : -1;
      if (delta < 0)
        return (sort_spec->SortDirection == ImGuiSortDirection_Ascending) ? -1 : +1;
    }
      
    if (a->entry_contents[1].c_str() != nullptr && b->entry_contents[1].c_str() != nullptr)
      return (strcmp(a->entry_contents[1].c_str(), b->entry_contents[1].c_str()));
    else
      return (a->entry_contents[0].c_str() - a->entry_contents[0].c_str());
  }
}
