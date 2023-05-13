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

#include "gtest/gtest.h"

#include "sensor_illumination_task.h"
#include "sensor_illumination_manager.h"

#include "iam_logger.h"
#include "iam_ptr.h"

#define LOG_LABEL UserIam::Common::LABEL_FINGERPRINT_AUTH_SA

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class FingerprintAuthSensorIllumination : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FingerprintAuthSensorIllumination::SetUpTestCase()
{
}

void FingerprintAuthSensorIllumination::TearDownTestCase()
{
}

void FingerprintAuthSensorIllumination::SetUp()
{
}

void FingerprintAuthSensorIllumination::TearDown()
{
}

HWTEST_F(FingerprintAuthSensorIllumination, FingerprintAuthSensorIllumination_001, TestSize.Level0)
{
    auto task = Common::MakeShared<SensorIlluminationTask>();
    IAM_LOGI("Begin EnableSensorIllumination");
    task->EnableSensorIllumination(500, 500, 200, 0);
    IAM_LOGI("End EnableSensorIllumination");
    sleep(2);

    IAM_LOGI("Begin TurnOnSensorIllumination 1");
    task->TurnOnSensorIllumination();
    IAM_LOGI("End TurnOnSensorIllumination 1");
    sleep(5);
    IAM_LOGI("Begin TurnOffSensorIllumination 1");
    task->TurnOffSensorIllumination();
    IAM_LOGI("End TurnOffSensorIllumination 1");
    sleep(2);

    IAM_LOGI("Begin TurnOnSensorIllumination 2");
    task->TurnOnSensorIllumination();
    IAM_LOGI("End TurnOnSensorIllumination 2");
    sleep(5);
    IAM_LOGI("Begin TurnOffSensorIllumination 2");
    task->TurnOffSensorIllumination();
    IAM_LOGI("End TurnOffSensorIllumination 2");
    sleep(2);

    IAM_LOGI("Begin DisableSensorIllumination");
    task->DisableSensorIllumination();
    IAM_LOGI("End DisableSensorIllumination");
}

HWTEST_F(FingerprintAuthSensorIllumination, FingerprintAuthSensorIllumination_002, TestSize.Level0)
{
    SaCommand enableCommand = {
        .id = SaCommandId::ENABLE_SENSOR_ILLUMINATION,
        .param = { .enableSensorIllumination = { 500, 500, 200, 0, 0 } }
    };
    SaCommand disableCommand = { SaCommandId::DISABLE_SENSOR_ILLUMINATION };
    SaCommand turnOnCommand = { SaCommandId::TURN_ON_SENSOR_ILLUMINATION };
    SaCommand turnOffCommand = { SaCommandId::TURN_OFF_SENSOR_ILLUMINATION };

    auto manager = SensorIlluminationManager::GetInstance();
    EXPECT_TRUE(manager != nullptr);
    auto executor = Common::MakeShared<FingerprintAuthExecutorHdi>(nullptr);
    EXPECT_TRUE(executor != nullptr);

    IAM_LOGI("Begin EnableSensorIllumination");
    manager->ProcessSaCommand(executor, enableCommand);
    IAM_LOGI("End EnableSensorIllumination");
    sleep(2);

    IAM_LOGI("Begin TurnOnSensorIllumination 1");
    manager->ProcessSaCommand(executor, turnOnCommand);
    IAM_LOGI("End TurnOnSensorIllumination 1");
    sleep(5);
    IAM_LOGI("Begin TurnOffSensorIllumination 1");
    manager->ProcessSaCommand(executor, turnOffCommand);
    IAM_LOGI("End TurnOffSensorIllumination 1");
    sleep(2);

    IAM_LOGI("Begin TurnOnSensorIllumination 2");
    manager->ProcessSaCommand(executor, turnOnCommand);
    IAM_LOGI("End TurnOnSensorIllumination 2");
    sleep(5);
    IAM_LOGI("Begin TurnOffSensorIllumination 2");
    manager->ProcessSaCommand(executor, turnOffCommand);;
    IAM_LOGI("End TurnOffSensorIllumination 2");
    sleep(2);

    IAM_LOGI("Begin DisableSensorIllumination");
    manager->ProcessSaCommand(executor, disableCommand);
    IAM_LOGI("End DisableSensorIllumination");
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
