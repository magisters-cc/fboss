// Copyright 2004-present Facebook. All Rights Reserved.

#include "fboss/agent/hw/switch_asics/Ramon3Asic.h"
#include <thrift/lib/cpp/util/EnumUtils.h>
#include "fboss/agent/FbossError.h"

namespace facebook::fboss {
bool Ramon3Asic::isSupported(Feature feature) const {
  switch (feature) {
    case HwAsic::Feature::REMOVE_PORTS_FOR_COLDBOOT:
    case HwAsic::Feature::FABRIC_PORTS:
    case HwAsic::Feature::LINK_TRAINING:
    case HwAsic::Feature::PMD_RX_LOCK_STATUS:
    case HwAsic::Feature::PMD_RX_SIGNAL_DETECT:
    case HwAsic::Feature::FEC:
    case HwAsic::Feature::FEC_CORRECTED_BITS:
    case HwAsic::Feature::PORT_FABRIC_ISOLATE:
    case HwAsic::Feature::SWITCH_DROP_STATS:
    case HwAsic::Feature::SAI_CONFIGURE_SIX_TAP:
    case HwAsic::Feature::TELEMETRY_AND_MONITORING:
    case HwAsic::Feature::WARMBOOT:
    case HwAsic::Feature::SAI_PORT_ERR_STATUS:
      return true;
    case HwAsic::Feature::SAI_PORT_SERDES_FIELDS_RESET:
    case HwAsic::Feature::FABRIC_TX_QUEUES:
    case HwAsic::Feature::RX_LANE_SQUELCH_ENABLE:
      return getAsicMode() != AsicMode::ASIC_MODE_SIM;
    case HwAsic::Feature::SAI_FEC_COUNTERS: // Enable after performance issues
                                            // reported in CS00012326877 are
                                            // fixed
      return false;
    default:
      return false;
  }
  return false;
}

std::set<cfg::StreamType> Ramon3Asic::getQueueStreamTypes(
    cfg::PortType portType) const {
  switch (portType) {
    case cfg::PortType::CPU_PORT:
    case cfg::PortType::INTERFACE_PORT:
    case cfg::PortType::RECYCLE_PORT:
      break;
    case cfg::PortType::FABRIC_PORT:
      return {cfg::StreamType::FABRIC_TX};
  }
  throw FbossError(
      "Ramon3 Asic does not support port type: ",
      apache::thrift::util::enumNameSafe(portType));
}

int Ramon3Asic::getDefaultNumPortQueues(
    cfg::StreamType /* streamType */,
    cfg::PortType portType) const {
  if (portType != cfg::PortType::FABRIC_PORT) {
    throw FbossError("Only fabric ports expected on Ramon asic");
  }
  // On Ramons we use a single fabric queue for all
  // traffic
  return 1;
}
uint64_t Ramon3Asic::getDefaultReservedBytes(
    cfg::StreamType /* streamType */,
    cfg::PortType /* portType */) const {
  throw FbossError("Ramon3 doesn't support queue feature");
}
cfg::MMUScalingFactor Ramon3Asic::getDefaultScalingFactor(
    cfg::StreamType /* streamType */,
    bool /* cpu */) const {
  throw FbossError("Ramon3 doesn't support queue feature");
}

uint32_t Ramon3Asic::getMaxMirrors() const {
  return 0;
}
uint16_t Ramon3Asic::getMirrorTruncateSize() const {
  throw FbossError("Ramon3 doesn't support mirror feature");
}

uint32_t Ramon3Asic::getMaxLabelStackDepth() const {
  throw FbossError("Ramon3 doesn't support label feature");
};
uint64_t Ramon3Asic::getMMUSizeBytes() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
};
int Ramon3Asic::getMaxNumLogicalPorts() const {
  throw FbossError("Ramon3 doesn't support logical ports feature");
}
uint32_t Ramon3Asic::getMaxWideEcmpSize() const {
  throw FbossError("Ramon3 doesn't support ecmp feature");
}
uint32_t Ramon3Asic::getMaxLagMemberSize() const {
  throw FbossError("Ramon3 doesn't support lag feature");
}
uint32_t Ramon3Asic::getPacketBufferUnitSize() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}
uint32_t Ramon3Asic::getPacketBufferDescriptorSize() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}
uint32_t Ramon3Asic::getMaxVariableWidthEcmpSize() const {
  return 512;
}
uint32_t Ramon3Asic::getMaxEcmpSize() const {
  return 4096;
}

uint32_t Ramon3Asic::getNumCores() const {
  throw FbossError("Num cores API not supported by Ramon3Asic");
}

bool Ramon3Asic::scalingFactorBasedDynamicThresholdSupported() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}
int Ramon3Asic::getBufferDynThreshFromScalingFactor(
    cfg::MMUScalingFactor /* scalingFactor */) const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}
uint32_t Ramon3Asic::getStaticQueueLimitBytes() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}
uint32_t Ramon3Asic::getNumMemoryBuffers() const {
  throw FbossError("Ramon3 doesn't support MMU feature");
}

const std::map<cfg::PortType, cfg::PortLoopbackMode>&
Ramon3Asic::desiredLoopbackModes() const {
  static const std::map<cfg::PortType, cfg::PortLoopbackMode> kLoopbackMode = {
      {cfg::PortType::FABRIC_PORT, cfg::PortLoopbackMode::MAC}};
  return kLoopbackMode;
}

uint32_t Ramon3Asic::getVirtualDevices() const {
  return 2;
}
}; // namespace facebook::fboss
