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
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <exception>
#include <iomanip>

namespace SmartPeak {

  enum PlotLineSetting {LineStyle, LineWidth, Marker, MarkerSize, Color};

  class PlotLineProperties {
  public:
    PlotLineProperties(uint index = 0);
    
    std::string plotStyler() const;
    bool pointtypeNone() const;
    static uint getGridLineType();
    static void resetLineCount();

    void set(const PlotLineSetting property, const std::string &value);
    void set(const std::pair<PlotLineSetting, std::string> &input);

  private:
    uint        line_index_;
    static uint line_count_;
    std::string colour_;
    double      line_width_ = 1;
    double      point_size_ = 1;
    std::string line_type_  = "-";
    std::string point_type_ = "none";

    static const std::vector<std::string>   basic_colours_hex_;
    static const std::map<std::string, int> linetype_cairo_term_;
    static const std::map<std::string, int> pointtype_generic_term_;
  };


  class PlotExporter
  {
  public:
    PlotExporter(std::string output_path, SessionHandler::GraphVizData& graphvis_data, int mode);
    
    void plot();
    
  private:
    bool isTermAvailable_(std::string term_name);
    void setGrid_(std::ofstream &fout);
    std::vector<std::string> getLegend_();
    void generatePNG_();
    void generatePDF_();
    void generateHTML_();
    void generateSVG_();
    void appendFileHeader_(std::ofstream &fout);
    void generatePlot_(std::ofstream &fout, const std::string &filename);
    
    uint        nr_plots_;
    bool        with_grid_;
    std::string output_path_;
    std::string filename = "smartpeak-exported-plot";
    std::string x_title_;
    std::string y_title_;
    std::string plot_title_;
    
    std::vector<PlotLineProperties> plotlines_properties_;
    SessionHandler::GraphVizData& graphvis_data_;
    
    bool plot_PNG_  = false;
    bool plot_PDF_  = false;
    bool plot_HTML_ = false;
    bool plot_SVG_  = false;

    bool canvas_available_  = false;
    bool cairo_available_   = false;
    bool svg_available_     = false;
  };
}
