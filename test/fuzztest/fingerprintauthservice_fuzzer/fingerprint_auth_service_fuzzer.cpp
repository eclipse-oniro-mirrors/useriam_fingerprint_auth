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

#include "fingerprint_auth_service_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "parcel.h"

#include "iam_logger.h"

#include "fingerprint_auth_service.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

#undef private

using namespace std;

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
namespace {
auto g_service = FingerprintAuthService::GetInstance();

void FuzzOnStart(Parcel &parcel)
{
    IAM_LOGI("begin");
    if (g_service != nullptr) {
        g_service->OnStart();
    }
    IAM_LOGI("end");
}

void FuzzOnStop(Parcel &parcel)
{
    IAM_LOGI("begin");
    if (g_service != nullptr) {
        g_service->OnStop();
    }
    IAM_LOGI("end");
}

using FuzzFunc = decltype(FuzzOnStart);
FuzzFunc *g_fuzzFuncs[] = { FuzzOnStart, FuzzOnStop };

void FingerprintAuthServiceFuzzTest(const uint8_t *data, size_t size)
{
    Parcel parcel;
    parcel.WriteBuffer(data, size);
    parcel.RewindRead(0);
    uint32_t index = parcel.ReadUint32() % (sizeof(g_fuzzFuncs) / sizeof(FuzzFunc *));
    auto fuzzFunc = g_fuzzFuncs[index];
    fuzzFunc(parcel);
    return;
}
} // namespace
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int32_t LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::UserIam::FingerprintAuth::FingerprintAuthServiceFuzzTest(data, size);
    return 0;
}
