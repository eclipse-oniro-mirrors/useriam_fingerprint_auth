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

#include "fingerprint_auth_executor_hdi.h"

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <utility>
#include <vector>

#include "hdf_base.h"
#include "refbase.h"

#include "iam_check.h"
#include "iam_executor_framework_types.h"
#include "iam_logger.h"

#include "fingerprint_auth_defines.h"
#include "fingerprint_auth_executor_callback_hdi.h"
#include "fingerprint_auth_hdi.h"
#include "sa_command_manager.h"

#define LOG_LABEL UserIam::Common::LABEL_FINGERPRINT_AUTH_SA

namespace OHOS {
namespace UserIam {
namespace FingerprintAuth {
using IamResultCode = UserAuth::ResultCode;
using IamExecutorRole = UserAuth::ExecutorRole;
using IamExecutorInfo = UserAuth::ExecutorInfo;
namespace UserAuth = OHOS::UserIam::UserAuth;
FingerprintAuthExecutorHdi::FingerprintAuthExecutorHdi(sptr<IExecutor> executorProxy)
    : executorProxy_(executorProxy) {};

IamResultCode FingerprintAuthExecutorHdi::GetExecutorInfo(IamExecutorInfo &info)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    ExecutorInfo localInfo = {};
    int32_t status = executorProxy_->GetExecutorInfo(localInfo);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("GetExecutorInfo fail result %{public}d", result);
        return result;
    }
    result = MoveHdiExecutorInfo(localInfo, info);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("MoveHdiExecutorInfo fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::OnRegisterFinish(const std::vector<uint64_t> &templateIdList,
    const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->OnRegisterFinish(templateIdList, frameworkPublicKey, extraInfo);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("OnRegisterFinish fail result %{public}d", result);
        return result;
    }

    result = RegisterSaCommandCallback();
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("RegisterSaCommandCallback fail");
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::Enroll(uint64_t scheduleId, const UserAuth::EnrollParam &param,
    const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback =
        sptr<IExecutorCallback>(new (std::nothrow) FingerprintAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Enroll(scheduleId, param.extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Enroll fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::Authenticate(uint64_t scheduleId, const UserAuth::AuthenticateParam &param,
    const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback =
        sptr<IExecutorCallback>(new (std::nothrow) FingerprintAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Authenticate(scheduleId, param.templateIdList, param.extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Authenticate fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::Identify(uint64_t scheduleId, const UserAuth::IdentifyParam &param,
    const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    auto callback =
        sptr<IExecutorCallback>(new (std::nothrow) FingerprintAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Identify(scheduleId, param.extraInfo, callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Identify fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::Delete(const std::vector<uint64_t> &templateIdList)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Delete(templateIdList);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Delete fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::Cancel(uint64_t scheduleId)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->Cancel(scheduleId);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("Cancel fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::SendCommand(UserAuth::PropertyMode commandId,
    const std::vector<uint8_t> &extraInfo, const std::shared_ptr<UserAuth::IExecuteCallback> &callbackObj)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);
    IF_FALSE_LOGE_AND_RETURN_VAL(callbackObj != nullptr, IamResultCode::GENERAL_ERROR);
    CommandId hdiCommandId;
    IamResultCode result = ConvertCommandId(commandId, hdiCommandId);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertCommandId fail result %{public}d", result);
        return result;
    }
    auto callback =
        sptr<IExecutorCallback>(new (std::nothrow) FingerprintAuthExecutorCallbackHdi(callbackObj));
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);
    int32_t status = executorProxy_->SendCommand(hdiCommandId, extraInfo, callback);
    result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("SendCommand fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

UserAuth::ResultCode FingerprintAuthExecutorHdi::GetProperty(const std::vector<uint64_t> &templateIdList,
    const std::vector<UserAuth::Attributes::AttributeKey> &keys, UserAuth::Property &property)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);

    std::vector<GetPropertyType> propertyTypes;
    IamResultCode result = ConvertAttributeKeyVectorToPropertyType(keys, propertyTypes);
    IF_FALSE_LOGE_AND_RETURN_VAL(result == IamResultCode::SUCCESS, IamResultCode::GENERAL_ERROR);

    Property hdiProperty;
    int32_t status = executorProxy_->GetProperty(templateIdList, propertyTypes, hdiProperty);
    result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("SendCommand fail result %{public}d", result);
        return result;
    }
    MoveHdiProperty(hdiProperty, property);
    return IamResultCode::SUCCESS;
}

UserAuth::ResultCode FingerprintAuthExecutorHdi::SetCachedTemplates(const std::vector<uint64_t> &templateIdList)
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);

    int32_t status = executorProxy_->SetCachedTemplates(templateIdList);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("SendCommand fail result %{public}d", result);
        return result;
    }
    return IamResultCode::SUCCESS;
}

void FingerprintAuthExecutorHdi::OnHdiDisconnect()
{
    IAM_LOGE("start");
    SaCommandManager::GetInstance().OnHdiDisconnect(shared_from_this());
}

IamResultCode FingerprintAuthExecutorHdi::MoveHdiExecutorInfo(ExecutorInfo &in, IamExecutorInfo &out)
{
    out.executorSensorHint = static_cast<uint32_t>(in.sensorId);
    out.executorMatcher = in.executorType;
    IamResultCode result = ConvertExecutorRole(in.executorRole, out.executorRole);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertExecutorRole fail result %{public}d", result);
        return result;
    }
    result = ConvertAuthType(in.authType, out.authType);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertAuthType fail result %{public}d", result);
        return result;
    }
    result = ConvertExecutorSecureLevel(in.esl, out.esl);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ConvertExecutorSecureLevel fail result %{public}d", result);
        return result;
    }
    in.publicKey.swap(out.publicKey);
    return IamResultCode::SUCCESS;
}

