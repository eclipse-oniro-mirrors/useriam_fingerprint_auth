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

#include "gtest/gtest.h"

#include "screen_state_monitor.h"

#include "iam_logger.h"
#include "iam_ptr.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class ScreenStateMonitorUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ScreenStateMonitorUnitTest::SetUpTestCase()
{
}

void ScreenStateMonitorUnitTest::TearDownTestCase()
{
}

void ScreenStateMonitorUnitTest::SetUp()
{
}

void ScreenStateMonitorUnitTest::TearDown()
{
}

HWTEST_F(ScreenStateMonitorUnitTest, ScreenStateMonitorUnitTest_001, TestSize.Level0)
{
    IAM_LOGI("begin ScreenStateMonitorUnitTest_001");
    ScreenStateMonitor::GetInstance().Subscribe();
    ScreenStateMonitor::GetInstance().Unsubscribe();
    ScreenStateMonitor::GetInstance().SetScreenOn(false);
    EXPECT_FALSE(ScreenStateMonitor::GetInstance().GetScreenOn());
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS