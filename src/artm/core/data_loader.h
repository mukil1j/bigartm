// Copyright 2014, Additive Regularization of Topic Models.

#ifndef SRC_ARTM_CORE_DATA_LOADER_H_
#define SRC_ARTM_CORE_DATA_LOADER_H_

#include <atomic>
#include <list>
#include <set>
#include <utility>

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/utility.hpp"
#include "boost/uuid/uuid.hpp"

#include "artm/messages.pb.h"
#include "artm/core/common.h"
#include "artm/core/template_manager.h"
#include "artm/core/thread_safe_holder.h"

namespace artm {
namespace core {

class Instance;
class DiskGeneration;

class DataLoader : boost::noncopyable, public Notifiable {
 public:
  explicit DataLoader(Instance* instance);
  virtual ~DataLoader();

  bool AddBatch(const AddBatchArgs& args);
  virtual void Callback(ModelIncrement* model_increment);
  Instance* instance();
  void InvokeIteration(const InvokeIterationArgs& args);

  // Returns false if BigARTM is still processing the collection, otherwise true.
  bool WaitIdle(const WaitIdleArgs& args);
  void DisposeModel(ModelName model_name);
  bool RequestThetaMatrix(const GetThetaMatrixArgs& get_theta_args,
                          ::artm::ThetaMatrix* theta_matrix);

 private:
  Instance* instance_;
  std::unique_ptr<DiskGeneration> generation_;

  typedef std::pair<boost::uuids::uuid, ModelName> CacheKey;
  ThreadSafeCollectionHolder<CacheKey, DataLoaderCacheEntry> cache_;

  mutable std::atomic<bool> is_stopping;

  // Keep all threads at the end of class members
  // (because the order of class members defines initialization order;
  // everything else should be initialized before creating threads).
  boost::thread thread_;

  void ThreadFunction();
  void PopulateDataStreams(const Batch& batch, ProcessorInput* pi);
};

}  // namespace core
}  // namespace artm

#endif  // SRC_ARTM_CORE_DATA_LOADER_H_
