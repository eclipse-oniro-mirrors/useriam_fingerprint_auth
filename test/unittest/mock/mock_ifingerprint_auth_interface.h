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

#ifndef MOCK_IFINGERPRINT_AUTH_INTERFACE_H
#define MOCK_IFINGERPRINT_AUTH_INTERFACE_H

#include "gmock/gmock.h"

#include "fingerprint_auth_hdi.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class MockIFingerprintAuthInterface : public IFingerprintAuthInterface {
public:
    virtual ~MockIFingerprintAuthInterface() = default;

    MOCK_METHOD1(GetExecutorList, int32_t(std::vector<sptr<IAllInOneExecutor>> &executorList));
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_IFINGERPRINT_AUTH_INTERFACE_MOCK_H