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

#include "fingerprint_auth_driver_hdi.h"

#include <memory>
#include <vector>

#include "hdf_base.h"
#include "refbase.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "fingerprint_auth_all_in_one_executor_hdi.h"
#include "fingerprint_auth_defines.h"
#include "fingerprint_auth_hdi.h"
#include "fingerprint_auth_interface_adapter.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
std::mutex FingerprintAuthDriverHdi::mutex_;

FingerprintAuthDriverHdi::FingerprintAuthDriverHdi(
    const std::shared_ptr<FingerprintAuthInterfaceAdapter> fingerprintAuthInterfaceAdapter)
    : fingerprintAuthInterfaceAdapter_(fingerprintAuthInterfaceAdapter)
{
}

void FingerprintAuthDriverHdi::GetExecutorList(std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> &executorList)
{
    IF_FALSE_LOGE_AND_RETURN(fingerprintAuthInterfaceAdapter_ != nullptr);
    auto fingerprintIf = fingerprintAuthInterfaceAdapter_->Get();
    if (fingerprintIf == nullptr) {
        IAM_LOGE("IFingerprintAuthInterface is null");
        return;
    }

    std::vector<sptr<IAllInOneExecutor>> iExecutorList;
    auto ret = fingerprintIf->GetExecutorList(iExecutorList);
    if (ret != HDF_SUCCESS) {
        IAM_LOGE("GetExecutorList fail");
        return;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    fingerprintAuthExecutorList_.clear();
    for (const auto &iExecutor : iExecutorList) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        auto executor = Common::MakeShared<FingerprintAllInOneExecutorHdi>(iExecutor);
        if (executor == nullptr) {
            IAM_LOGE("make share failed");
            continue;
        }
        executorList.push_back(executor);
        fingerprintAuthExecutorList_.push_back(executor);
    }
}

void FingerprintAuthDriverHdi::OnHdiDisconnect()
{
    IAM_LOGI("start");
    std::lock_guard<std::mutex> guard(mutex_);
    for (const auto &iExecutor : fingerprintAuthExecutorList_) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        iExecutor->OnHdiDisconnect();
    }
    fingerprintAuthExecutorList_.clear();
    return;
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
