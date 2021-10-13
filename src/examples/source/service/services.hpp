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

#include <SmartPeak/core/Server.h>

#include "workflow_grpc.h"
#include <grpcpp/grpcpp.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

// client-side
class WorkflowClientS {
  public:
  WorkflowClientS(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  std::string runWorkflow(const std::string& sequence_file_path,
                          SmartPeak::SessionHandler::GraphVizData& graph_data_out,
                          SmartPeak::SessionHandler::HeatMapData& heatmap_data_out) {

    SmartPeakServer::WorkflowParameters workflow_parameters;
    workflow_parameters.set_sequence_file(sequence_file_path);
    SmartPeakServer::WorkflowResult workflow_status;
    grpc::ClientContext context;
    
    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());
    grpc::Status status = stub_->runWorkflow(&context, workflow_parameters, &workflow_status);
    
    auto graph_data = workflow_status.graph_data();
    auto heatmap_data = workflow_status.heatmap_data();
    
    // heatmap_data
    heatmap_data_out.selected_sample_names_.resize(heatmap_data.selected_sample_names_size());
    for (size_t i = 0; i < heatmap_data.selected_sample_names_size(); ++i) {
      heatmap_data_out.selected_sample_names_(i)  = heatmap_data.selected_sample_names(i);
    }
    
    heatmap_data_out.selected_transitions_.resize(heatmap_data.selected_transitions_size());
    for (size_t i = 0; i < heatmap_data.selected_transitions_size(); ++i) {
      heatmap_data_out.selected_transitions_(i)  = heatmap_data.selected_transitions(i);
    }
    
    heatmap_data_out.selected_transition_groups_.resize(heatmap_data.selected_transition_groups_size());
    for (size_t i = 0; i < heatmap_data.selected_transition_groups_size(); ++i) {
      heatmap_data_out.selected_transition_groups_(i)  = heatmap_data.selected_transition_groups(i);
    }
    
    
    heatmap_data_out.feat_heatmap_col_labels.resize(heatmap_data.header_column_size());
    for (size_t i = 0; i < heatmap_data.header_column_size(); ++i) {
      heatmap_data_out.feat_heatmap_col_labels(i)  = heatmap_data.header_column(i);
    }
    
    heatmap_data_out.feat_heatmap_row_labels.resize(heatmap_data.header_row_size());
    for (size_t i = 0; i < heatmap_data.header_row_size(); ++i) {
      heatmap_data_out.feat_heatmap_row_labels(i)  = heatmap_data.header_row(i);
    }
    
    heatmap_data_out.feat_heatmap_data.resize(heatmap_data.header_column_size(), heatmap_data.header_row_size());
    for (size_t i = 0; i < heatmap_data.header_column_size(); ++i) {
      for (size_t j = 0; j < heatmap_data.header_row_size(); ++j) {
        heatmap_data_out.feat_heatmap_data(i,j) = heatmap_data.column_data(i).axis_data(j);
      }
    }
    
    heatmap_data_out.feat_heatmap_x_axis_title = heatmap_data.x_axis_title();
    heatmap_data_out.feat_heatmap_y_axis_title = heatmap_data.y_axis_title();
    heatmap_data_out.feat_value_min_ = heatmap_data.feat_value_min();
    heatmap_data_out.feat_value_max_ = heatmap_data.feat_value_max();
    heatmap_data_out.selected_feature_ = heatmap_data.selected_feature();
    
    // graph_data
    graph_data_out.series_names_area_.resize(graph_data.series_names_area_size(), "");
    for (size_t i = 0; i < graph_data.series_names_area_size(); ++i) {
      graph_data_out.series_names_area_[i]  = graph_data.series_names_area(i);
    }
    
    assert(graph_data.x_data_size()==graph_data.y_data_size());
    graph_data_out.x_data_area_.resize(graph_data.x_data_size());
    graph_data_out.y_data_area_.resize(graph_data.y_data_size());
    for (size_t i = 0; i < graph_data.x_data_size(); ++i) {
      ::SmartPeakServer::SingleAxisData x_data = graph_data.x_data(i);
      ::SmartPeakServer::SingleAxisData y_data = graph_data.y_data(i);
      graph_data_out.x_data_area_[i].resize(x_data.axis_data_size(), 0);
      graph_data_out.y_data_area_[i].resize(y_data.axis_data_size(), 0);
      assert(x_data.axis_data_size()==y_data.axis_data_size());
      for (size_t j = 0; j < x_data.axis_data_size(); ++j) {
        graph_data_out.x_data_area_[i][j] = x_data.axis_data(j);
        graph_data_out.y_data_area_[i][j] = y_data.axis_data(j);
      }
    }
    
    graph_data_out.series_names_scatter_.resize(graph_data.series_names_scatter_size(), "");
    for (size_t i = 0; i < graph_data.series_names_scatter_size(); ++i) {
      graph_data_out.series_names_scatter_[i]  = graph_data.series_names_scatter(i);
    }
    
    assert(graph_data.x_data_scatter_size()==graph_data.y_data_scatter_size());
    graph_data_out.x_data_scatter_.resize(graph_data.x_data_scatter_size());
    graph_data_out.y_data_scatter_.resize(graph_data.y_data_scatter_size());
    for (size_t i = 0; i < graph_data.x_data_scatter_size(); ++i) {
      ::SmartPeakServer::SingleAxisData x_data = graph_data.x_data_scatter(i);
      ::SmartPeakServer::SingleAxisData y_data = graph_data.y_data_scatter(i);
      graph_data_out.x_data_scatter_[i].resize(x_data.axis_data_size(), 0);
      graph_data_out.y_data_scatter_[i].resize(y_data.axis_data_size(), 0);
      assert(x_data.axis_data_size()==y_data.axis_data_size());
      for (size_t j = 0; j < x_data.axis_data_size(); ++j) {
        graph_data_out.x_data_scatter_[i][j] = x_data.axis_data(j);
        graph_data_out.y_data_scatter_[i][j] = y_data.axis_data(j);
      }
    }
    
    graph_data_out.x_axis_title_ = graph_data.x_axis_title();
    graph_data_out.y_axis_title_ = graph_data.y_axis_title();
    
    graph_data_out.x_min_ = graph_data.x_min();
    graph_data_out.x_max_ = graph_data.x_max();
    graph_data_out.y_min_ = graph_data.y_min();
    graph_data_out.y_max_ = graph_data.y_max();
    
    graph_data_out.nb_points_ = graph_data.nb_points();
    graph_data_out.max_nb_points_ = graph_data.max_nb_points();

    if (status.ok()) {
      return workflow_status.status_code();
    } else {
      std::cout << status.error_code() << ": " << status.error_message() << std::endl;
      return "RPC failed";
    }
}

 private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};

class WorkflowClientA {
 public:
  WorkflowClientA(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  std::string runWorkflow(const std::string& sequence_file_path) {
    SmartPeakServer::WorkflowParameters workflow_parameters; // request
    workflow_parameters.set_sequence_file(sequence_file_path);
    SmartPeakServer::WorkflowResult workflow_status; // reply

    grpc::ClientContext context;
    grpc::CompletionQueue cq;
    grpc::Status status;

    context.AddMetadata("smartpeak_version", SmartPeak::Utilities::getSmartPeakVersion());

    AsyncClientCall* call = new AsyncClientCall;
    call->response_reader = stub_->AsyncrunWorkflow(&call->context, workflow_parameters, &cq_);
}
  
  void AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;

    while (cq_.Next(&got_tag, &ok)) {
      ResponseHandler* responseHandler = static_cast<ResponseHandler*>(got_tag);
      std::cout << "Tag received: " << responseHandler << std::endl;
      std::cout << "Next returned: " << ok << std::endl;
      responseHandler->HandleResponse(ok);
    }
  }

 private:
  class ResponseHandler {
      public:
        virtual bool HandleResponse(bool eventStatus) = 0;
      };
  
  class AsyncClientCall: public ResponseHandler {
      enum CallStatus {CREATE, PROCESS, FINISH};
      CallStatus callStatus_;
    public:

