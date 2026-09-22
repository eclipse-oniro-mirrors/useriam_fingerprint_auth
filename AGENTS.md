# 指纹认证组件指引

## 项目定位

本仓库对应 OpenHarmony `base/useriam/fingerprint_auth`。指纹认证（fingerprintauth）是 OpenHarmony 支持的一种生物认证执行器：按统一用户认证框架（useriam_user_auth_framework）定义的资源注册接口，将指纹认证相关资源注册到统一用户认证框架，并根据框架调度调用指纹认证 HDI，完成指纹的录入、删除、认证和识别等功能。指纹数据是用户的重要生物识别信息，录入/删除有较高访问权限要求，指纹数据的提取、比对、存储、录入应在可信执行环境/安全芯片等最高安全级别环境中实现。

优先按这些目录定位问题：

- `services/`：指纹认证 SA 服务主体，包括服务生命周期、HDI 驱动适配、执行器回调、SA 命令分发、传感器补光管理。
- `services_ex/`：扩展服务（动态加载的 `libfingerprintauthservice_ex.z.so`），包括传感器补光渲染任务、屏幕状态监听。
- `common/`：组件公共头文件和工具（结果码定义、日志宏、检查宏、智能指针工具）。
- `sa_profile/`：SA 配置文件（SA ID 943）。
- `test/`：单元测试和 fuzz 目标。

### 按任务类型定位代码

| 任务类型 | 首选目录 | 关键文件 |
| --- | --- | --- |
| 修改 SA 生命周期（启动/停止/驱动注册） | `services/` | `fingerprint_auth_service.h/cpp` |
| 修改 HDI 驱动管理（获取执行器列表/断连处理） | `services/` | `fingerprint_auth_driver_hdi.h/cpp` |
| 修改执行器能力（录入/认证/识别/删除/取消/属性） | `services/` | `fingerprint_auth_all_in_one_executor_hdi.h/cpp` |
| 修改执行结果回调（结果/提示/消息转换、振动反馈） | `services/` | `fingerprint_auth_executor_callback_hdi.h/cpp` |
| 修改 HDI 接口获取/桩替换 | `services/` | `fingerprint_auth_interface_adapter.h/cpp` |
| 修改 SA 命令（SA 与执行器间命令）分发 | `services/` | `sa_command_manager.h/cpp`, `isa_command_processor.h` |
| 修改传感器补光控制逻辑 | `services/` | `sensor_illumination_manager.h/cpp` |
| 修改扩展服务动态加载 | `services/` | `service_ex_manager.h/cpp` |
| 修改补光渲染（Rosen 绘制/亮度-透明度） | `services_ex/` | `sensor_illumination_task.h/cpp` |
| 修改屏幕状态监听 | `services_ex/` | `screen_state_monitor.h/cpp` |
| 修改公共定义（结果码等） | `common/inc/` | `fingerprint_auth_defines.h` |
| 修改 SA 配置 | `sa_profile/` | `943.json` |
| 修改构建配置 | 根目录 | `bundle.json`, `fingerprint_auth.gni`, `services/BUILD.gn`, `services_ex/BUILD.gn` |

### 嵌套指引

本仓库无目录级别的嵌套指引。

## 构建和验证

构建命令从 OpenHarmony 源码根目录执行，不在本子目录执行。

```sh
./build.sh --product-name rk3568 --build-target fingerprint_auth --ccache
```

单元测试构建与运行（构建目标名见 `test/unittest/BUILD.gn`，`FingerprintAuthServiceUnitTest` 与 `FingerprintSensorIlluminationTest` 两个 ohos_unittest）：

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 FingerprintAuthServiceUnitTest
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 FingerprintSensorIlluminationTest
```

fuzz 目标（`test/fuzztest/BUILD.gn`）：

```sh
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 FingerprintAuthServiceFuzzTest
prebuilts/build-tools/linux-x86/bin/ninja -C out/rk3568 FingerprintAuthAllInOneExecutorHdiFuzzTest
```

### 代码质量与契约检查

修改涉及以下类别时，除构建/测试外还需执行对应检查：

- **格式检查**：C++ 代码遵循仓根 `.clang-format`（BasedOnStyle LLVM，ColumnLimit 120，IndentWidth 4，SortIncludes true），提交前对改动文件运行 clang-format 校验格式
- **License 检查**：新增文件必须带 Apache-2.0 许可头；涉及 license 合规问题时运行 OSS Audit Tool（`OAT.xml` 为仓内配置）
- **公共 API / SDK 契约检查（涉及 SDK API 改动时）**：核对 `//interface/sdk-js/api/@ohos.userIAM.userAuth.d.ts` 中 `UserAuth`/`FingerprintTips` 等公开符号与枚举语义，确保 SDK 层声明与 HDI/执行器侧实现一致
- **HDI 契约检查（涉及 IDL 改动时）**：修改 `//drivers/interface/fingerprint_auth` 下 `*.idl` 后必须重新运行 IDL 编译器生成代码（见"生成代码边界"），并确认 proxy/stub 与 `v1_0`~`v2_0` 各版本兼容

### 完成标准

任务被认为完成，当且仅当：

1. **代码改动已提交** - 使用 `git commit -s`，多代理协作时添加 `Co-Authored-By: Agent`
2. **本地构建通过** - 执行上述构建命令
3. **相关测试通过** - 对应单元测试通过
4. **质量与契约检查通过（如适用）** - 按"代码质量与契约检查"执行 clang-format / OAT license / SDK 契约 / HDI 契约检查
5. **板侧验证（如适用）** - 涉及 HDI 交互/补光渲染/认证流程的改动需提供验证证据
6. **文档更新（如适用）** - 公共 API 修改需更新注释和文档

### 如果无法运行验证

明确说明无法运行的原因，列出推荐的验证步骤供人工执行，标记需要人工验证的部分。

### 完成报告格式

报告应包含：改动摘要（文件列表、改动点）、验证结果（构建/测试输出）、风险评估（API 兼容性、性能风险）、未完成事项。

## 知识索引

### 外部知识路由

不同任务需要读取的深层知识来源如下。**先读本仓库代码定位，再按需路由到下列外部文档**（不要重复在本仓库内猜测框架语义）：

| 任务/触发场景 | 应阅读的知识来源 | 用途 |
| --- | --- | --- |
| 修改执行器注册/调度/认证流程语义 | `//base/useriam/user_auth_framework`（README_ZH.md、执行器管理 SA 相关源码） | 理解框架侧对执行器注册、认证调度、回调契约的要求 |
| 任务涉及权限、安全模型、认证信任级别 | `//base/useriam/user_auth_framework` 的权限与安全相关文档/源码 | 确认权限边界与信任级别语义 |
| 修改或新增 HDI 接口/类型 | `//drivers/interface/fingerprint_auth` 下 `v1_0`~`v2_0` 的 `*.idl` 定义 | IDL 是 HDI 契约的单一事实来源，改动需同步 IDL 并重新生成 |
| 排查 HDI 桩行为/南向适配参考 | `//drivers/peripheral/fingerprint_auth`（hdi_service/common/test） | 桩实现参考与适配示例 |
| 修改 SDK 对外 API（`@ohos.userIAM.userAuth`）相关语义 | `//interface/sdk-js/api/@ohos.userIAM.userAuth.d.ts`（含 `FingerprintTips` 等枚举） | 确认 SDK 层公开 API 契约与枚举语义 |
| 涉及补光渲染/图形绘制 | `//foundation/graphic/graphic_2d` 相关文档与 `//foundation/arkui/advanced_ui_component` | Rosen 绘制、RSSurfaceNode 用法 |
| 组件整体架构与相关仓 | 本仓 `README_ZH.md`（含架构图）、`bundle.json` 相关仓列表 | 架构概览与依赖关系 |

### 领域词汇路由

任务描述、日志、issue、API 或文件名中出现以下术语时，按需加载对应知识：

| 领域词汇/术语 | 出现时的含义与应阅读位置 |
| --- | --- |
| `ResultCode` / `VENDOR_RESULT_CODE_BEGIN(=10000)` | 组件结果码枚举（`common/inc/fingerprint_auth_defines.h:24-77`）；厂商扩展码从 10000 起 |
| `ConvertResultCode` / `ConvertCommandId` | HDI↔框架结果码/命令码转换（`services/inc/fingerprint_auth_all_in_one_executor_hdi.h:65,69`、`fingerprint_auth_executor_callback_hdi.h:53`）；新增转换复用这些方法 |
| `GetSensorIlluminationTask` | 扩展服务导出入口（`services/inc/service_ex_manager.h:35`，经 `fingerprint_auth_service_ex_map` 的 version script 导出） |
| `SA` / `SystemAbility` / `943` | 指纹认证 SA（`sa_profile/943.json`，进程 `useriam`，`libfingerprintauthservice.z.so`） |
| `HDI` / `IAllInOneExecutor` / `IFingerprintAuthInterface` | 指纹 HDI 接口（`//drivers/interface/fingerprint_auth` 的 IDL 与 `libfingerprint_auth_proxy_2.0`） |
| `Executor` / 执行器 / 录入(enroll)/认证(auth)/识别(identify) | 执行器能力模型（`services/inc/fingerprint_auth_all_in_one_executor_hdi.h`，透传 HDI `IAllInOneExecutor`） |
| `LOCK_TEMPLATE` / `UNLOCK_TEMPLATE` | 锁屏/冻结策略命令（改动前必须确认，见"项目约束"） |
| `补光` / `sensor illumination` / `SensorIllumination` | 传感器补光渲染（`services_ex/` 的 `sensor_illumination_task`、`services/` 的 `sensor_illumination_manager`） |
| `MemoryGuard` | 防内存泄漏守卫（`services/src/memory_guard.cpp`），SA 生命周期中启用 |

### 架构背景

指纹认证 SA（`FingerprintAuthService`，SA ID 943，`sa_profile/943.json`，进程 `useriam`）作为执行器注册到统一用户认证框架（useriam_user_auth_framework），由框架通过执行器管理 SA 调度。SA 启动时通过 `IDriverManager::Start` 注册 HDI 配置（`fingerprint_auth_interface_service`），经 `FingerprintAuthDriverHdi` 获取 HDI 的执行器列表，封装为 `FingerprintAllInOneExecutorHdi` 供框架调度。南向厂商需实现 `//drivers/interface/fingerprint_auth` 定义的 IDL 接口，`//drivers/peripheral/fingerprint_auth` 提供桩实现。

主要调用链：

- 启动：`FingerprintAuthService::OnStart` → `StartDriverManager` → `IDriverManager::Start(HDI_NAME_2_CONFIG)`
- 获取执行器：`FingerprintAuthDriverHdi::GetExecutorList` → `FingerprintAuthInterfaceAdapter::Get`（`IFingerprintAuthInterface::Get`）→ `GetExecutorList` → 封装 `FingerprintAllInOneExecutorHdi`
- 认证/录入/识别：框架调用 `FingerprintAllInOneExecutorHdi::Enroll/Authenticate/Identify/...` → 透传 HDI `IAllInOneExecutor`，结果经 `FingerprintAuthExecutorCallbackHdi`（OnResult/OnTip/OnMessage）回传给框架
- 命令通道：HDI 执行器 → `SaCommandCallback::OnSaCommands` → `SaCommandManager::ProcessSaCommands` → 各 `ISaCommandProcessor`（如 `SensorIlluminationManager`）
- 补光：`SensorIlluminationManager` → `ServiceExManager`（dlopen `libfingerprintauthservice_ex.z.so`）→ `SensorIlluminationTask`（Rosen RSSurfaceNode 自绘制窗口画圆，按屏幕亮度线性插值背景透明度，超时 1000ms 自动关闭）

### 开始编辑前

在修改代码前，按以下顺序确认，并在回复中明确声明：

1. 确认任务类别（新增功能/缺陷修复/接口变更/构建配置/测试/文档）
2. 根据"按任务类型定位代码"表确定需要修改的目录与文件
3. 根据"外部知识路由"表判断是否需要先读取外部框架/HDI/SDK 文档
4. 根据"项目约束"确认不违反任何约束（Do-not / Ask-before / 架构不变量）
5. 声明："我将修改 X（文件列表），任务类别为 T，已阅读 Y 相关源码与 Z 文档，遵循 W 约束"

**开始编辑前必须先输出上述声明**，未完成约束核对前不修改任何代码。

## 项目约束

### 安全与生物识别数据约束

**Do not（禁止）：**

- 将指纹模板、认证结果等敏感信息写入非安全日志（日志格式串中 `%{public}` 会被打码，`%{private}` 不打码，注意区分）
- 绕过已有的权限检查或安全模型（指纹录入/删除属高权限操作）
- 在非安全环境（REE 明文）存储或比对指纹数据，或破坏与 HDI 的安全通道假设
- 将跨进程传递的 HDI 对象/缓冲区在未校验的情况下直接使用

**Ask before（修改前必须确认）：**

- 涉及执行器注册信息（模板 ID、公钥、认证子类型）暴露范围的改动
- 涉及认证结果/错误码语义变化的改动
- 涉及锁屏/冻结（LOCK_TEMPLATE/UNLOCK_TEMPLATE）策略的改动

### 公共 API 与协议兼容性

**Do not（禁止）：**

- 修改已发布 HDI 接口（`//drivers/interface/fingerprint_auth`）的签名、参数、返回值
- 修改 `fingerprint_auth_defines.h` 中已有 `ResultCode` 枚举值（厂商扩展码从 `VENDOR_RESULT_CODE_BEGIN = 10000` 之后开始）
- 修改 `ConvertResultCode`/`ConvertCommandId` 等 HDI↔框架结果码/命令码映射表中已有映射的语义

