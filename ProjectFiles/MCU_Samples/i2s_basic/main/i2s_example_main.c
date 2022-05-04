//LCD includes
#include "common.h"
#include "i2s_parallel.h"
#include "gfx3d.h"
#include "cct.h"

//SPP includes
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "time.h"
#include "sys/time.h"

#define BLUETOOTH 0
#define LCD 1
#define MFRC 0
#define CARD_SIM 0
#define MFRC_CHECK_CONT 0

#define SPP_TAG "OddsBooster"
#define SPP_SERVER_NAME "SPP_SERVER"
#define EXAMPLE_DEVICE_NAME "OddsBooster"
#define SPP_SHOW_SPEED 1

#define GPIO_IRQ 13
#define MESS_LEN 13

#define MFRC_IRQ 13
#define MFRC_ALE 27
#define MFRC_NWR 14
#define MFRC_NRD 12
#define MFRC_D0 23
#define MFRC_D1 22
#define MFRC_D2 21
#define MFRC_D3 19
#define MFRC_D4 18
#define MFRC_D5 5
#define MFRC_D6 4
#define MFRC_D7 15

#define MFRC_PAGE_REG           0x00
#define MFRC_CMD_REG            0x01
#define MFRC_FIFO_DATA_REG      0x02
#define MFRC_FIFO_LENGTH_REG    0x04
#define MFRC_IRG_EN_REG         0x06
#define MFRC_IRQ_RQ_REG         0x07
#define MFRC_CTRL_REG           0x09
#define MFRC_ERROR_REG          0x0A

#define MFRC_RECV_CMD           0x16

#define TOTAL_SIMS              5

#if LCD

uint8_t FrameBuffer[FRAME_SIZE];
//uint8_t* FrameBuffer;

#endif

#if BLUETOOTH

static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;

static struct timeval time_new, time_old;
static long data_num = 0;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

typedef struct {
    uint8_t num;
    uint8_t suit;
} card_t;

//static QueueHandle_t gpio_evt_queue = NULL;

//holds previous card value
static uint8_t nums[13] = {'a', '2', '3', '4', '5', '6', '7', '8', '9', 't', 'j', 'q', 'k'};
static uint8_t suits[4] = {'h', 'd', 's', 'c'};

int new_flag = 0;
int sim_idx = 0;
card_t prev_card = {'x', 'x'};
card_t deck[52];
int used[52];
char message[13] = "Card ID: 00\n";

void reset_deck(){
    for(int n = 0; n < 13; n++){
        for (int s = 0; s < 4; s++){
            deck[(n*4)+s].num = nums[n];
            deck[(n*4)+s].suit = suits[s];
            used[(n*4)+s] = 1;
        }
    }
}

//function called to send response on received bluetooth packet
void write_response(esp_spp_cb_param_t* param){
    if(param->data_ind.data[0] == 'r'){
        reset_deck();
        return;
    }
    if(new_flag){
        ESP_LOGI(SPP_TAG, "Card: %c%c", prev_card.num, prev_card.suit);
        message[9] = prev_card.num;
        message[10] = prev_card.suit;
        new_flag = 0;
    }
    else{
        message[9] = 'x';
        message[10] = 'x';
    }
    esp_spp_write(param->data_ind.handle, MESS_LEN, (uint8_t*)&message);

#if CARD_SIM
    int rand_id = rand() % 52;
    while(used[rand_id] == 0){
        rand_id = (rand_id+1)%52;
    }
    prev_card.num = deck[rand_id].num;
    prev_card.suit = deck[rand_id].suit;
    new_flag = 1;
#endif
}

//print the speed of the bluetooth transmission
static void print_speed(void){
    float time_old_s = time_old.tv_sec + time_old.tv_usec / 1000000.0;
    float time_new_s = time_new.tv_sec + time_new.tv_usec / 1000000.0;
    float time_interval = time_new_s - time_old_s;
    float speed = data_num * 8 / time_interval / 1000.0;
    ESP_LOGI(SPP_TAG, "speed(%fs ~ %fs): %f kbit/s" , time_old_s, time_new_s, speed);
    data_num = 0;
    time_old.tv_sec = time_new.tv_sec;
    time_old.tv_usec = time_new.tv_usec;
}

//function controlling serial port bluetooth communication event handling
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
    switch (event) {
    case ESP_SPP_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT");
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        esp_bt_dev_set_device_name(EXAMPLE_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len=%d handle=%d",
                 param->data_ind.len, param->data_ind.handle);
        esp_log_buffer_hex("",param->data_ind.data,param->data_ind.len);
        write_response(param);
#if SPP_SHOW_SPEED
        gettimeofday(&time_new, NULL);
        data_num += param->data_ind.len;
        if (time_new.tv_sec - time_old.tv_sec >= 3) {
            print_speed();
        }
#endif
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT");
        break;
    case ESP_SPP_SRV_OPEN_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
        gettimeofday(&time_old, NULL);
        break;
    case ESP_SPP_SRV_STOP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT");
        break;
    case ESP_SPP_UNINIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT");
        break;
    default:
        break;
    }
}

//advertisement functionality for bluetooth
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param){
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d", param->mode_chg.mode);
        break;

    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        break;
    }
    }
    return;
}

/*
static void IRAM_ATTR gpio_isr_handler(void* arg){
    uint32_t gpio_num = (uint32_t) 0;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

//function that happens on an interrupt
static void gpio_task_example(void* arg){
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(SPP_TAG, "ESP_SPP_GPIO_IRQ_EVT");
            new_flag = 1;
            int i;
            for(i = 0; i<6; i++){
                card_in = card_in | (0xff & (((uint8_t)gpio_get_level(gpio_card_num[i])) << i));
            }
        }
    }
}
*/

#endif

#if LCD

//setup the LCD
void lcd_config() {
    i2s_parallel_buffer_desc_t bufdesc;
    i2s_parallel_config_t cfg = {
        .gpio_bus = {
                    32, 	// 0 : b0 or all 
					-1, 	// 1 : g0
					-1, 	// 2 : r0
					26, 	// 3 : de
					33, 	// 4 : HS
					25, 	// 5 : VS
					-1,		// 6 : unused
					-1},	// 7 : unused
        .gpio_clk = 3,	// XCK (I want to use 3)

        .bits = I2S_PARALLEL_BITS_8,
        .clkspeed_hz = 16*1000*1000,//resulting pixel clock = 6.5MHz
        .buf = &bufdesc
    };

    bufdesc.memory = FrameBuffer;
    bufdesc.size = FRAME_SIZE;
	// make sure buffer has encoded syncs
	CNFGClearScreen(0, FrameBuffer);

   // this lcd power up sequence must be followed to avoid damage
	delayMs(50);  // allow supply voltage to stabilize
    i2s_parallel_setup(&I2S1, &cfg);
	// enable lcd bias voltage V0 after clocks are available
	delayMs(50);
}

//task controlling the LCD (thread)
void lcdTask(void *pvParameters) {
	//allocate the space for the frame buffer
	//FrameBuffer = malloc(FRAME_SIZE);
   	lcd_config();
	/*
	CNFGClearScreen(0xff, FrameBuffer);
	delayMs(1000);
	CNFGDrawText(1, 10, 10, false, FrameBuffer);
	CNFGDrawText(17, 18, 10, false, FrameBuffer);
	ESP_LOGI(SPP_TAG, "Write Text");
	*/

	while(1) {
		CNFGClearScreen(0xff, FrameBuffer);
		delayMs(1000);
		CNFGClearScreen(0x00, FrameBuffer);
		delayMs(1000);
		ESP_LOGI(SPP_TAG, "Val1: %d\n", FrameBuffer[0]);
		ESP_LOGI(SPP_TAG, "Val2: %d\n", FrameBuffer[1000]);
	}
	//I'm not sure how you would ever get here to do this
	//free(FrameBuffer);
}

#endif

#if MFRC

void mfrc_write(uint8_t address, uint8_t data){

	gpio_set_direction(MFRC_D7, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D6, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D5, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D4, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D3, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D2, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D1, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D0, GPIO_MODE_OUTPUT);

	//set address
	gpio_set_level(MFRC_D7, 0);
	gpio_set_level(MFRC_D6, 0);
	gpio_set_level(MFRC_D5, (address & 0x20)>>5);
	gpio_set_level(MFRC_D4, (address & 0x10)>>4);
	gpio_set_level(MFRC_D3, (address & 0x08)>>3);
	gpio_set_level(MFRC_D2, (address & 0x04)>>2);
	gpio_set_level(MFRC_D1, (address & 0x02)>>1);
	gpio_set_level(MFRC_D0, (address & 0x01)>>0);
	gpio_set_level(MFRC_ALE, 1);
	delayMs(1);
	gpio_set_level(MFRC_ALE, 0);

	delayMs(1);

	//write
	gpio_set_level(MFRC_D7, (data & 0x80)>>7);
	gpio_set_level(MFRC_D6, (data & 0x40)>>6);
	gpio_set_level(MFRC_D5, (data & 0x20)>>5);
	gpio_set_level(MFRC_D4, (data & 0x10)>>4);
	gpio_set_level(MFRC_D3, (data & 0x08)>>3);
	gpio_set_level(MFRC_D2, (data & 0x04)>>2);
	gpio_set_level(MFRC_D1, (data & 0x02)>>1);
	gpio_set_level(MFRC_D0, (data & 0x01)>>0);
	gpio_set_level(MFRC_NWR, 0);
	delayMs(1);
	gpio_set_level(MFRC_NWR, 1);
}

uint8_t mfrc_read(uint8_t address){
	
	gpio_set_direction(MFRC_D7, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D6, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D5, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D4, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D3, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D2, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D1, GPIO_MODE_OUTPUT);
	gpio_set_direction(MFRC_D0, GPIO_MODE_OUTPUT);

	//set address
	gpio_set_level(MFRC_D7, 0);
	gpio_set_level(MFRC_D6, 0);
	gpio_set_level(MFRC_D5, (address & 0x20)>>5);
	gpio_set_level(MFRC_D4, (address & 0x10)>>4);
	gpio_set_level(MFRC_D3, (address & 0x08)>>3);
	gpio_set_level(MFRC_D2, (address & 0x04)>>2);
	gpio_set_level(MFRC_D1, (address & 0x02)>>1);
	gpio_set_level(MFRC_D0, (address & 0x01)>>0);
	gpio_set_level(MFRC_ALE, 1);
	delayMs(1);
	gpio_set_level(MFRC_ALE, 0);

	delayMs(1);

	gpio_set_direction(MFRC_D7, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D6, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D5, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D4, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D3, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D2, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D1, GPIO_MODE_INPUT);
	gpio_set_direction(MFRC_D0, GPIO_MODE_INPUT);

	//read
	uint8_t data = 0x00;
	gpio_set_level(MFRC_NRD, 0);
	delayMs(1);
	data |= (gpio_get_level(MFRC_D7) << 7);
	data |= (gpio_get_level(MFRC_D6) << 6);
	data |= (gpio_get_level(MFRC_D5) << 5);
	data |= (gpio_get_level(MFRC_D4) << 4);
	data |= (gpio_get_level(MFRC_D3) << 3);
	data |= (gpio_get_level(MFRC_D2) << 2);
	data |= (gpio_get_level(MFRC_D1) << 1);
	data |= (gpio_get_level(MFRC_D0) << 0);
	gpio_set_level(MFRC_NRD, 1);

	return data;
}

#endif	
	
void app_main(){

#if LCD

	//start the LCD on the other CPU core
	xTaskCreatePinnedToCore(&lcdTask, "lcdTask", 4096, NULL, 20, NULL, 1);

#endif

#if BLUETOOTH
    //setup the card deck
    reset_deck();

	//bluetooth setup and initialization
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

    /*
    //set handling method for the interrupts
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);
    //install gpio isr service
    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_IRQ, gpio_isr_handler, NULL);
    */

    //setup all the bluetooth stuff
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }

#if (CONFIG_BT_SSP_ENABLED == true)
    /* Set default parameters for Secure Simple Pairing */
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);

#endif

#if MFRC

	//pin init
	gpio_config_t irq_config = {
        .intr_type = GPIO_INTR_HIGH_LEVEL,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL<< MFRC_IRQ),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&irq_config);
	gpio_config_t ale_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_ALE),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&ale_config);
	gpio_config_t nwr_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_NWR),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&nwr_config);
	gpio_config_t nrd_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_NRD),
        .pull_down_en = 0,
        .pull_up_en = 1,
    };
    gpio_config(&nrd_config);
    gpio_config_t d0_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D0),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d0_config);
    gpio_config_t d1_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D1),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d1_config);
    gpio_config_t d2_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D2),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d2_config);
    gpio_config_t d3_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D3),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d3_config);
    gpio_config_t d4_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D4),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d4_config);
    gpio_config_t d5_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D5),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d5_config);
	gpio_config_t d6_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D6),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d6_config);
	gpio_config_t d7_config = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL<< MFRC_D7),
        .pull_down_en = 1,
        .pull_up_en = 0,
    };
    gpio_config(&d7_config);

	gpio_set_level(MFRC_ALE, 0);
	gpio_set_level(MFRC_NRD, 1);
	gpio_set_level(MFRC_NWR, 1);

	//wait for initialization
	uint8_t cmd_reg_val = 0xff;
    cmd_reg_val = mfrc_read(MFRC_CMD_REG);
    ESP_LOGI(SPP_TAG, "CMD Reg Val: %x", cmd_reg_val);
    cmd_reg_val = mfrc_read(MFRC_PAGE_REG);
    ESP_LOGI(SPP_TAG, "Page Reg Val: %x", cmd_reg_val);
    delayMs(1000);

#if MFRC_CHECK_CONT

	while(cmd_reg_val != 0x00){
		//read the cmd reg
		cmd_reg_val = mfrc_read(MFRC_CMD_REG);
        ESP_LOGI(SPP_TAG, "CMD Reg Val: %x", cmd_reg_val);
		delayMs(1000);
	}

    ESP_LOGI(SPP_TAG, "Initialization Step 1 Success :) \n");

    //reset the microprocessor initialization (this seems like it would just enable paging)
    mfrc_write(MFRC_PAGE_REG, 0x80);
    cmd_reg_val = 0xff;
	while(cmd_reg_val != 0x00){
		//read the cmd reg
		cmd_reg_val = mfrc_read(MFRC_CMD_REG);
		delayMs(1000);
	}

	ESP_LOGI(SPP_TAG, "Initialization Step 2 Success :) \n");

    //set the page resiter so addressing to linear mode
    mfrc_write(MFRC_PAGE_REG, 0x00);

#endif

#endif

   }
