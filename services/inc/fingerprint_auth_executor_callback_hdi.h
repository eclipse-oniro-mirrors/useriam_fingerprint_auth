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

#ifndef FINGERPRINT_AUTH_EXECUTOR_CALLBACK_HDI
#define FINGERPRINT_AUTH_EXECUTOR_CALLBACK_HDI

#include <cstdint>

#include "hdf_base.h"
#include "nocopyable.h"

#include "iam_executor_iexecute_callback.h"
#include "v1_0/iexecutor_callback.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace FingerprintHdi = OHOS::HDI::FingerprintAuth::V1_0;
namespace UserAuth = OHOS::UserIam::UserAuth;
class FingerprintAuthExecutorCallbackHdi : public FingerprintHdi::IExecutorCallback, public NoCopyable {
public:
    explicit FingerprintAuthExecutorCallbackHdi(std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback);
    ~FingerprintAuthExecutorCallbackHdi() override = default;

    // FingerprintHdi::IExecutorCallback
    int32_t OnResult(int32_t result, const std::vector<uint8_t> &extraInfo) override;
    int32_t OnTip(int32_t tip, const std::vector<uint8_t> &extraInfo) override;

private:
    UserIam::UserAuth::ResultCode ConvertResultCode(const int32_t in);

    std::shared_ptr<UserAuth::IExecuteCallback> frameworkCallback_;
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // FINGERPRINT_AUTH_EXECUTOR_CALLBACK_HDI
