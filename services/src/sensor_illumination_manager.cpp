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

#include "sensor_illumination_manager.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "sa_command_manager.h"
#include "service_ex_manager.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace {
constexpr uint32_t MAX_RADIUS_LEN = 300;
constexpr uint32_t MAX_X_Y_VALUE = 1000; // center X and Y in per thousand
std::shared_ptr<SensorIlluminationManager> CreateSensorIlluminationManager()
{
    auto manager = Common::MakeShared<SensorIlluminationManager>();
    IF_FALSE_LOGE_AND_RETURN_VAL(manager != nullptr, nullptr);

    std::vector<SaCommandId> commandIds = { SaCommandId::ENABLE_SENSOR_ILLUMINATION,
        SaCommandId::DISABLE_SENSOR_ILLUMINATION, SaCommandId::TURN_ON_SENSOR_ILLUMINATION,
        SaCommandId::TURN_OFF_SENSOR_ILLUMINATION };
    SaCommandManager::GetInstance().RegisterSaCommandProcessor(commandIds, manager);
    return manager;
}
} // namespace

std::shared_ptr<SensorIlluminationManager> SensorIlluminationManager::GetInstance()
{
    static auto manager = CreateSensorIlluminationManager();
    if (manager == nullptr) {
        IAM_LOGE("SensorIlluminationManager is null");
    }
    return manager;
}

UserAuth::ResultCode SensorIlluminationManager::ProcessSaCommand(
    std::shared_ptr<FingerprintAllInOneExecutorHdi> executor, const SaCommand &command)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executor != nullptr, UserAuth::GENERAL_ERROR);
    std::lock_guard<std::mutex> lock(mutex_);

    UserAuth::ResultCode result = UserAuth::GENERAL_ERROR;
    switch (command.id) {
        case SaCommandId::ENABLE_SENSOR_ILLUMINATION:
            result = EnableSensorIllumination(executor, command.param);
            break;
        case SaCommandId::DISABLE_SENSOR_ILLUMINATION:
            result = DisableSensorIllumination(executor, command.param);
            break;
        case SaCommandId::TURN_ON_SENSOR_ILLUMINATION:
            result = TurnOnSensorIllumination(executor, command.param);
            break;
        case SaCommandId::TURN_OFF_SENSOR_ILLUMINATION:
            result = TurnOffSensorIllumination(executor, command.param);
            break;
        default:
            IAM_LOGE("command id %{public}d not match", command.id);
    }
    return result;
}

void SensorIlluminationManager::OnHdiDisconnect(std::shared_ptr<FingerprintAllInOneExecutorHdi> executor)
{
    IF_FALSE_LOGE_AND_RETURN(executor != nullptr);

    std::lock_guard<std::mutex> lock(mutex_);

    if (executorInProc_ != executor) {
        return;
    }

    IAM_LOGI("start");
    executorInProc_ = nullptr;

    if (taskInProc_ != nullptr) {
        UserAuth::ResultCode disableResultCode = taskInProc_->DisableSensorIllumination();
        if (disableResultCode != UserAuth::ResultCode::SUCCESS) {
            IAM_LOGE("turn off sensor illumination fail");
        }
        taskInProc_ = nullptr;
    }
    IAM_LOGI("success");
}

UserAuth::ResultCode SensorIlluminationManager::EnableSensorIllumination(
    std::shared_ptr<FingerprintAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != nullptr) {
        IAM_LOGE("another executor is using this module");
        return UserAuth::GENERAL_ERROR;
    }

    if (taskInProc_ != nullptr) {
        IAM_LOGE("another task is running");
        return UserAuth::GENERAL_ERROR;
    }

    IAM_LOGI("start");
    auto acquireRet = ServiceExManager::GetInstance().Acquire();
    IF_FALSE_LOGE_AND_RETURN_VAL(acquireRet == UserAuth::SUCCESS, UserAuth::GENERAL_ERROR);

    auto taskInProc = ServiceExManager::GetInstance().GetSensorIlluminationTask();
    if (taskInProc == nullptr) {
        ServiceExManager::GetInstance().Release();
        IAM_LOGE("failed to get task");
        return UserAuth::GENERAL_ERROR;
    }
    taskInProc->RegisterDestructCallback([]() {
        IAM_LOGI("task destruct");
        ServiceExManager::GetInstance().Release();
    });
    IF_FALSE_LOGE_AND_RETURN_VAL(param.enableSensorIllumination.centerX < MAX_X_Y_VALUE, UserAuth::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(param.enableSensorIllumination.centerY < MAX_X_Y_VALUE, UserAuth::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(param.enableSensorIllumination.radius < MAX_RADIUS_LEN, UserAuth::GENERAL_ERROR);

    UserAuth::ResultCode enableResultCode = taskInProc->EnableSensorIllumination(param.enableSensorIllumination.centerX,
        param.enableSensorIllumination.centerY, param.enableSensorIllumination.radius,
        param.enableSensorIllumination.color);
    IF_FALSE_LOGE_AND_RETURN_VAL(enableResultCode == UserAuth::ResultCode::SUCCESS, UserAuth::GENERAL_ERROR);

    executorInProc_ = executor;
    taskInProc_ = taskInProc;
    IAM_LOGI("success");
    return UserAuth::SUCCESS;
}

UserAuth::ResultCode SensorIlluminationManager::DisableSensorIllumination(
    std::shared_ptr<FingerprintAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != executor) {
        IAM_LOGE("illumination is not enabled for this executor");
        return UserAuth::GENERAL_ERROR;
    }

    IAM_LOGI("start");
    executorInProc_ = nullptr;

    if (taskInProc_ != nullptr) {
        UserAuth::ResultCode disableResultCode = taskInProc_->DisableSensorIllumination();
        if (disableResultCode != UserAuth::ResultCode::SUCCESS) {
            IAM_LOGE("turn off sensor illumination fail");
        }
        taskInProc_ = nullptr;
    }
    IAM_LOGI("success");
    return UserAuth::SUCCESS;
}

UserAuth::ResultCode SensorIlluminationManager::TurnOnSensorIllumination(
    std::shared_ptr<FingerprintAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != executor) {
        IAM_LOGE("illumination is not enabled for this executor");
        return UserAuth::GENERAL_ERROR;
    }

    if (taskInProc_ == nullptr) {
        IAM_LOGE("no task is running");
        return UserAuth::GENERAL_ERROR;
    }

    UserAuth::ResultCode turnOnResultCode = taskInProc_->TurnOnSensorIllumination();
    IF_FALSE_LOGE_AND_RETURN_VAL(turnOnResultCode == UserAuth::ResultCode::SUCCESS, UserAuth::GENERAL_ERROR);

    return UserAuth::SUCCESS;
}

UserAuth::ResultCode SensorIlluminationManager::TurnOffSensorIllumination(
    std::shared_ptr<FingerprintAllInOneExecutorHdi> executor, const SaCommandParam param)
{
    if (executorInProc_ != executor) {
        IAM_LOGE("illumination is not enabled for this executor");
        return UserAuth::GENERAL_ERROR;
    }

    if (taskInProc_ == nullptr) {
        IAM_LOGE("no task is running");
        return UserAuth::GENERAL_ERROR;
    }

    UserAuth::ResultCode turnOffResultCode = taskInProc_->TurnOffSensorIllumination();
    IF_FALSE_LOGE_AND_RETURN_VAL(turnOffResultCode == UserAuth::ResultCode::SUCCESS, UserAuth::GENERAL_ERROR);

    return UserAuth::SUCCESS;
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS