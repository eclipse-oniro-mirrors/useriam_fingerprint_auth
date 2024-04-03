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

#ifndef SENSOR_ILLUMINATION_MANAGER
#define SENSOR_ILLUMINATION_MANAGER

#include <map>
#include <mutex>

#include "nocopyable.h"

#include "fingerprint_auth_all_in_one_executor_hdi.h"
#include "isa_command_processor.h"
#include "isensor_illumination_task.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
class SensorIlluminationManager : public std::enable_shared_from_this<SensorIlluminationManager>,
                                  public ISaCommandProcessor,
                                  public NoCopyable {
public:
    SensorIlluminationManager() {};
    ~SensorIlluminationManager() = default;
    static std::shared_ptr<SensorIlluminationManager> GetInstance();

    UserAuth::ResultCode ProcessSaCommand(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor,
        const SaCommand &command) override;
    void OnHdiDisconnect(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor) override;

private:
    UserAuth::ResultCode EnableSensorIllumination(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor,
        const SaCommandParam param);
    UserAuth::ResultCode DisableSensorIllumination(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor,
        const SaCommandParam param);
    UserAuth::ResultCode TurnOnSensorIllumination(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor,
        const SaCommandParam param);
    UserAuth::ResultCode TurnOffSensorIllumination(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor,
        const SaCommandParam param);

    std::shared_ptr<FingerprintAllInOneExecutorHdi> executorInProc_ = nullptr;
    std::shared_ptr<ISensorIlluminationTask> taskInProc_ = nullptr;
    std::mutex mutex_;
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // SENSOR_ILLUMINATION_MANAGER