**Ask before（修改前必须确认）：**

- 新增结果码/命令码映射
- 修改 HDI 断连（`OnHdiDisconnect`）行为

### 性能约束

- 补光渲染（`DrawSurfaceNode`/`DrawCanvas`）在认证期间会被高频调用（亮度变化即重绘），不要在绘制路径中增加全量扫描、字符串格式化或 INFO 日志
- 回调路径（`OnResult`/`OnTip`/`OnMessage`）避免阻塞或耗时操作

### 并发与生命周期约束

- `SaCommandManager` 使用 `shared_mutex` 保护注册表，`SensorIlluminationManager`/`SensorIlluminationTask` 使用 `mutex`/`recursive_mutex` 串行化补光状态，改动时保持既有锁序与作用域，避免死锁
- `SensorIlluminationTask` 使用 `weak_from_this` 在定时器回调中防悬垂，新增异步任务时遵循相同模式
- 保持 `ServiceExManager` 的 `dlopen` 单例句柄语义，不要在 `Release` 中提前 `dlclose`（当前版本策略是 so 不释放）

### 编码约定

- 日志统一使用 `IAM_LOGI/IAM_LOGE` 等宏（`common/logs/iam_logger.h`，LOG_TAG 为 `FINGERPRINT_AUTH_SA`），不要直接调用 hilog
- 指针判空使用 `IF_FALSE_LOGE_AND_RETURN`/`IF_FALSE_LOGE_AND_RETURN_VAL`（`common/utils/iam_check.h`），不要使用改变代码逻辑的 `CHKPV*`/`CHKPR*` 等宏
- 智能指针创建使用 `Common::MakeShared/MakeUnique`（`common/utils/iam_ptr.h`，异常安全），`sptr` 与 `std::shared_ptr` 互转使用 `SptrToStdSharedPtr`
- HDI 类型与框架类型转换（ExecutorInfo/Property/ResultCode/命令 ID）集中在 `FingerprintAllInOneExecutorHdi` 的 `Convert*`/`Move*` 方法中，新增转换优先复用这些方法

### 生成代码边界

**Do not（禁止）：**

- 直接修改 HDI IDL 编译器生成的 C++ 代码（`v2_0/*` 头文件、proxy/stub）

**正确做法：**

- 修改 IDL 定义文件（`//drivers/interface/fingerprint_auth` 下的 `*.idl`）
- 重新运行 IDL 编译器生成代码

### 构建配置约束

- `services/BUILD.gn` 与 `services_ex/BUILD.gn` 中 sanitize（integer_overflow/ubsan/boundary_sanitize/cfi/cfi_cross_dso）与 `branch_protector_ret = "pac_ret"` 是安全加固要求，新增编译目标时保持一致的加固配置
- 修改 `fingerprint_auth.gni` 中 `use_display_manager_component`/`use_power_manager_component` 开关时，需同时确认 `services_ex/BUILD.gn` 中对应 `CONFIG_USE_*` 宏与源码 `#ifdef` 分支保持一致

### 设备操作约束

**涉及真实设备时的注意事项：**

- 不执行可能影响设备正常运行的破坏性操作（如强制删除全部指纹、强制关闭指纹传感器）
- 需要在真实设备上验证的改动（如补光渲染效果、认证成功率），必须提供板侧证据（日志、截图、hdc 输出）
- 涉及指纹数据的测试需在受控设备上进行，避免真实用户指纹数据泄露

### 第三方依赖与 License 约束

**Do not（禁止）：**

- 未经合规评估直接引入新的第三方源码/库；本组件依赖全部通过 `bundle.json` 的 `components` 列表与 `BUILD.gn` 的 `external_deps` 声明（`bundle.json:23-44`），不要绕过依赖声明直接 `#include` 外部头文件或链接未声明库
- 修改 `OAT.xml` 中的 license 过滤规则来掩盖 license 问题，或在新增文件时省略 Apache-2.0 许可头（仓库 `LICENSE` 为 Apache License 2.0）

**Ask before（修改前必须确认）：**

- 新增或变更 `bundle.json` 组件依赖（如新增 `display_manager`/`power_manager` 之外的图形、传感器依赖），需确认对应组件在本产品是否使能
- 升级/替换 `drivers_interface_fingerprint_auth`（`libfingerprint_auth_proxy_2.0`）或 `user_auth_framework`（`userauth_executors`）等关键依赖版本
