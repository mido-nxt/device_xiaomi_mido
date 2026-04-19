/*
 * Copyright (C) 2019,2021 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "vendor.lineage.touch-service.keydisabler"

#include "KeyDisabler.h"

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/strings.h>
#include <unistd.h>

using ::android::base::ReadFileToString;
using ::android::base::Trim;
using ::android::base::WriteStringToFile;

namespace aidl {
namespace vendor {
namespace lineage {
namespace touch {

static constexpr const char kControlPath[] = "/proc/sys/dev/xiaomi_msm8953_touchscreen/disable_keys";

KeyDisabler::KeyDisabler() {
    has_key_disabler_ = (access(kControlPath, F_OK) == 0);
}

ndk::ScopedAStatus KeyDisabler::getEnabled(bool* _aidl_return) {
    if (!has_key_disabler_) {
        *_aidl_return = false;
        return ndk::ScopedAStatus::ok();
    }

    std::string buf;
    if (!ReadFileToString(kControlPath, &buf, true)) {
        LOG(ERROR) << "Failed to read from " << kControlPath;
        *_aidl_return = false;
        return ndk::ScopedAStatus::ok();
    }

    *_aidl_return = (Trim(buf) == "1");
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus KeyDisabler::setEnabled(bool enabled) {
    if (!has_key_disabler_) {
        return ndk::ScopedAStatus::fromExceptionCode(EX_UNSUPPORTED_OPERATION);
    }

    if (!WriteStringToFile(enabled ? "1" : "0", kControlPath, true)) {
        LOG(ERROR) << "Failed to write to " << kControlPath;
        return ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_STATE);
    }

    return ndk::ScopedAStatus::ok();
}

}  // namespace touch
}  // namespace lineage
}  // namespace vendor
}  // namespace aidl
