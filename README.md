# Inviction Challenge

## Environment

 * MacOS 11.2
 * nRF Connect 3.1.0

 ## Build Configuration (using nRF Connect extension for VS Code)

    * SDK: nRF Connect SDK v3.1.0
    * Toolchain: nRF Connect Toolchain v3.1.0
    * Board target: nrf52840dk/nrf52840
    * Base configuration: prj.conf
    * Extra Kconfig fragments: overlay-no-ble.conf [optional]
    * Base Devicetree overlays: boards/nrf52840dk_nrf52840.overlay

## BLE

Primary service UUID `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`

Characteristic UUID `6E400001-B5A3-XXXX-E0A9-E50E24DCCA9E`
| Characteristic | UUID | Flags |
| -- | -- | -- |
| Voltage mV | `F394` | Read/Notify |
| Interval | `F395` | Read-only |

## Settings

NA

## Testing

NA

## Notes

    * Support for `native_sim` is only available for Linux
