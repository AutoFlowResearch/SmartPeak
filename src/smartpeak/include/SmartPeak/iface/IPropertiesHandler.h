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
// $Maintainer: Douglas McCloskey $
// $Authors: Douglas McCloskey, Bertrand Boudaud $
// --------------------------------------------------------------------------

#pragma once

#include <SmartPeak/core/CastValue.h>

#include <map>
#include <optional>

namespace SmartPeak
{
  /**
  * Interface for classes that exposes properties in order to be serialized/deserialized from a session.
  */
  struct IPropertiesHandler
  {
    /**
      @brief Returns the name of the properties container
      @return name of the properties container

      The name must be unique across all the instances of the session.
    */
    virtual std::string getPropertiesHandlerName() const = 0;

    /**
      @brief Returns the list of properties, name of the property, and its type.
      @return a map of name/type
    */
    virtual std::map<std::string, CastValue::Type> getPropertiesSchema() const = 0;

    /**
      @brief In case the IPropertiesHandler is a table, returns the number of rows.
      @return number of rows
    */
    virtual size_t getNbRows() const { return 1; };

    /**
      @brief Returns the property value based on the name. 
      @return the property value, as CastValue, std::nullopt if the property does not exists.
    */
    virtual std::optional<CastValue> getProperty(const std::string& property, const size_t row = 0) const = 0;

    /**
      @brief Sets the property value based on the name.
    */
    virtual void setProperty(const std::string& property, const CastValue& value, const size_t row) = 0;
  };
}
