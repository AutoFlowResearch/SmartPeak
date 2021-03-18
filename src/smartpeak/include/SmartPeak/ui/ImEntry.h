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

#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <imgui.h>
#include <imgui_internal.h>

namespace SmartPeak
{
  /**
    @brief Generic entry for tables

  */
  struct ImEntry
  {
    /** @brief Unique entry identifier  */
    std::size_t ID;
    
    /** @brief vector holding entry contents (i.e. row) */
    std::vector<std::string> entry_contents;
    
    /** @brief current sort specs */
    static const ImGuiTableSortSpecs* s_current_sort_specs;
    
    /*
    @brief Checks whether a character is a digit

    @param[in] ch the character to examine
    @param[out] returns true if `ch` is a digit
    */
    static bool is_digit(const char ch);
    
    /*
    @brief Checks whether a string is a number

    @param[in] s string to check
    @param[out] returns true if `s` is a number
    */
    static bool is_number(const std::string& s);
    
    /*
    @brief Pair-wise lexicographical sorting

    @param[in] lhs, rhs pointers of 2 strings to sort
    @param[out] returns a postive int when lhs is greater than rhs,
                zero when equivalent and negative otherwise
    */
    static int lexicographical_sort(const char* lhs, const char* rhs);
    
    /*
    @brief Compares two different `ImEntry`

    @param[in] lhs, rhs void pointers to be used by std::qsort for comparing
    @param[out] returns a postive int when the element pointed to by lhs
                (i.e. ImEntry) goes after the element pointed to by rhs
                (i.e. ImEntry), zero when equivalent and negative otheriwse
    */
    static int CompareWithSortSpecs(const void* lhs, const void* rhs);
  };
}