      AsyncClientCall(): callStatus_(CREATE) {}

    SmartPeakServer::WorkflowResult reply;
      grpc::ClientContext context;
      grpc::Status status;
      
      std::unique_ptr<grpc::ClientAsyncResponseReader<SmartPeakServer::WorkflowResult>> response_reader;

      bool HandleResponse(bool responseStatus) override {
        switch (callStatus_) {
        case CREATE:
          if (responseStatus) {
            //response_reader->Read(&reply, (void*)this);
            response_reader->StartCall();
            callStatus_ = PROCESS;
          } else {
            //response_reader->Finish(&status, (void*)this);
            response_reader->Finish(&reply, &status, (void*)this);
            callStatus_ = FINISH;
          }
          break;
        case PROCESS:
          if (responseStatus) {
            std::cout << ">>> GUI received : " << this << " : " << reply.status_code() << std::endl;
            //response_reader->Read(&reply, (void*)this);
            response_reader->ReadInitialMetadata((void*)this);
          } else {
            //response_reader->Finish(&status, (void*)this);
            response_reader->Finish(&reply, &status, (void*)this);
            callStatus_ = FINISH;
          }
          break;
        case FINISH:
          if (status.ok()) {
            std::cout << ">>> GUI : Server Response Completed: " << this << " CallData: " << this << std::endl;
          } else {
            std::cout << ">>> GUI : RPC failed" << std::endl;
          }
          delete this;
        }
      }
    };
  
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
  grpc::CompletionQueue cq_;
};

class LogStreamClientS {
 public:
  LogStreamClientS(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  void getLogstream() {
    SmartPeakServer::InquireLogs inquire_logs;
    SmartPeakServer::LogStream log_stream;
    grpc::ClientContext context;

    inquire_logs.set_nr_lines(1000);

    std::unique_ptr<grpc::ClientReader<SmartPeakServer::LogStream> > reader(
        stub_->getLogStream(&context, inquire_logs)  );
    while (reader->Read(&log_stream)) {
      LOGI << "SERVER LOG : " << log_stream.log_line();
    }
    grpc::Status status = reader->Finish();
    if (status.ok()) {
      std::cout << "GUI >> getLogStream-rpc succeeded." << std::endl;
    } else {
      std::cout << "GUI >> getLogStream-rpc failed." << std::endl;
    }
  }

 private:
  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
};


class LogStreamClientA {
 public:
  LogStreamClientA(std::shared_ptr<grpc::Channel> channel)
      : stub_(SmartPeakServer::Workflow::NewStub(channel)) {}

  void getLogstream() {
    SmartPeakServer::InquireLogs inquire_logs;
    SmartPeakServer::LogStream log_stream;
    grpc::ClientContext context;
    grpc::CompletionQueue cq;
    grpc::Status status;

    inquire_logs.set_nr_lines(500);

    AsyncClientCall* call = new AsyncClientCall;
    
    call->response_reader = stub_->AsyncgetLogStream(&call->context, inquire_logs, &cq_, (void *)call);
  }

  void AsyncCompleteRpc() {
    void* got_tag;
    bool ok = false;

    while (cq_.Next(&got_tag, &ok)) {
      ResponseHandler* responseHandler = static_cast<ResponseHandler*>(got_tag);
      std::cout << "Tag received: " << responseHandler << std::endl;

      std::cout << "Next returned: " << ok << std::endl;
      responseHandler->HandleResponse(ok);
    }
  }

 private:

  class ResponseHandler {
    public:
      virtual bool HandleResponse(bool eventStatus) = 0;
    };

  class AsyncClientCall: public ResponseHandler {
    enum CallStatus {CREATE, PROCESS, FINISH};
    CallStatus callStatus_;
  public:

    AsyncClientCall(): callStatus_(CREATE) {}

    SmartPeakServer::LogStream reply;
    grpc::ClientContext context;
    grpc::Status status;
    
