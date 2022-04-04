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
// $Authors: Douglas McCloskey, Pasquale Domenico Colaianni $
// --------------------------------------------------------------------------

#include <SmartPeak/core/ApplicationProcessors/StoreFilenames.h>

namespace SmartPeak
{

  bool StoreFilenames::doProcess()
  {
    auto db_context = application_handler_.filenames_.getSessionDB().beginWrite(
      "filenames",
      "file_id", "TEXT",
      "filename_pattern", "TEXT",
      "embedded", "INT"
    );
    if (!db_context)
    {
      return false;
    }
    for (const auto& file_id : application_handler_.filenames_.getFileIds())
    {
      int embedded = (application_handler_.filenames_.isEmbedded(file_id) ? 1 : 0);
      application_handler_.filenames_.getSessionDB().write(
        *db_context,
        file_id,
        application_handler_.filenames_.getNamePattern(file_id).generic_string(),
        embedded
      );
    }
    application_handler_.filenames_.getSessionDB().endWrite(*db_context);

    db_context = application_handler_.filenames_.getSessionDB().beginWrite(
      "filenames_tags",
      "tag", "TEXT",
      "value", "TEXT"
    );
    if (!db_context)
    {
      return false;
    }
    for (const auto& tag_name : application_handler_.filenames_.getTagNames())
    {
      application_handler_.filenames_.getSessionDB().write(
        *db_context,
        tag_name.first,
        application_handler_.filenames_.getTagValue(tag_name.second)
      );
    }
    application_handler_.filenames_.getSessionDB().endWrite(*db_context);
    return true;
  }

}
