/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "fingerprint_auth_service.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "iam_executor_idriver_manager.h"
#include "ipc_skeleton.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "refbase.h"
#include "system_ability.h"
#include "system_ability_definition.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_para2str.h"
#include "iam_ptr.h"

#include "fingerprint_auth_defines.h"
#include "fingerprint_auth_driver_hdi.h"
#include "fingerprint_auth_interface_adapter.h"
#include "sensor_illumination_manager.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
using namespace OHOS::UserIam;
namespace {
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(FingerprintAuthService::GetInstance().get());
const uint16_t FINGERPRINT_AUTH_DEFAULT_HDI_ID = 1;
const auto FINGERPRINT_AUTH_DEFAULT_HDI_ADAPTER = Common::MakeShared<FingerprintAuthInterfaceAdapter>();
auto FINGERPRINT_AUTH_DEFAULT_HDI = Common::MakeShared<FingerprintAuthDriverHdi>(FINGERPRINT_AUTH_DEFAULT_HDI_ADAPTER);
// serviceName and HdiConfig.id must be globally unique
const std::map<std::string, UserAuth::HdiConfig> HDI_NAME_2_CONFIG = {
    { "fingerprint_auth_interface_service", { FINGERPRINT_AUTH_DEFAULT_HDI_ID, FINGERPRINT_AUTH_DEFAULT_HDI } },
};
const std::vector<std::shared_ptr<FingerprintAuthDriverHdi>> FINGERPRINT_AUTH_DRIVER_HDIS = {
    FINGERPRINT_AUTH_DEFAULT_HDI
};
// setup brightness manager
const auto FINGER_SENSOR_ILLUMINATION_MANAGER = SensorIlluminationManager::GetInstance();
} // namespace
std::mutex FingerprintAuthService::mutex_;
std::shared_ptr<FingerprintAuthService> FingerprintAuthService::instance_ = nullptr;

FingerprintAuthService::FingerprintAuthService() : SystemAbility(SUBSYS_USERIAM_SYS_ABILITY_FINGERPRINTAUTH, true)
{
}

std::shared_ptr<FingerprintAuthService> FingerprintAuthService::GetInstance()
{
    if (instance_ == nullptr) {
        std::lock_guard<std::mutex> guard(mutex_);
        if (instance_ == nullptr) {
            instance_ = Common::MakeShared<FingerprintAuthService>();
            if (instance_ == nullptr) {
                IAM_LOGE("make share failed");
            }
        }
    }
    return instance_;
}

void FingerprintAuthService::OnStart()
{
    IAM_LOGI("start");
    StartDriverManager();
    IAM_LOGI("success");
}

void FingerprintAuthService::OnStop()
{
    IAM_LOGE("service is persistent, OnStop is not implemented");
    return;
}

void FingerprintAuthService::StartDriverManager()
{
    IAM_LOGI("start");
    int32_t ret = UserAuth::IDriverManager::Start(HDI_NAME_2_CONFIG);
    if (ret != UserAuth::ResultCode::SUCCESS) {
        IAM_LOGE("start driver manager failed");
    }
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
