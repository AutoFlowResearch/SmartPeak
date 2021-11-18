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

  /**
  @brief Plot line properties manager
  */
  class PlotLineProperties {
  public:
    PlotLineProperties(uint index = 0);
    
    /*
    @brief styles plot

    @param[out] styling string
    */
    std::string plotStyler() const;
    
    /*
    @brief check if point type is of style "none"

    @param[out] true if of style "none"
    */
    bool pointtypeNone() const;
    
    /*
    @brief reads grid line type

    @param[out] grid line type
    */
    static uint getGridLineType();
    
    /*
    @brief resets line count
    */
    static void resetLineCount();

    /*
    @brief sets plotline settings

    @param[int]     property plotline property
    @param[int,out] property sets plotline property
    */
    void set(const PlotLineSetting property, const std::string &value);
    
    /*
    @brief sets plotline settings

    @param[int,out] property plotline property and property sets plotline property paired
    */
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

  /**
  @brief Class for generating plots
  */
  class PlotExporter
  {
  public:
    
    enum class PlotType {
      CURVE,
      HEATMAP
    };
    
    enum class ExportedFormat {
      PNG,
      PDF,
      SVG
    };
  
    PlotExporter(std::string output_path, SessionHandler::GraphVizData& graphvis_data, int format, PlotType plot_type);
    
    PlotExporter(std::string output_path, SessionHandler::HeatMapData& graphvis_data, int format, PlotType plot_type);
    
    /*
    @brief starts plotting

    @param[out] true if plotting is successful
    */
    bool plot();
    
    void setGNUPLOTPath(std::string gnuplot_path) { gnuplot_path_ = gnuplot_path; };
    
  private:
    /*
    @brief checks gnuplot-term availability

    @param[out] true if term found
    */
    bool isTermAvailable_(std::string term_name);
    
    /*
    @brief checks gnuplot availability on host system

    @param[out] true if detected
    */
    bool isGNUPLOTPresent_();
    
    /*
    @brief sets grid visibility
     
    @param[in] fout filestream to append to
    */
    void setGrid_(std::ofstream &fout);
    
    /*
    @brief get list of plotnames/legend
     
    @param[out] list of legends for the plot
    */
    std::vector<std::string> getLegend_();
    
    /*
    @brief generates PNG plots
    */
    void generatePNG_();
    
    /*
    @brief generates PDF plots
    */
    void generatePDF_();
    
    /*
    @brief generates SVG plots
    */
    void generateSVG_();
    
    /*
    @brief adds basic gnuplot script information
     
    @param[in] fout filestream to append to
    */
    void appendFileHeader_(std::ofstream &fout);
    
    /*
    @brief discards temporary files
    */
    void removeTempFiles_();
    
    /*
    @brief gnuplot executer
     
    @param[in] fout filestream to append to
    @param[in] path to generated gnuplot script
    */
    void generatePlot_(std::ofstream &fout, const std::string &filename, const ExportedFormat exported_format);
    
    uint            nr_plots_;
    bool            with_grid_;
    std::string     output_path_;
    std::string     filename = "smartpeak-exported-plot";
    std::string     x_title_;
    std::string     y_title_;
    std::string     plot_title_;
    std::string     gnuplot_path_;
    uint            file_width_;
    uint            file_height_;
    PlotType        plot_type_;
    ExportedFormat  exported_format_;
    
    std::vector<PlotLineProperties> plotlines_properties_;
    SessionHandler::GraphVizData    graphvis_data_;
    SessionHandler::HeatMapData     heatmap_data_;
    
    bool plot_PNG_  = false;
    bool plot_PDF_  = false;
    bool plot_SVG_  = false;
  };
}
