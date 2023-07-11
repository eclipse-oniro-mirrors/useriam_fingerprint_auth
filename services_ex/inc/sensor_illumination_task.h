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
#include "transaction/rs_interfaces.h"
#include "ui/rs_surface_node.h"

#include "iam_common_defines.h"

#include "isensor_illumination_task.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
using CanvasParam = struct {
    uint32_t centerXInPx;
    uint32_t centerYInPy;
    uint32_t radius;
    uint32_t color;
    uint32_t alpha;
    int32_t frameWidth;
    int32_t frameHeight;
};

class SensorIlluminationTask : public ISensorIlluminationTask,
                               public std::enable_shared_from_this<SensorIlluminationTask>,
                               public NoCopyable {
public:
    SensorIlluminationTask();
    ~SensorIlluminationTask() override;

    UserAuth::ResultCode EnableSensorIllumination(uint32_t centerX, uint32_t centerY, uint32_t radius,
        uint32_t color) override;
    UserAuth::ResultCode DisableSensorIllumination() override;
    UserAuth::ResultCode TurnOnSensorIllumination() override;
    UserAuth::ResultCode TurnOffSensorIllumination() override;
    void OnDisplayTimeOut() override;
    void RegisterDestructCallback(DestructCallback callback) override;

private:
    UserAuth::ResultCode DrawSurfaceNode();

    static constexpr uint32_t INVALID_BRIGHTNESS = UINT32_MAX;
    static constexpr int32_t INVALID_DISPLAY_ID = -1;

    std::shared_ptr<Rosen::RSSurfaceNode> rsSurfaceNode_ = nullptr;
    std::shared_ptr<Rosen::RSSurface> rsSurface_ = nullptr;
    std::shared_ptr<Rosen::RenderContext> renderContext_ = nullptr;
    Rosen::DisplayId defaultDisplayId_ = INVALID_DISPLAY_ID;
    Rosen::ScreenId defaultScreenId_ = Rosen::INVALID_SCREEN_ID;
    CanvasParam canvasParam_ = {};
    uint32_t brightness_ = INVALID_BRIGHTNESS;

    Utils::Timer timer_;
    uint32_t currTimerId_ = 0;
    std::recursive_mutex recursiveMutex_;
    bool isIlluminationOn_ = false;
    DestructCallback destructCallback_;
};
std::shared_ptr<ISensorIlluminationTask> GetSensorIlluminationTask();
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // SENSOR_ILLUMINATION_TASK