    std::unique_ptr<grpc::ClientAsyncReaderInterface<SmartPeakServer::LogStream>> response_reader;

    bool HandleResponse(bool responseStatus) override {
      switch (callStatus_) {
      case CREATE:
        if (responseStatus) {
          response_reader->Read(&reply, (void*)this);
          callStatus_ = PROCESS;
        } else {
          response_reader->Finish(&status, (void*)this);
          callStatus_ = FINISH;
        }
        break;
      case PROCESS:
        if (responseStatus) {
          std::cout << ">>> GUI received : " << this << " : " << reply.log_line() << std::endl;
          response_reader->Read(&reply, (void*)this);
        } else {
          response_reader->Finish(&status, (void*)this);
          callStatus_ = FINISH;
        }
        break;
      case FINISH:
        if (status.ok()) {
          std::cout << ">>> GUI : Server Response Completed: " << this << " CallData: " << this << std::endl;
        } else {
          std::cout << ">>> GUI : RPC failed" << std::endl;
        }
        delete this;
      }
    }
  };

  std::unique_ptr<SmartPeakServer::Workflow::Stub> stub_;
  grpc::CompletionQueue cq_;
};

// server-side
class WorkflowService final : public SmartPeakServer::Workflow::Service {
public:
  
  explicit WorkflowService() : is_logger_init_(false), session_id_(SmartPeak::Utilities::makeUniqueStringFromTime()) {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Starting SmartPeak Server version " + SmartPeak::Utilities::getSmartPeakVersion());
    is_logger_init_ = true;
  }
  
