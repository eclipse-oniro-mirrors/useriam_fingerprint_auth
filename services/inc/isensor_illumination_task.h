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

#ifndef ISENSOR_ILLUMINATION_TASK
#define ISENSOR_ILLUMINATION_TASK

#include <mutex>

#include "iam_common_defines.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class ISensorIlluminationTask {
public:
    ISensorIlluminationTask() = default;
    virtual ~ISensorIlluminationTask() = default;

    using DestructCallback = std::function<void(void)>;

    virtual UserAuth::ResultCode EnableSensorIllumination(uint32_t centerX, uint32_t centerY, uint32_t radius,
        uint32_t color) = 0;
    virtual UserAuth::ResultCode DisableSensorIllumination() = 0;
    virtual UserAuth::ResultCode TurnOnSensorIllumination() = 0;
    virtual UserAuth::ResultCode TurnOffSensorIllumination() = 0;
    virtual void OnDisplayTimeOut() = 0;
    virtual void RegisterDestructCallback(DestructCallback callback) = 0;
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // ISENSOR_ILLUMINATION_TASK