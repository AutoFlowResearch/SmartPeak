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

#include <SmartPeak/core/ApplicationProcessors/LoadFilenames.h>

namespace SmartPeak
{

  bool LoadFilenames::process()
  {
    LOGD << "START LoadFilenames";
    auto filenames = LoadFilenames::loadFilenamesFromDB(application_handler_.filenames_.getSessionDB().getDBFilePath());
    if (!filenames)
    {
      return false;
    }
    // reset main dir
    (*filenames).setTagValue(Filenames::Tag::MAIN_DIR, application_handler_.main_dir_.generic_string());
    (*filenames).getSessionDB().setDBFilePath(application_handler_.filenames_.getSessionDB().getDBFilePath());
    application_handler_.filenames_ = *filenames;
    LOGD << "END LoadFilenames";
    return true;
  }

  std::optional<Filenames> LoadFilenames::loadFilenamesFromDB(const std::filesystem::path& path_db)
  {
    if (!std::filesystem::exists(path_db))
    {
      LOGE << "Session file does not exist: " << path_db.generic_string();
      return std::nullopt;
    }
    Filenames filenames;
    filenames.getSessionDB().setDBFilePath(path_db);
    auto db_context = filenames.getSessionDB().beginRead(
      "filenames",
      "file_id",
      "filename_pattern",
      "embedded"
    );
    if (!db_context)
    {
      return std::nullopt;
    }
    std::string file_id;
    std::string name_pattern;
    int embedded;
    while (filenames.getSessionDB().read(
      *db_context,
      file_id,
      name_pattern,
      embedded
    ))
    {
      filenames.addFileName(file_id, name_pattern);
      filenames.setEmbedded(file_id, embedded != 0);
    };
    filenames.getSessionDB().endRead(*db_context);

    db_context = filenames.getSessionDB().beginRead(
      "filenames_tags",
      "tag",
      "value"
    );
    if (!db_context)
    {
      return std::nullopt;
    }
    std::string tag;
    std::string value;
    while (filenames.getSessionDB().read(
      *db_context,
      tag,
      value
    ))
    {
      filenames.setTagValue(filenames.getTagNames().at(tag), value);
    };
    filenames.getSessionDB().endRead(*db_context);
    return filenames;
  }

}