  virtual ::grpc::Status runWorkflow(::grpc::ServerContext* context,
                                     const ::SmartPeakServer::WorkflowParameters* request,
                                     ::SmartPeakServer::WorkflowResult* response) override {

    server_manager_.sequence_file = request->sequence_file();
    auto [job_done, log_records] = SmartPeak::serv::handleWorkflowRequest(&server_manager_, is_logger_init_);
    const auto app_hand = server_manager_.get_application_handler();
    const auto seq_hand = app_hand.sequenceHandler_;
    
    if (!std::filesystem::exists(app_hand.main_dir_ / session_id_)) {
      std::filesystem::create_directory(app_hand.main_dir_ / session_id_);
    }
    
    SmartPeak::SessionHandler::GraphVizData generated_graph_data;
    SmartPeak::SessionHandler::HeatMapData generated_heatmap_data;
    
    std::set<std::string> sample_names, component_names, component_group_names;
    for (const auto& injection : seq_hand.getSequence()) {
      sample_names.insert(injection.getMetaData().getSampleName());
      for (const auto& chromatogram : injection.getRawData().getChromatogramMap().getChromatograms()) {
        component_names.insert(chromatogram.getNativeID());
      }
    }
    
    for (const auto& injection : seq_hand.getSequence()) {
      for (const auto& feature : injection.getRawData().getFeatureMapHistory()) {
        if(!feature.getMetaValue("PeptideRef").isEmpty()) {
          component_group_names.insert(feature.getMetaValue("PeptideRef"));
        }
      }
    }
    
    //sample_names.insert("150601_0_BloodProject01_PLT_QC_Broth-1");
    //sample_names.insert("150601_0_BloodProject01_PLT_QC_Broth-1-10.0x");
    //component_names.insert("23dpg.23dpg_1.Heavy");
    //component_names.insert("2mcit.2mcit_1.Light");
    //component_names.insert("2mcit.2mcit_2.Light");
    //component_names.insert("2obut.2obut_1.Light");
    //component_group_names.insert("2obut");
    //component_group_names.insert("2mcit");
    
    
    server_manager_.get_session_handler().setFeatureExplorer();
    Eigen::Tensor<std::string, 2> selected_feature_names =
    server_manager_.get_session_handler().feature_table.body_;

    std::vector<std::string> feature_names;
    for (int i = 0; i < selected_feature_names.size(); ++i) {
      feature_names.push_back(selected_feature_names(i));
    }
    
    auto t1_getChromatogramScatterPlot = std::chrono::high_resolution_clock::now();
    server_manager_.get_session_handler().getChromatogramScatterPlot(seq_hand, generated_graph_data, std::make_pair(0,2000),
                                                                     sample_names, component_names, 6000000);
    auto t2_getChromatogramScatterPlot = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_getChromatogramScatterPlot = t2_getChromatogramScatterPlot - t1_getChromatogramScatterPlot;
    std::cout << ">> Time to process all chromatogram data : " << duration_getChromatogramScatterPlot.count() << "ms" << std::endl;
    
    auto t1_getHeatMap = std::chrono::high_resolution_clock::now();
    server_manager_.get_session_handler().getHeatMap(seq_hand, generated_heatmap_data, "asymmetry_factor",
                                                     sample_names, component_names, component_group_names);
    auto t2_getHeatMap = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration_getHeatMap = t2_getHeatMap - t1_getHeatMap;
    std::cout << ">> Time to process all heatmap data for asymmetry_factor : " << duration_getHeatMap.count() << "ms" << std::endl;
    
    // heatmap data
    ::SmartPeakServer::HeatmapData heatmap_data;
    for (size_t i = 0; i < generated_heatmap_data.selected_sample_names_.size(); ++i) {
      heatmap_data.add_selected_sample_names();
      heatmap_data.set_selected_sample_names(i, generated_heatmap_data.selected_sample_names_[i]);
    }
    
    for (size_t i = 0; i < generated_heatmap_data.selected_transitions_.size(); ++i) {
      heatmap_data.add_selected_transitions();
      heatmap_data.set_selected_transitions(i, generated_heatmap_data.selected_transitions_[i]);
    }
    
    for (size_t i = 0; i < generated_heatmap_data.selected_transition_groups_.size(); ++i) {
      heatmap_data.add_selected_transition_groups();
      heatmap_data.set_selected_transition_groups(i, generated_heatmap_data.selected_transition_groups_[i]);
    }
    
    for (size_t i = 0; i < generated_heatmap_data.feat_heatmap_row_labels.size(); ++i) {
      heatmap_data.add_header_row();
      heatmap_data.set_header_row(i, generated_heatmap_data.feat_heatmap_row_labels[i]);
    }
    for (size_t i = 0; i < generated_heatmap_data.feat_heatmap_col_labels.size(); ++i) {
      heatmap_data.add_header_column();
      heatmap_data.set_header_column(i, generated_heatmap_data.feat_heatmap_col_labels[i]);
    }
    
    for (size_t i = 0; i < generated_heatmap_data.feat_heatmap_col_labels.size(); ++i) {
      ::SmartPeakServer::SingleAxisData* col_data = heatmap_data.add_column_data();
      for (size_t j = 0; j < generated_heatmap_data.feat_heatmap_row_labels.size(); ++j) {
        col_data->add_axis_data(generated_heatmap_data.feat_heatmap_data(i,j));
      }
    }
    
    heatmap_data.set_x_axis_title(generated_heatmap_data.feat_heatmap_x_axis_title);
    heatmap_data.set_y_axis_title(generated_heatmap_data.feat_heatmap_y_axis_title);
    heatmap_data.set_selected_feature(generated_heatmap_data.selected_feature_);
    heatmap_data.set_feat_value_min(generated_heatmap_data.feat_value_min_);
    heatmap_data.set_feat_value_max(generated_heatmap_data.feat_value_max_);
    
    // graph_data
    ::SmartPeakServer::GraphData graph_data;
    for (size_t i = 0; i < generated_graph_data.series_names_area_.size(); ++i) {
      graph_data.add_series_names_area();
      graph_data.set_series_names_area(i, generated_graph_data.series_names_area_[i]);
    }
    
    assert(generated_graph_data.x_data_area_.size()==generated_graph_data.y_data_area_.size());
    for (size_t i = 0; i < generated_graph_data.x_data_area_.size(); ++i) {
      ::SmartPeakServer::SingleAxisData* x_data = graph_data.add_x_data();
      ::SmartPeakServer::SingleAxisData* y_data = graph_data.add_y_data();
      for (size_t j = 0; j < generated_graph_data.x_data_area_.at(i).size(); ++j) {
        x_data->add_axis_data(generated_graph_data.x_data_area_[i][j]);
        y_data->add_axis_data(generated_graph_data.y_data_area_[i][j]);
      }
    }
    
    for (size_t i = 0; i < generated_graph_data.series_names_scatter_.size(); ++i) {
      graph_data.add_series_names_scatter();
      graph_data.set_series_names_scatter(i, generated_graph_data.series_names_scatter_[i]);
    }
    
    assert(generated_graph_data.x_data_scatter_.size()==generated_graph_data.x_data_scatter_.size());
    for (size_t i = 0; i < generated_graph_data.x_data_scatter_.size(); ++i) {
      ::SmartPeakServer::SingleAxisData* x_data_scatter = graph_data.add_x_data_scatter();
      ::SmartPeakServer::SingleAxisData* y_data_scatter = graph_data.add_y_data_scatter();
      for (size_t j = 0; j < generated_graph_data.x_data_scatter_.at(i).size(); ++j) {
        x_data_scatter->add_axis_data(generated_graph_data.x_data_scatter_[i][j]);
        y_data_scatter->add_axis_data(generated_graph_data.y_data_scatter_[i][j]);
      }
    }
    
    graph_data.set_x_axis_title(generated_graph_data.x_axis_title_);
    graph_data.set_y_axis_title(generated_graph_data.y_axis_title_);
    
    graph_data.set_x_min(generated_graph_data.x_min_);
    graph_data.set_x_max(generated_graph_data.x_max_);
    graph_data.set_y_min(generated_graph_data.y_min_);
    graph_data.set_y_max(generated_graph_data.y_max_);
    
    graph_data.set_nb_points(generated_graph_data.nb_points_);
    graph_data.set_max_nb_points(generated_graph_data.max_nb_points_);
    
    assert(graph_data.series_names_area_size()==generated_graph_data.series_names_area_.size());
    assert(graph_data.x_data_size()==generated_graph_data.x_data_area_.size());
    assert(graph_data.y_data_size()==generated_graph_data.y_data_area_.size());
    assert(graph_data.z_data_size()==generated_graph_data.z_data_area_.size());
    assert(graph_data.series_names_scatter_size()==generated_graph_data.series_names_scatter_.size());
    assert(graph_data.x_data_scatter_size()==generated_graph_data.x_data_scatter_.size());
    assert(graph_data.y_data_scatter_size()==generated_graph_data.y_data_scatter_.size());
    assert(graph_data.x_axis_title()==generated_graph_data.x_axis_title_);
    assert(graph_data.y_axis_title()==generated_graph_data.y_axis_title_);
    assert(graph_data.x_min()==generated_graph_data.x_min_);
    assert(graph_data.x_max()==generated_graph_data.x_max_);
    assert(graph_data.y_min()==generated_graph_data.y_min_);
    assert(graph_data.y_max()==generated_graph_data.y_max_);
    assert(graph_data.nb_points()==generated_graph_data.nb_points_);
    assert(graph_data.max_nb_points()==generated_graph_data.max_nb_points_);
    
    // graph_data : 1404 KB, heatmap_data : 7 KB
    std::cout << ">>>> graph plot data pb size : " << graph_data.ByteSizeLong()/1000 << " KB" << std::endl;
    std::cout << ">>>> heatmap plot data pb size : " << heatmap_data.ByteSizeLong()/1000 << " KB" << std::endl;
    
    // open streams
    auto graph_data_path = app_hand.main_dir_ / session_id_ / "graph_viz_data.pb";
    std::ofstream ofs(graph_data_path.string(), std::ios_base::out | std::ios_base::binary);
    graph_data.SerializeToOstream(&ofs);
    
    response->set_allocated_heatmap_data(new ::SmartPeakServer::HeatmapData(heatmap_data));
    response->set_allocated_graph_data(new ::SmartPeakServer::GraphData(graph_data));
    
    response->set_status_code( job_done ? "YES" : "NO" );
    
    // makeUniqueStringFromTime :
    // - folder to where results are saved
    // - session_<session_id_>.json
    response->set_session_id(session_id_);
    response->set_path_to_results(app_hand.main_dir_.string()); //exported reports
    return grpc::Status::OK;
  }
  
