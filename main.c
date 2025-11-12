
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "inc/main.h"
#include <string.h>
#include <hardware/i2c.h>
#include <hardware/spi.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h>
#include <pico/binary_info.h>
#include "inc/comms.h"
#include "inc/ModbusComms.h"
#include "inc/DynamentComms.h"
#include <hardware/pio_instructions.h>
#include <hardware/gpio.h>
#include <time.h>


/* Private typedef */
#define GAS_READING_MEASURAND   30057
#define POLL_COUNT              2

/* Private variables ---------------------------------------------------------*/
struct repeating_timer timer_heartbeat;
int pollCounter = POLL_COUNT;

/* Function declarations */
void RequestGasReading();
void ReadingReceived(int status, float value);
void DualReadingReceived(int status, float reading1, float reading2);



static bool timer_heartbeat_isr(struct repeating_timer *t)
{
    if (cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN)) {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    } else {
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }

    if (pollCounter > 0) {
        --pollCounter;
        if (pollCounter <= 0) {
            RequestGasReading();
            pollCounter = POLL_COUNT;
        }
    }
    return true;
}

int main() {
    stdio_init_all();

    watchdog_enable(WATCHDOG_MILLISECONDS, 1);

    uart_init(UART_SEN, UART_BAUD_RATE);
    gpio_set_function(UART_SEN_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_SEN_TX_PIN, GPIO_FUNC_UART);

    uart_init(ESP_UART_ID, ESP_BAUD_RATE);
    gpio_set_function(ESP_E_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(ESP_E_TX_PIN, GPIO_FUNC_UART);

    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }


    add_repeating_timer_ms(1000, timer_heartbeat_isr, NULL, &timer_heartbeat);

    initialise_comms();
    if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL)
        InitialiseDynamentComms();

    for (;;) {
        Watchdog();
        if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL)
            DynamentCommsHandler();
        else
            ModbusCommsHandler();

        cyw43_arch_poll(); // keep network stack running
    }

    return 0;
}

void Watchdog(void)
{
    watchdog_update();
}

void RequestGasReading()
{
    printf("RequestGasReading called\n");
    if (COMMS_PROTOCOL == DYNAMENT_PROTOCOL) {
        printf("Using Dynament_Protocol\n");
        RequestLiveData2(DualReadingReceived);
    } else {
        printf("Using another protocol\n");
        ReadMeasurand(GAS_READING_MEASURAND, ReadingReceived);
    }
}

void ReadingReceived(int status, float value)
{
    if (status == READ_RESPONSE_VALUE_VALID) {
        // Handle valid reading
    } else {
        // Handle invalid/timed-out reading
    }
}

void DualReadingReceived(int status, float reading1, float reading2)
{
    if (status == READ_RESPONSE_VALUE_VALID) {
        printf("--------- Callback Executed: Status = %d, **** \033[93mGas1 = %f\033[0m, Gas2 = %f\n", status, reading1, reading2);
        uart_write_blocking(ESP_UART_ID, (const uint8_t*)&reading1, sizeof(reading1));
        printf("Sent Successfully to ESP32\n");
    } else {
        printf("Callback Executed: Status = %d (Invalid)\n", status);
    }
}
