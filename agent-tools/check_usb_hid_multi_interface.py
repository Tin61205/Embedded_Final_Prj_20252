from pathlib import Path
import re
import sys


ROOT = Path(__file__).resolve().parents[1]
HID_CORE_C = ROOT / "ub_lib" / "usb_hid_host_lolevel" / "usbh_hid_core.c"
HID_CORE_H = ROOT / "ub_lib" / "usb_hid_host_lolevel" / "usbh_hid_core.h"
USBH_CONF_H = ROOT / "ub_lib" / "usb_hid_host_lolevel" / "usbh_conf.h"


def fail(message):
    print(f"FAIL: {message}")
    return False


def main():
    checks = []
    core = HID_CORE_C.read_text(encoding="utf-8", errors="ignore")
    header = HID_CORE_H.read_text(encoding="utf-8", errors="ignore")
    conf = USBH_CONF_H.read_text(encoding="utf-8", errors="ignore")

    match = re.search(r"#define\s+USBH_MAX_NUM_INTERFACES\s+(\d+)", conf)
    checks.append(match is not None or fail("USBH_MAX_NUM_INTERFACES is missing"))
    if match is not None:
        checks.append(int(match.group(1)) >= 4 or fail("USBH_MAX_NUM_INTERFACES must allow at least 4 interfaces"))

    checks.append("itf_idx" in header or fail("HID_Machine must store the selected interface index"))
    checks.append("USBH_HID_FindBootInterface" in core or fail("HID core must search for a boot keyboard/mouse interface"))
    checks.append("Itf_Desc[HID_Machine.itf_idx]" in core or fail("HID core must read the selected interface descriptor"))
    checks.append("Ep_Desc[HID_Machine.itf_idx]" in core or fail("HID core must read endpoints from the selected interface"))
    checks.append("wIndex.w = HID_Machine.itf_idx" in core or fail("HID class requests must target the selected interface"))
    checks.append("wValue.w = protocol" in core or fail("SET_PROTOCOL must pass through the requested protocol value"))
    checks.append("Itf_Desc[0]" not in core or fail("HID core must not hard-code interface 0"))
    checks.append("Ep_Desc[0]" not in core or fail("HID core must not hard-code endpoint table 0"))

    if not all(checks):
        return 1

    print("PASS: USB HID host scans and targets the selected HID boot interface")
    return 0


if __name__ == "__main__":
    sys.exit(main())