  ::grpc::Status getLogStream(::grpc::ServerContext* context,
                              const ::SmartPeakServer::InquireLogs* request,
                              ::grpc::ServerWriter< ::SmartPeakServer::LogStream>* writer) override {
    
    auto logstream = console_handler_->gui_appender_.getAppenderRecordList(plog::debug);
    auto nr_lines = request->nr_lines();
    int message_list_start = (logstream.size() > nr_lines) ? logstream.size() - nr_lines : 0;
    for (int i = message_list_start; i < logstream.size(); ++i)
    {
      std::string logline_str(logstream.at(i).second.data(), logstream.at(i).second.data() + logstream.at(i).second.size());
      SmartPeakServer::LogStream logline;
      logline.set_log_line(logline_str);
      writer->Write(logline);
    }
    
    return grpc::Status::OK;
  }
  
private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  SmartPeakServer::Workflow::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
  bool is_logger_init_;
  std::string session_id_;
  SmartPeak::ConsoleHandler* console_handler_;
  SmartPeak::serv::ServerManager server_manager_;
};

void runSyncServer(std::string server_address) {
  WorkflowService workflow_service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  grpc::ServerBuilder builder;
  
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&workflow_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << ">> SERVER listening on : " << server_address << std::endl;

  server->Wait();
}

struct CallData {
  SmartPeakServer::Workflow::AsyncService* service;
  grpc::ServerCompletionQueue* cq;
  SmartPeak::ConsoleHandler* console_handler_;
  SmartPeak::serv::ServerManager* server_manager_;
  std::vector<SmartPeak::GuiAppender::GuiAppenderRecord> log_records;
  