void FingerprintAuthExecutorHdi::MoveHdiProperty(Property &in, UserAuth::Property &out)
{
    out.authSubType = in.authSubType;
    out.lockoutDuration = in.lockoutDuration;
    out.remainAttempts = in.remainAttempts;
    out.enrollmentProgress.swap(in.enrollmentProgress);
    out.sensorInfo.swap(in.sensorInfo);
}

void FingerprintAuthExecutorHdi::MoveHdiTemplateInfo(TemplateInfo &in, UserAuth::TemplateInfo &out)
{
    out.executorType = in.executorType;
    out.freezingTime = in.lockoutDuration;
    out.remainTimes = in.remainAttempts;
    in.extraInfo.swap(out.extraInfo);
}

IamResultCode FingerprintAuthExecutorHdi::ConvertCommandId(const UserAuth::PropertyMode in, CommandId &out)
{
    if (static_cast<CommandId>(in) > CommandId::VENDOR_COMMAND_BEGIN) {
        out = static_cast<CommandId>(in);
        IAM_LOGI("vendor command id %{public}d, no covert", out);
        return IamResultCode::SUCCESS;
    }

    static const std::map<UserAuth::PropertyMode, CommandId> data = {
        { UserAuth::PropertyMode::PROPERTY_MODE_FREEZE, CommandId::LOCK_TEMPLATE },
        { UserAuth::PropertyMode::PROPERTY_MODE_UNFREEZE, CommandId::UNLOCK_TEMPLATE } };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("command id %{public}d is invalid", in);
        return IamResultCode::INVALID_PARAMETERS;
    }
    out = iter->second;
    IAM_LOGI("covert command id %{public}d to idl command is %{public}d", in, out);
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertAuthType(const AuthType in, UserAuth::AuthType &out)
{
    static const std::map<AuthType, UserAuth::AuthType> data = {
        { AuthType::FINGERPRINT, UserAuth::AuthType::FINGERPRINT },
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("authType %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertExecutorRole(const ExecutorRole in, IamExecutorRole &out)
{
    static const std::map<ExecutorRole, IamExecutorRole> data = {
        { ExecutorRole::COLLECTOR, IamExecutorRole::COLLECTOR },
        { ExecutorRole::VERIFIER, IamExecutorRole::VERIFIER },
        { ExecutorRole::ALL_IN_ONE, IamExecutorRole::ALL_IN_ONE },
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("executorRole %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertExecutorSecureLevel(const ExecutorSecureLevel in,
    UserAuth::ExecutorSecureLevel &out)
{
    static const std::map<ExecutorSecureLevel, UserAuth::ExecutorSecureLevel> data = {
        { ExecutorSecureLevel::ESL0, UserAuth::ExecutorSecureLevel::ESL0 },
        { ExecutorSecureLevel::ESL1, UserAuth::ExecutorSecureLevel::ESL1 },
        { ExecutorSecureLevel::ESL2, UserAuth::ExecutorSecureLevel::ESL2 },
        { ExecutorSecureLevel::ESL3, UserAuth::ExecutorSecureLevel::ESL3 },
    };
    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("executorSecureLevel %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    }
    out = iter->second;
    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertResultCode(const int32_t in)
{
    HDF_STATUS hdfIn = static_cast<HDF_STATUS>(in);
    static const std::map<HDF_STATUS, IamResultCode> data = {
        { HDF_SUCCESS, IamResultCode::SUCCESS },
        { HDF_FAILURE, IamResultCode::GENERAL_ERROR },
        { HDF_ERR_TIMEOUT, IamResultCode::TIMEOUT },
        { HDF_ERR_QUEUE_FULL, IamResultCode::BUSY },
        { HDF_ERR_DEVICE_BUSY, IamResultCode::BUSY },
    };

    IamResultCode out;
    auto iter = data.find(hdfIn);
    if (iter == data.end()) {
        out = IamResultCode::GENERAL_ERROR;
    } else {
        out = iter->second;
    }
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", in, out);
    return out;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertAttributeKeyVectorToPropertyType(
    const std::vector<UserAuth::Attributes::AttributeKey> inItems, std::vector<GetPropertyType> &outItems)
{
    outItems.clear();
    for (auto &inItem : inItems) {
        GetPropertyType outItem;
        IamResultCode result = ConvertAttributeKeyToPropertyType(inItem, outItem);
        IF_FALSE_LOGE_AND_RETURN_VAL(result == IamResultCode::SUCCESS, IamResultCode::GENERAL_ERROR);
        outItems.push_back(outItem);
    }

    return IamResultCode::SUCCESS;
}

IamResultCode FingerprintAuthExecutorHdi::ConvertAttributeKeyToPropertyType(const UserAuth::Attributes::AttributeKey in,
    GetPropertyType &out)
{
    static const std::map<UserAuth::Attributes::AttributeKey, GetPropertyType> data = {
        { UserAuth::Attributes::ATTR_PIN_SUB_TYPE, GetPropertyType::AUTH_SUB_TYPE },
        { UserAuth::Attributes::ATTR_FREEZING_TIME, GetPropertyType::LOCKOUT_DURATION },
        { UserAuth::Attributes::ATTR_REMAIN_TIMES, GetPropertyType::REMAIN_ATTEMPTS },
        { UserAuth::Attributes::ATTR_ENROLL_PROGRESS, GetPropertyType::ENROLL_PROGRESS },
        { UserAuth::Attributes::ATTR_SENSOR_INFO, GetPropertyType::SENSOR_INFO },
    };

    auto iter = data.find(in);
    if (iter == data.end()) {
        IAM_LOGE("attribute %{public}d is invalid", in);
        return IamResultCode::GENERAL_ERROR;
    } else {
        out = iter->second;
    }
    IAM_LOGI("covert hdi result code %{public}d to framework result code %{public}d", in, out);
    return IamResultCode::SUCCESS;
}

UserAuth::ResultCode FingerprintAuthExecutorHdi::RegisterSaCommandCallback()
{
    IF_FALSE_LOGE_AND_RETURN_VAL(executorProxy_ != nullptr, IamResultCode::GENERAL_ERROR);

    sptr<SaCommandCallback> callback = new (std::nothrow) SaCommandCallback(shared_from_this());
    IF_FALSE_LOGE_AND_RETURN_VAL(callback != nullptr, IamResultCode::GENERAL_ERROR);

    int32_t status = executorProxy_->RegisterSaCommandCallback(callback);
    IamResultCode result = ConvertResultCode(status);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("RegisterSaCommandCallback fail result %{public}d", result);
        return result;
    }

    return IamResultCode::SUCCESS;
}

int32_t FingerprintAuthExecutorHdi::SaCommandCallback::OnSaCommands(const std::vector<SaCommand> &commands)
{
    IAM_LOGI("start");
    IamResultCode result = SaCommandManager::GetInstance().ProcessSaCommands(executorHdi_, commands);
    if (result != IamResultCode::SUCCESS) {
        IAM_LOGE("ProcessSaCommands fail");
        return HDF_FAILURE;
    }
    IAM_LOGI("success");
    return HDF_SUCCESS;
};
} // namespace FingerprintAuth
} // namespace UserIam
} // namespace OHOS
