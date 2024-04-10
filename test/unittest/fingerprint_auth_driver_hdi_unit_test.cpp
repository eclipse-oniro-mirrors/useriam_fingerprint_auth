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

#include "iam_logger.h"
#include "iam_ptr.h"

#include "fingerprint_auth_driver_hdi.h"
#include "mock_fingerprint_auth_interface_adapter.h"
#include "mock_iall_in_one_executor.h"
#include "mock_ifingerprint_auth_interface.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

using namespace testing;
using namespace testing::ext;
using namespace OHOS::UserIam::Common;

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
class FingerprintAuthDriverHdiUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void FingerprintAuthDriverHdiUnitTest::SetUpTestCase()
{
}

void FingerprintAuthDriverHdiUnitTest::TearDownTestCase()
{
}

void FingerprintAuthDriverHdiUnitTest::SetUp()
{
}

void FingerprintAuthDriverHdiUnitTest::TearDown()
{
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_001, TestSize.Level0)
{
    FingerprintAuthDriverHdi driverHdi(nullptr);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_002, TestSize.Level0)
{
    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(nullptr));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_003, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_)).Times(Exactly(1));

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_004, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) { return static_cast<int32_t>(HDF_FAILURE); });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_005, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) { return static_cast<int32_t>(HDF_SUCCESS); });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_006, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 1);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_007, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            list.push_back(sptr<IAllInOneExecutor>(nullptr));
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            list.push_back(sptr<IAllInOneExecutor>(nullptr));
            executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 2);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_GetExecutorListTest_008, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_FAILURE);
        });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 0);
}

HWTEST_F(FingerprintAuthDriverHdiUnitTest, FingerprintAuthDriverHdi_OnHdiDisconnectTest_001, TestSize.Level0)
{
    sptr<MockIFingerprintAuthInterface> interface(new (std::nothrow) MockIFingerprintAuthInterface());
    ASSERT_TRUE(interface != nullptr);
    EXPECT_CALL(*interface, GetExecutorList(_))
        .Times(Exactly(1))
        .WillOnce([](std::vector<sptr<IAllInOneExecutor>> &list) {
            auto executor = sptr<IAllInOneExecutor>(new (std::nothrow) MockIAllInOneExecutor());
            EXPECT_TRUE(executor != nullptr);
            list.push_back(executor);
            return static_cast<int32_t>(HDF_SUCCESS);
        });

    auto adapter = MakeShared<MockFingerprintAuthInterfaceAdapter>();
    ASSERT_TRUE(adapter != nullptr);
    EXPECT_CALL(*adapter, Get()).Times(Exactly(1)).WillOnce(Return(interface));

    FingerprintAuthDriverHdi driverHdi(adapter);
    std::vector<std::shared_ptr<UserAuth::IAuthExecutorHdi>> executorList;
    driverHdi.GetExecutorList(executorList);
    EXPECT_TRUE(executorList.size() == 1);
    driverHdi.OnHdiDisconnect();
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
