/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "hdf_base.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "fingerprint_auth_defines.h"
#include "fingerprint_auth_executor_hdi.h"
#include "mock_iexecute_callback.h"
#include "mock_iexecutor.h"

#define LOG_LABEL UserIam::Common::LABEL_FINGERPRINT_AUTH_SA

using namespace testing;
using namespace testing::ext;
using namespace OHOS::UserIam::Common;
namespace FingerprintHdi = OHOS::HDI::FingerprintAuth::V1_0;

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
using IamResultCode = OHOS::UserIam::UserAuth::ResultCode;
using IamExecutorRole = OHOS::UserIam::UserAuth::ExecutorRole;
using IamExecutorInfo = OHOS::UserIam::UserAuth::ExecutorInfo;
using IamAuthType = OHOS::UserIam::UserAuth::AuthType;
using IamExecutorSecureLevel = OHOS::UserIam::UserAuth::ExecutorSecureLevel;
using IamPropertyMode = OHOS::UserIam::UserAuth::PropertyMode;
namespace {
static const std::map<HDF_STATUS, IamResultCode> RESULT_CODE_MAP = {
    {HDF_SUCCESS, IamResultCode::SUCCESS},
    {HDF_FAILURE, IamResultCode::GENERAL_ERROR},
    {HDF_ERR_TIMEOUT, IamResultCode::TIMEOUT},
    {HDF_ERR_QUEUE_FULL, IamResultCode::BUSY},
    {HDF_ERR_DEVICE_BUSY, IamResultCode::BUSY},
    {static_cast<HDF_STATUS>(HDF_ERR_DEVICE_BUSY - 1), IamResultCode::GENERAL_ERROR},
    {static_cast<HDF_STATUS>(HDF_SUCCESS + 1), IamResultCode::GENERAL_ERROR},
};
}

class FingerprintAuthExecutorHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FingerprintAuthExecutorHdiUnitTest::SetUpTestCase()
{
}

void FingerprintAuthExecutorHdiUnitTest::TearDownTestCase()
{
}

void FingerprintAuthExecutorHdiUnitTest::SetUp()
{
}

