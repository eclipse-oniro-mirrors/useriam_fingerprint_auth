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

#include "fingerprint_auth_executor_callback_hdi.h"

#include <map>

#include "iam_check.h"
#include "iam_logger.h"
#include "fingerprint_auth_defines.h"
#include "v1_0/fingerprint_auth_types.h"

#define LOG_LABEL UserIam::Common::LABEL_FINGERPRINT_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace UserAuth = OHOS::UserIam::UserAuth;
using IamResultCode = OHOS::UserIam::UserAuth::ResultCode;
FingerprintAuthExecutorCallbackHdi::FingerprintAuthExecutorCallbackHdi(
    std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback)
    : frameworkCallback_(frameworkCallback)
{
}

int32_t FingerprintAuthExecutorCallbackHdi::OnResult(int32_t result, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnResult %{public}d", result);
    IamResultCode retCode = ConvertResultCode(result);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    frameworkCallback_->OnResult(retCode, extraInfo);
    return HDF_SUCCESS;
}

int32_t FingerprintAuthExecutorCallbackHdi::OnTip(int32_t tip, const std::vector<uint8_t> &extraInfo)
{
    IAM_LOGI("OnTip %{public}d", tip);
    IF_FALSE_LOGE_AND_RETURN_VAL(frameworkCallback_ != nullptr, HDF_FAILURE);
    frameworkCallback_->OnAcquireInfo(tip, extraInfo);
    return HDF_SUCCESS;
}

IamResultCode FingerprintAuthExecutorCallbackHdi::ConvertResultCode(const int32_t in)
{
    ResultCode hdiIn = static_cast<ResultCode>(in);
    if (hdiIn > ResultCode::VENDOR_RESULT_CODE_BEGIN) {
        IAM_LOGI("vendor hdi result code %{public}d, no covert", hdiIn);
        return static_cast<IamResultCode>(in);
    }

    static const std::map<ResultCode, IamResultCode> data = {
        {ResultCode::SUCCESS, IamResultCode::SUCCESS},
        {ResultCode::FAIL, IamResultCode::FAIL},
        {ResultCode::GENERAL_ERROR, IamResultCode::GENERAL_ERROR},
        {ResultCode::CANCELED, IamResultCode::CANCELED},
        {ResultCode::TIMEOUT, IamResultCode::TIMEOUT},
        {ResultCode::BUSY, IamResultCode::BUSY},
        {ResultCode::INVALID_PARAMETERS, IamResultCode::INVALID_PARAMETERS},
        {ResultCode::LOCKED, IamResultCode::LOCKED},
        {ResultCode::NOT_ENROLLED, IamResultCode::NOT_ENROLLED},
        // should be IamResultCode::OPERATION_NOT_SUPPORT
        {ResultCode::OPERATION_NOT_SUPPORT, IamResultCode::FAIL},
    };

    IamResultCode out;
    auto iter = data.find(hdiIn);
    if (iter == data.end()) {
        out = IamResultCode::GENERAL_ERROR;
        IAM_LOGE("convert hdi undefined result code %{public}d to framework result code %{public}d", in, out);
        return out;
    }
    out = iter->second;
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", hdiIn, out);
    return out;
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
