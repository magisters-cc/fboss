// (c) Meta Platforms, Inc. and affiliates. Confidential and proprietary.

#include "fboss/platform/platform_manager/PkgUtils.h"

#include <folly/logging/xlog.h>

#include "fboss/platform/helpers/PlatformUtils.h"

namespace {
constexpr auto kHostKernelVersion = "`uname --kernel-release`";
}

namespace facebook::fboss::platform::platform_manager {

void PkgUtils::processRpms(const PlatformConfig& config) const {
  auto bspKmodsRpmName = fmt::format(
      "{}-{}-{}",
      *config.bspKmodsRpmName(),
      kHostKernelVersion,
      *config.bspKmodsRpmVersion());
  if (!isRpmInstalled(bspKmodsRpmName)) {
    XLOG(INFO) << fmt::format("Installing BSP Kmods {}", bspKmodsRpmName);
    installRpm(bspKmodsRpmName, 3 /* maxAttempts */);
  } else {
    XLOG(INFO) << fmt::format(
        "BSP Kmods {} is already installed", bspKmodsRpmName);
  }
}

void PkgUtils::processKmods(const PlatformConfig& config) const {
  XLOG(INFO) << "Reloading Kernel Modules";
  for (auto& kmod : *config.kmodsToReload()) {
    reloadKmod(kmod);
  }
}

bool PkgUtils::isRpmInstalled(const std::string& rpmFullName) const {
  XLOG(INFO) << fmt::format(
      "Checking whether BSP Kmods {} is installed", rpmFullName);
  auto cmd = fmt::format("dnf list {} --installed", rpmFullName);
  XLOG(INFO) << fmt::format("Running command ({})", cmd);
  auto [exitStatus, standardOut] = PlatformUtils().execCommand(cmd);
  return exitStatus == 0;
}

void PkgUtils::installRpm(const std::string& rpmFullName, int maxAttempts)
    const {
  int exitStatus{0}, attempt{1};
  std::string standardOut{};
  auto cmd = fmt::format("dnf install {} --assumeyes", rpmFullName);
  do {
    XLOG(INFO) << fmt::format(
        "Running command ({}); Attempt: {}", cmd, attempt++);
    std::tie(exitStatus, standardOut) = PlatformUtils().execCommand(cmd);
    XLOG(INFO) << standardOut;
  } while (attempt <= maxAttempts && exitStatus != 0);
  if (exitStatus != 0) {
    XLOG(ERR) << fmt::format(
        "Command ({}) failed with exit code {}", cmd, exitStatus);
    throw std::runtime_error(fmt::format(
        "Failed to install rpm ({}) with exit code {}",
        rpmFullName,
        exitStatus));
  }
}

void PkgUtils::reloadKmod(const std::string& moduleName) const {
  int exitStatus{0};
  std::string standardOut{};
  auto unloadCmd = fmt::format("modprobe --remove {}", moduleName);
  auto loadCmd = fmt::format("modprobe {}", moduleName);
  XLOG(INFO) << fmt::format("Running command ({})", unloadCmd);
  std::tie(exitStatus, standardOut) = PlatformUtils().execCommand(unloadCmd);
  if (exitStatus != 0) {
    XLOG(ERR) << fmt::format(
        "Command ({}) failed with exit code {}", unloadCmd, exitStatus);
    throw std::runtime_error(fmt::format(
        "Failed to unload kmod ({}) with exit code {}",
        moduleName,
        exitStatus));
  }
  XLOG(INFO) << fmt::format("Running command ({})", loadCmd);
  std::tie(exitStatus, standardOut) = PlatformUtils().execCommand(loadCmd);
  if (exitStatus != 0) {
    XLOG(ERR) << fmt::format(
        "Command ({}) failed with exit code {}", loadCmd, exitStatus);
    throw std::runtime_error(fmt::format(
        "Failed to load kmod ({}) with exit code {}", moduleName, exitStatus));
  }
}

} // namespace facebook::fboss::platform::platform_manager
