#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <task.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;
    int filter_data[5];
    int filter_index = 0;
    int sum = 0;
    int average = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!

            sum -= filter_data[filter_index];
            filter_data[filter_index] = data;
            sum += data;
            average = sum / 5;
            filter_index = (filter_index + 1) % 5;

            printf("Average: %d\n", average);

            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
