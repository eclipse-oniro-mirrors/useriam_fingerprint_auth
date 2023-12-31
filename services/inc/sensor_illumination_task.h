/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SENSOR_ILLUMINATION_TASK
#define SENSOR_ILLUMINATION_TASK

#include <mutex>

#include "display_manager.h"
#include "nocopyable.h"
#include "timer.h"
#include "ui/rs_surface_node.h"

#include "iam_common_defines.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class SensorIlluminationTask : public std::enable_shared_from_this<SensorIlluminationTask>, public NoCopyable {
public:
    SensorIlluminationTask();
    ~SensorIlluminationTask() override;

    UserAuth::ResultCode EnableSensorIllumination(uint32_t centerX, uint32_t centerY, uint32_t radius, uint32_t color);
    UserAuth::ResultCode DisableSensorIllumination();
    UserAuth::ResultCode TurnOnSensorIllumination();
    UserAuth::ResultCode TurnOffSensorIllumination();
    void OnDisplayTimeOut();

private:
    std::shared_ptr<Rosen::RSSurfaceNode> currRsSurface_;
    Rosen::DisplayId defaultDisplayId_;
    Utils::Timer timer_;
    uint32_t currTimerId_;
    std::recursive_mutex recursiveMutex_;
    bool isIlluminationOn_ = false;
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // SENSOR_ILLUMINATION_TASK