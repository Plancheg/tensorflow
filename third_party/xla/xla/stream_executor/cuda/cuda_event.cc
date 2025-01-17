/* Copyright 2015 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "xla/stream_executor/cuda/cuda_event.h"

#include "absl/status/statusor.h"
#include "third_party/gpus/cuda/include/cuda.h"
#include "xla/stream_executor/cuda/cuda_stream.h"
#include "xla/stream_executor/gpu/gpu_executor.h"

namespace stream_executor {
namespace gpu {

Event::Status GpuEvent::PollForStatus() {
  absl::StatusOr<CUresult> status =
      GpuDriver::QueryEvent(parent_->gpu_context(), gpu_event_);
  if (!status.ok()) {
    LOG(ERROR) << "Error polling for event status: "
               << status.status().message();
    return Event::Status::kError;
  }

  switch (status.value()) {
    case CUDA_SUCCESS:
      return Event::Status::kComplete;
    case CUDA_ERROR_NOT_READY:
      return Event::Status::kPending;
    default:
      LOG(INFO) << "Error condition returned for event status: "
                << status.value();
      return Event::Status::kError;
  }
}

}  // namespace gpu
}  // namespace stream_executor