  CallData(SmartPeakServer::Workflow::AsyncService* service, grpc::ServerCompletionQueue* cq) : service(service), cq(cq) {
    auto [logfilepath, logdir_created] = SmartPeak::Utilities::getLogFilepath("smartpeak_log");
    console_handler_ = &SmartPeak::ConsoleHandler::get_instance();
    console_handler_->set_log_directory(std::filesystem::path(logfilepath).parent_path().string());
    console_handler_->use_colors(false);
    console_handler_->set_severity(plog::debug);
    console_handler_->initialize("Start SmartPeak version " + SmartPeak::Utilities::getSmartPeakVersion());
  }
};

class Call {
 public:
  virtual void Proceed(bool ok) = 0;
};

class RunWorkflowCall final : public Call {
 public:
  explicit RunWorkflowCall(CallData* data) :
    data_(data), responder_(&ctx_), status_(REQUEST), is_logger_init(false)
  {
    data->service->RequestrunWorkflow(&ctx_, &request_, &responder_, data_->cq, data_->cq, this);
  }

  void Proceed(bool ok) {
    std::string value;
    int shard;

    switch (status_) {
      case REQUEST:
      {
        new RunWorkflowCall(data_);
        if (!ok) {
          std::cerr << "RunWorkflowCall has failed. Finishing." << std::endl;
          responder_.FinishWithError(grpc::Status::CANCELLED, this);
          status_ = FINISH;
          break;
        }
          
        std::string sequence_file = "";
        sequence_file = request_.sequence_file();
        data_->server_manager_->sequence_file = sequence_file;
        
        auto [job_done, log_records] = SmartPeak::serv::handleWorkflowRequest(data_->server_manager_, is_logger_init);
        data_->log_records = log_records;
        response_.set_status_code(job_done?"OK":"FAILED");
        responder_.Finish(response_, grpc::Status::OK, this);
        status_ = FINISH;
        break;
      }

      case FINISH:
      {
        if (!ok) {
          std::cerr << "RunWorkflowCall finished unexpectedly" << std::endl;
          delete this;
          break;
        }
      }
    }
  }

