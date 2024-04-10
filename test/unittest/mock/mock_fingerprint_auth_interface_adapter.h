/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef MOCK_FINGERPRINT_AUTH_INTERFACE_ADAPTER
#define MOCK_FINGERPRINT_AUTH_INTERFACE_ADAPTER

#include "fingerprint_auth_hdi.h"
#include "fingerprint_auth_interface_adapter.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class MockFingerprintAuthInterfaceAdapter : public FingerprintAuthInterfaceAdapter {
public:
    MockFingerprintAuthInterfaceAdapter() = default;
    virtual ~MockFingerprintAuthInterfaceAdapter() = default;

    MOCK_METHOD0(Get, sptr<IFingerprintAuthInterface>());
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

#endif // MOCK_FINGERPRINT_AUTH_INTERFACE_ADAPTER
