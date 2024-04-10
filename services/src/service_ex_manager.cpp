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

#include "service_ex_manager.h"

#include <dlfcn.h>

#include "iam_check.h"
#include "iam_logger.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace {
const char *SO_NAME = "libfingerprintauthservice_ex.z.so";
const char *GET_SENSOR_ILLUMINATION_TASK_SYMBOL_NAME = "_ZN4OHOS7UserIam15FingerprintAuth25GetSensorIlluminationTaskEv";
using GetSensorIlluminationTaskFunc = std::shared_ptr<ISensorIlluminationTask> (*)();
} // namespace

ServiceExManager &ServiceExManager::GetInstance()
{
    static ServiceExManager manager;
    return manager;
}

UserAuth::ResultCode ServiceExManager::Acquire()
{
    std::lock_guard<std::recursive_mutex> lock(recursiveMutex_);
    IAM_LOGI("start");
    if (!isOpened_) {
        handle_ = dlopen(SO_NAME, RTLD_LAZY);
        if (handle_ == nullptr) {
            IAM_LOGE("failed to open so, error: %{public}s", dlerror());
            return UserAuth::GENERAL_ERROR;
        }
        isOpened_ = true;
        IAM_LOGI("dlopen success");
    }

    IAM_LOGI("success");
    return UserAuth::SUCCESS;
}

void ServiceExManager::Release()
{
    std::lock_guard<std::recursive_mutex> lock(recursiveMutex_);
    IAM_LOGI("in this version, so is not released");
}

std::shared_ptr<ISensorIlluminationTask> ServiceExManager::GetSensorIlluminationTask()
{
    std::lock_guard<std::recursive_mutex> lock(recursiveMutex_);
    IF_FALSE_LOGE_AND_RETURN_VAL(handle_ != nullptr, nullptr);

    GetSensorIlluminationTaskFunc getFunc =
        reinterpret_cast<GetSensorIlluminationTaskFunc>(dlsym(handle_, GET_SENSOR_ILLUMINATION_TASK_SYMBOL_NAME));
    if (getFunc == nullptr) {
        IAM_LOGE("failed to find symbol, error: %{public}s", dlerror());
        return nullptr;
    }

    return getFunc();
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS