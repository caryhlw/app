#ifdef CONFIG_BT

#include <errno.h>
#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "ble.h"
#include "uuids.h"
#include "adc.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_ble, LOG_LEVEL);

static const struct bt_data adv[] =
{
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
};
    
static struct bt_uuid_128 base_uuid =
BT_UUID_INIT_128(BT_UUID_128_ENCODE(UUID_1_1, UUID_1_2, UUID_1_3, UUID_1_4, UUID_1_5));
static const struct bt_uuid_128 uuid_sample_interval =
BT_UUID_INIT_128(BT_UUID_128_ENCODE(UUID_2_1, UUID_2_2, UUID_2_3, UUID_2_4, UUID_2_5));
static const struct bt_uuid_128 uuid_voltage_mv =
BT_UUID_INIT_128(BT_UUID_128_ENCODE(UUID_3_1, UUID_3_2, UUID_3_3, UUID_3_4, UUID_3_5));
    
static ssize_t sample_voltage_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);
static ssize_t sample_interval_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);
static void ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value);
BT_GATT_SERVICE_DEFINE(APP_SVC,
    BT_GATT_PRIMARY_SERVICE(&base_uuid),
    BT_GATT_CHARACTERISTIC(&uuid_voltage_mv.uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, sample_voltage_read_cb, NULL, NULL),
    BT_GATT_CHARACTERISTIC(&uuid_sample_interval.uuid, BT_GATT_CHRC_READ
        , BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, sample_interval_read_cb, NULL, NULL),
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE)
);

static void connected(struct bt_conn* conn, uint8_t err);
static void disconnected(struct bt_conn* conn, uint8_t reason);

struct bt_conn_cb conn_cb =
{
    .connected = connected,
    .disconnected = disconnected
};

int ble_init(void)
{
    
    int rc;
    
    rc = bt_enable(NULL);
    if (rc == 0)
    {
        LOG_INF("BLE enabled");
        rc = bt_le_adv_start(BT_LE_ADV_CONN_NAME, adv, ARRAY_SIZE(adv), NULL, 0);
        if (rc == 0)
        {
            LOG_INF("Advertising as %s", "FW-CHALLENGE");
            bt_conn_cb_register(&conn_cb);
            
            #ifdef CONFIG_X
            // Bonus
            smp_bt_register();
            #endif
        }
        else
        {
            LOG_WRN("Advertising failed (%d)", rc);
        }
    }
    else
    {
        LOG_WRN("BLE enable failed (%d)", rc);
    }
    return rc;
}

void ble_voltage_update(uint8_t voltage_mv)
{
    LOG_INF("Voltage mV updated");
    bt_gatt_notify(NULL, &APP_SVC.attrs[1], &voltage_mv, sizeof(voltage_mv));
}

static void connected(struct bt_conn* conn, uint8_t err)
{
    if (err == 0)
    {
        LOG_INF("Connected");
    }
    else
    {

    }
}

static void disconnected(struct bt_conn* conn, uint8_t reason)
{
    LOG_INF("Disconnected (%d), reason");
}

static ssize_t sample_voltage_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
    int voltage_mv;

    LOG_DBG("Reading sample voltage from within callback...");
    voltage_mv = adc_voltage_get();
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &voltage_mv, sizeof(voltage_mv));
}

static ssize_t sample_interval_read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
    int interval_ms;

    LOG_DBG("Reading sample interval from within callback...");
    interval_ms = adc_sample_interval_get();
    return bt_gatt_attr_read(conn, attr, buf, len, offset, &interval_ms, sizeof(interval_ms));
}

static void ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    return;
}

#endif
