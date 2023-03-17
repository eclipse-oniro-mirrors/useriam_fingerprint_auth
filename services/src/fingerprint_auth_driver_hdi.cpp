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

#include <cstdint>

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"
#include "iam_executor_iauth_executor_hdi.h"

#include "fingerprint_auth_defines.h"
#include "fingerprint_auth_executor_hdi.h"
#include "v1_0/ifingerprint_auth_interface.h"

#define LOG_LABEL UserIam::Common::LABEL_FINGERPRINT_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
FingerprintAuthDriverHdi::FingerprintAuthDriverHdi(
    std::shared_ptr<FingerprintAuthInterfaceAdapter> fingerprintAuthInterfaceAdapter)
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

    std::vector<sptr<IExecutor>> iExecutorList;
    auto ret = fingerprintIf->GetExecutorList(iExecutorList);
    if (ret != HDF_SUCCESS) {
        IAM_LOGE("GetExecutorList fail");
        return;
    }
    for (const auto &iExecutor : iExecutorList) {
        if (iExecutor == nullptr) {
            IAM_LOGE("iExecutor is nullptr");
            continue;
        }
        auto executor = UserIam::Common::MakeShared<FingerprintAuthExecutorHdi>(iExecutor);
        if (executor == nullptr) {
            IAM_LOGE("make share failed");
            continue;
        }
        executorList.push_back(executor);
    }
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
