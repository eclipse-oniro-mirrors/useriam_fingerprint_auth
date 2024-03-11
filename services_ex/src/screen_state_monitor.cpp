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

#include "screen_state_monitor.h"

#include "common_event_manager.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "iservice_registry.h"
#include "power_mgr_client.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"
#include "want.h"

#include "iam_check.h"
#include "iam_logger.h"
#include "iam_ptr.h"

#include "sa_command_manager.h"

#define LOG_TAG "FINGERPRINT_AUTH_SA"

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
using namespace EventFwk;
using namespace AAFwk;
using namespace PowerMgr;
using ResultCode = UserAuth::ResultCode;

namespace {
const CommonEventSubscribeInfo GetCommonEventSubscribeInfo()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    return CommonEventSubscribeInfo(matchingSkills);
}
} // namespace

class EventSubscriber : public CommonEventSubscriber, public NoCopyable {
public:
    explicit EventSubscriber(const CommonEventSubscribeInfo &subscribeInfo) : CommonEventSubscriber(subscribeInfo) {};
    ~EventSubscriber() override {};

    static std::shared_ptr<EventSubscriber> GetInstance();
    static ResultCode Subscribe();
    static void Unsubscribe();

    void OnReceiveEvent(const CommonEventData &eventData) override;
};

std::shared_ptr<EventSubscriber> EventSubscriber::GetInstance()
{
    static auto subscriber = Common::MakeShared<EventSubscriber>(GetCommonEventSubscribeInfo());
    if (subscriber == nullptr) {
        IAM_LOGE("EventSubscriber is null");
    }
    return subscriber;
}

ResultCode EventSubscriber::Subscribe()
{
    EventSubscriber::Unsubscribe();
    auto instance = GetInstance();
    IF_FALSE_LOGE_AND_RETURN_VAL(instance != NULL, ResultCode::GENERAL_ERROR);

    int32_t ret = CommonEventManager::NewSubscribeCommonEvent(instance);
    if (ret != ERR_OK) {
        IAM_LOGE("subscribe fail, ret = %{public}d", ret);
        return ResultCode::GENERAL_ERROR;
    }
    IAM_LOGI("subscribe success");
    return ResultCode::SUCCESS;
}

void EventSubscriber::Unsubscribe()
{
    auto instance = GetInstance();
    IF_FALSE_LOGE_AND_RETURN(instance != NULL);

    int32_t ret = CommonEventManager::NewUnSubscribeCommonEvent(instance);
    if (ret != ERR_OK) {
        IAM_LOGE("unsubscribe fail, ret = %{public}d", ret);
        return;
    }
    IAM_LOGI("unsubscribe success");
}

void EventSubscriber::OnReceiveEvent(const CommonEventData &eventData)
{
    const Want &want = eventData.GetWant();
    std::string action = want.GetAction();
    if (action != CommonEventSupport::COMMON_EVENT_SCREEN_ON && action != CommonEventSupport::COMMON_EVENT_SCREEN_OFF) {
        return;
    }
    IAM_LOGI("receive event %{public}s", action.c_str());
    if (action == CommonEventSupport::COMMON_EVENT_SCREEN_ON) {
        ScreenStateMonitor::GetInstance().SetScreenOn(true);
    } else {
        ScreenStateMonitor::GetInstance().SetScreenOn(false);
    }
}

ScreenStateMonitor &ScreenStateMonitor::GetInstance()
{
    static ScreenStateMonitor monitor;
    return monitor;
}

void ScreenStateMonitor::Subscribe()
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (isSubscribing_) {
        IAM_LOGI("duplicate subscribe");
        return;
    }

    ResultCode result = EventSubscriber::Subscribe();
    IF_FALSE_LOGE_AND_RETURN(result == ResultCode::SUCCESS);
    isSubscribing_ = true;
    isOn_ = PowerMgrClient::GetInstance().IsScreenOn();
    IAM_LOGI("screen on %{public}d", isOn_);
}

void ScreenStateMonitor::Unsubscribe()
{
    std::lock_guard<std::mutex> lock(mutex_);

    EventSubscriber::Unsubscribe();
    isSubscribing_ = false;
}

void ScreenStateMonitor::SetScreenOn(bool isOn)
{
    std::lock_guard<std::mutex> lock(mutex_);

    isOn_ = isOn;
    IAM_LOGI("set screen on %{public}d", isOn_);
}

bool ScreenStateMonitor::GetScreenOn()
{
    std::lock_guard<std::mutex> lock(mutex_);

    return isOn_;
}
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS