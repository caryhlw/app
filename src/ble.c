#ifdef CONFIG_BT

#include <errno.h>
#include <zephyr/logging/log.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>

#include "ble.h"

#define LOG_LEVEL LOG_LEVEL_INF
LOG_MODULE_REGISTER(app_ble, LOG_LEVEL);

static ssize_t read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset);
static void ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value);

static const struct bt_data adv[] =
{
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)
};

static struct bt_uuid_128 base_uuid = BT_UUID_INIT_128(
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
);

static const struct bt_uuid_128 char_uuid = BT_UUID_INIT_128(
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
);

BT_GATT_SERVICE_DEFINE(svc,
    BT_GATT_PRIMARY_SERVICE(&base_uuid),
    BT_GATT_CHARACTERISTIC(&char_uuid, BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE, read_cb, NULL, NULL),
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ)
);

int ble_init(void)
{

    int rc;
    
#ifdef CONFIG_BT
    LOG_DBG("Initializing BLE");

    rc = bt_enable(NULL);
    if (rc == 0)
    {
        rc = bt_le_adv_start(BT_LE_ADV_CONN_NAME, adv, ARRAY_SIZE(adv), NULL, 0);
        if (rc == 0)
        {
            LOG_INF("Advertising started");
        }
        else
        {
            LOG_ERR("Advertising failed (%d)", rc);
        }
    }
    else
    {
        LOG_ERR("BLE enable failed (%d)", rc);
    }
#else
    LOG_INF("BLE not configured");
    rc = 0;
#endif
    return rc;
}

static ssize_t read_cb(struct bt_conn* conn, const struct bt_gatt_attr* attr, void* buf, uint16_t len, uint16_t offset)
{
    return 0;
}

static void ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    return;
}

#endif