 private:
  CallData* data_;
  grpc::ServerContext ctx_;
  grpc::ServerAsyncResponseWriter<::SmartPeakServer::WorkflowResult> responder_;
  ::SmartPeakServer::WorkflowParameters request_;
  ::SmartPeakServer::WorkflowResult response_;
  enum CallStatus { REQUEST, FINISH };
  CallStatus status_;
  bool is_logger_init;
};

class GetLogStreamCall final : public Call {
 public:
  explicit GetLogStreamCall(CallData* data) :
    data_(data), responder_(&ctx_), status_(REQUEST)
  {
    data_->service->RequestgetLogStream(&ctx_, &request_, &responder_, data_->cq, data_->cq, (void*)this);
  }

  void Proceed(bool ok) {
    switch (status_) {
      case REQUEST:
      {
        new GetLogStreamCall(data_);
        if (!ok) {
          std::cerr << "GetLogStreamCall has failed. Finishing." << std::endl;
          responder_.Finish(grpc::Status::CANCELLED, this);
          status_ = FINISH;
          break;
        }

        //auto logstream = data_->log_records;
        auto logstream = data_->console_handler_->gui_appender_.getAppenderRecordList(plog::debug);
        auto nr_lines = request_.nr_lines();
        int message_list_start = (logstream.size() > nr_lines) ? logstream.size() - nr_lines : 0;
        for (int i = message_list_start; i < logstream.size(); ++i)
        {
          std::string logline_str(logstream.at(i).second.data(), logstream.at(i).second.data() + logstream.at(i).second.size());
          response_.Clear();
          response_.set_log_line(logline_str);
          responder_.Write(response_, this);
        }

        responder_.Finish(grpc::Status::OK, this);
        status_ = FINISH;
        break;
      }
        
      case FINISH:
      {
        if (!ok)
          std::cerr << "GetLogStreamCall finished unexpectedly" << std::endl;
        delete this;
        break;
      }
    }
  }

 private:
  CallData* data_;
  grpc::ServerContext ctx_;
  grpc::ServerAsyncWriter<::SmartPeakServer::LogStream> responder_;
  ::SmartPeakServer::InquireLogs request_;
  ::SmartPeakServer::LogStream response_;
  enum CallStatus { REQUEST, FINISH };
  CallStatus status_;
};

class AsyncServer final {
 public:
  AsyncServer(const std::string& listening_address) {}
  ~AsyncServer()  {
    server_->Shutdown();
    cq_->Shutdown();
  }

  void init(const std::string& listening_address,
                         const std::string& hostname) {
    grpc::ServerBuilder builder;
    int selected_port;
    builder.AddListeningPort(listening_address, grpc::InsecureServerCredentials(),
                             &selected_port);
    builder.RegisterService(&service_);
    cq_ = builder.AddCompletionQueue();
    server_ = builder.BuildAndStart();
    if (selected_port == 0) {
      std::cerr << "Could not bind to given port" << std::endl;
      exit(EXIT_FAILURE);
    }
    std::string port = std::to_string(selected_port);
    std::string node_address = hostname + ":" + port;
    std::cerr << "Async SmartPeak Server listening on port " << port << std::endl;
  }
  
  void run() {
    CallData data{&service_, cq_.get()};
    new RunWorkflowCall(&data);
    new GetLogStreamCall(&data);
    void* tag;
    bool ok;
    for (;;) {
      if (!cq_->Next(&tag, &ok)) {
        std::cerr << "Shutting down..." << std::endl;
        break;
      }
      static_cast<Call*>(tag)->Proceed(ok);
    }
  }

 private:
  SmartPeakServer::Workflow::AsyncService service_;
  std::unique_ptr<grpc::Server> server_;
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
  void* call_ = nullptr;
  std::thread watcher_;
};