void FingerprintAuthExecutorHdiUnitTest::TearDown()
{
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_001, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, GetExecutorInfo(_)).Times(Exactly(1)).WillOnce([](FingerprintHdi::ExecutorInfo &info) {
        info = {
            .executorRole = FingerprintHdi::ExecutorRole::ALL_IN_ONE,
            .authType = FingerprintHdi::FINGERPRINT,
            .esl = FingerprintHdi::ExecutorSecureLevel::ESL0,
        };
        return HDF_SUCCESS;
    });
    FingerprintAuthExecutorHdi executorHdi(executorProxy);
    IamExecutorInfo info = {};
    auto ret = executorHdi.GetExecutorInfo(info);
    EXPECT_TRUE(info.executorRole == IamExecutorRole::ALL_IN_ONE);
    EXPECT_TRUE(info.authType == IamAuthType::FINGERPRINT);
    EXPECT_TRUE(info.esl == IamExecutorSecureLevel::ESL0);
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FingerprintHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FingerprintHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = FingerprintHdi::FINGERPRINT,
                    .esl = FingerprintHdi::ExecutorSecureLevel::ESL0,
                };
                return static_cast<int32_t>(pair.first);
            });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_003, TestSize.Level0)
{
    static const std::map<FingerprintHdi::AuthType, pair<IamAuthType, IamResultCode>> data = {
        {FingerprintHdi::FINGERPRINT, {IamAuthType::FINGERPRINT, IamResultCode::SUCCESS}},
        {static_cast<FingerprintHdi::AuthType>(FingerprintHdi::FINGERPRINT + 1),
            {IamAuthType::FINGERPRINT, IamResultCode::GENERAL_ERROR}},
        {static_cast<FingerprintHdi::AuthType>(FingerprintHdi::FINGERPRINT - 1),
            {IamAuthType::FINGERPRINT, IamResultCode::GENERAL_ERROR}},
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FingerprintHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FingerprintHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = pair.first,
                    .esl = FingerprintHdi::ExecutorSecureLevel::ESL0,
                };
                return HDF_SUCCESS;
            });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.authType == pair.second.first);
        }
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_004, TestSize.Level0)
{
    static const std::map<FingerprintHdi::ExecutorRole, pair<IamExecutorRole, IamResultCode>> data = {
        {FingerprintHdi::ExecutorRole::COLLECTOR, {IamExecutorRole::COLLECTOR, IamResultCode::SUCCESS}},
        {FingerprintHdi::ExecutorRole::VERIFIER, {IamExecutorRole::VERIFIER, IamResultCode::SUCCESS}},
        {FingerprintHdi::ExecutorRole::ALL_IN_ONE, {IamExecutorRole::ALL_IN_ONE, IamResultCode::SUCCESS}},
        {static_cast<FingerprintHdi::ExecutorRole>(FingerprintHdi::ExecutorRole::COLLECTOR - 1),
            {IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR}},
        {static_cast<FingerprintHdi::ExecutorRole>(FingerprintHdi::ExecutorRole::ALL_IN_ONE + 1),
            {IamExecutorRole::ALL_IN_ONE, IamResultCode::GENERAL_ERROR}},
    };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FingerprintHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = pair.first,
                    .authType = FingerprintHdi::FINGERPRINT,
                    .esl = FingerprintHdi::ExecutorSecureLevel::ESL0,
                };
                return HDF_SUCCESS;
            });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.executorRole == pair.second.first);
        }
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_005, TestSize.Level0)
{
    static const std::map<FingerprintHdi::ExecutorSecureLevel,
        pair<IamExecutorSecureLevel, IamResultCode>> data =
        {
            {FingerprintHdi::ExecutorSecureLevel::ESL0,
                {IamExecutorSecureLevel::ESL0, IamResultCode::SUCCESS}},
            {FingerprintHdi::ExecutorSecureLevel::ESL1,
                {IamExecutorSecureLevel::ESL1, IamResultCode::SUCCESS}},
            {FingerprintHdi::ExecutorSecureLevel::ESL2,
                {IamExecutorSecureLevel::ESL2, IamResultCode::SUCCESS}},
            {FingerprintHdi::ExecutorSecureLevel::ESL3,
                {IamExecutorSecureLevel::ESL3, IamResultCode::SUCCESS}},
            {static_cast<FingerprintHdi::ExecutorSecureLevel>(FingerprintHdi::ExecutorSecureLevel::ESL0 - 1),
                {IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR}},
            {static_cast<FingerprintHdi::ExecutorSecureLevel>(FingerprintHdi::ExecutorSecureLevel::ESL3 + 1),
                {IamExecutorSecureLevel::ESL3, IamResultCode::GENERAL_ERROR}},
        };
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetExecutorInfo(_))
            .Times(Exactly(1))
            .WillOnce([&pair](FingerprintHdi::ExecutorInfo &info) {
                info = {
                    .executorRole = FingerprintHdi::ExecutorRole::ALL_IN_ONE,
                    .authType = FingerprintHdi::FINGERPRINT,
                    .esl = pair.first,
                };
                return HDF_SUCCESS;
            });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        IamExecutorInfo info = {};
        auto ret = executorHdi.GetExecutorInfo(info);
        EXPECT_TRUE(ret == pair.second.second);
        if (ret == IamResultCode::SUCCESS) {
            EXPECT_TRUE(info.esl == pair.second.first);
        }
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetExecutorInfo_006, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    IamExecutorInfo info = {};
    auto ret = executorHdi.GetExecutorInfo(info);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetTemplateInfo_001, TestSize.Level0)
{
    const UserAuth::TemplateInfo data = {.executorType = 1,
        .freezingTime = 2,
        .remainTimes = 3,
        .extraInfo = {4, 5, 6}};
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, GetTemplateInfo(_, _))
        .Times(Exactly(1))
        .WillOnce([&data](uint64_t templateId, FingerprintHdi::TemplateInfo &info) {
            info = {.executorType = data.executorType,
                .lockoutDuration = data.freezingTime,
                .remainAttempts = data.remainTimes,
                .extraInfo = data.extraInfo};
            return HDF_SUCCESS;
        });
    FingerprintAuthExecutorHdi executorHdi(executorProxy);
    UserAuth::TemplateInfo info = {};
    auto ret = executorHdi.GetTemplateInfo(0, info);
    EXPECT_TRUE(ret == IamResultCode::SUCCESS);
    EXPECT_TRUE(info.executorType == data.executorType);
    EXPECT_TRUE(info.freezingTime == data.freezingTime);
    EXPECT_TRUE(info.remainTimes == data.remainTimes);
    EXPECT_TRUE(info.extraInfo == data.extraInfo);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetTemplateInfo_002, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, GetTemplateInfo(_, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t templateId, FingerprintHdi::TemplateInfo &info) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        UserAuth::TemplateInfo info = {};
        auto ret = executorHdi.GetTemplateInfo(0, info);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_GetTemplateInfo_003, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    UserAuth::TemplateInfo info = {};
    auto ret = executorHdi.GetTemplateInfo(0, info);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_OnRegisterFinish_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, OnRegisterFinish(_, _, _))
            .Times(Exactly(1))
            .WillOnce(
                [&pair](const std::vector<uint64_t> &templateIdList, const std::vector<uint8_t> &frameworkPublicKey,
                    const std::vector<uint8_t> &extraInfo) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        UserAuth::TemplateInfo info = {};
        auto ret =
            executorHdi.OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_OnRegisterFinish_002, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.OnRegisterFinish(std::vector<uint64_t>(), std::vector<uint8_t>(), std::vector<uint8_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Enroll_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Enroll(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FingerprintHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Enroll_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Enroll(_, _, _)).Times(Exactly(0));
    FingerprintAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Enroll_003, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Enroll(0, 0, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Authenticate_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint64_t> &templateIdList,
                          const std::vector<uint8_t> &extraInfo,
                          const sptr<FingerprintHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Authenticate_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Authenticate(_, _, _, _)).Times(Exactly(0));
    FingerprintAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Authenticate_003, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Authenticate(0, 0, std::vector<uint64_t>(), std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}
HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Identify_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Identify(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FingerprintHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Identify_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, Identify(_, _, _)).Times(Exactly(0));
    FingerprintAuthExecutorHdi executorHdi(executorProxy);
    auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Identify_003, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret = executorHdi.Identify(0, 0, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Delete_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Delete(_))
            .Times(Exactly(1))
            .WillOnce([&pair](const std::vector<uint64_t> &templateIdList) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto ret = executorHdi.Delete(std::vector<uint64_t>());
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Delete_002, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.Delete(std::vector<uint64_t>());
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Cancel_001, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, Cancel(_)).Times(Exactly(1)).WillOnce([&pair](uint64_t scheduleId) {
            return pair.first;
        });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto ret = executorHdi.Cancel(0);
        EXPECT_TRUE(ret == pair.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_Cancel_002, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto ret = executorHdi.Cancel(0);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_SendCommand_001, TestSize.Level0)
{
    FingerprintAuthExecutorHdi executorHdi(nullptr);
    auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
    ASSERT_TRUE(executeCallback != nullptr);
    auto ret =
        executorHdi.SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_SendCommand_002, TestSize.Level0)
{
    auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
    ASSERT_TRUE(executorProxy != nullptr);
    EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
    FingerprintAuthExecutorHdi executorHdi(executorProxy);

    auto ret = executorHdi.SendCommand(IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), nullptr);
    EXPECT_TRUE(ret == IamResultCode::GENERAL_ERROR);
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_SendCommand_003, TestSize.Level0)
{
    static const std::map<IamPropertyMode, pair<FingerprintHdi::CommandId, IamResultCode>> data = {
        {IamPropertyMode::PROPERTY_MODE_FREEZE,
            {FingerprintHdi::CommandId::LOCK_TEMPLATE, IamResultCode::SUCCESS}},
        {IamPropertyMode::PROPERTY_MODE_UNFREEZE,
            {FingerprintHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::SUCCESS}},
        {static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_FREEZE - 1),
            {FingerprintHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(IamPropertyMode::PROPERTY_MODE_UNFREEZE + 1),
            {FingerprintHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(FingerprintHdi::VENDOR_COMMAND_BEGIN),
            {FingerprintHdi::CommandId::UNLOCK_TEMPLATE, IamResultCode::INVALID_PARAMETERS}},
        {static_cast<IamPropertyMode>(FingerprintHdi::VENDOR_COMMAND_BEGIN + 1),
            {static_cast<FingerprintHdi::CommandId>(FingerprintHdi::VENDOR_COMMAND_BEGIN + 1),
            IamResultCode::SUCCESS}}};
    for (const auto &pair : data) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        if (pair.second.second == IamResultCode::SUCCESS) {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
                .Times(Exactly(1))
                .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                              const sptr<FingerprintHdi::IExecutorCallback> &callbackObj) {
                    EXPECT_TRUE(commandId == pair.second.first);
                    return HDF_SUCCESS;
                });
        } else {
            EXPECT_CALL(*executorProxy, SendCommand(_, _, _)).Times(Exactly(0));
        }
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.SendCommand(pair.first, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second.second);
    }
}

HWTEST_F(FingerprintAuthExecutorHdiUnitTest, FingerprintAuthExecutorHdi_SendCommand_004, TestSize.Level0)
{
    for (const auto &pair : RESULT_CODE_MAP) {
        auto executorProxy = new (std::nothrow) FingerprintHdi::MockIExecutor();
        ASSERT_TRUE(executorProxy != nullptr);
        EXPECT_CALL(*executorProxy, SendCommand(_, _, _))
            .Times(Exactly(1))
            .WillOnce([&pair](int32_t commandId, const std::vector<uint8_t> &extraInfo,
                          const sptr<FingerprintHdi::IExecutorCallback> &callbackObj) { return pair.first; });
        FingerprintAuthExecutorHdi executorHdi(executorProxy);
        auto executeCallback = UserIam::Common::MakeShared<UserIam::UserAuth::MockIExecuteCallback>();
        ASSERT_TRUE(executeCallback != nullptr);
        auto ret = executorHdi.SendCommand(
            IamPropertyMode::PROPERTY_MODE_FREEZE, std::vector<uint8_t>(), executeCallback);
        EXPECT_TRUE(ret == pair.second);
    }
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
