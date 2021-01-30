#include "mbed.h"
#include "stm32l475e_iot01_tsensor.h"
#include "stm32l475e_iot01_hsensor.h"
#include "stm32l475e_iot01_psensor.h"
#include "stm32l475e_iot01_magneto.h"
#include "stm32l475e_iot01_gyro.h"
#include "stm32l475e_iot01_accelero.h"

Timer debounce;
InterruptIn btnRecord(USER_BUTTON);
// InterruptIn btnFlag(SW3);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

int16_t pDataXYZ[3] = {0};
int idR[32] = {0};
int indexR = 0;

void record(void) {
    BSP_ACCELERO_AccGetXYZ(pDataXYZ);
    printf("\nACCELERO_X = %d\n", pDataXYZ[0]);
    printf("ACCELERO_Y = %d\n", pDataXYZ[1]);
    printf("ACCELERO_Z = %d\n", pDataXYZ[2]);
}

void startRecord(void) {
    printf("---start---\n");
    idR[indexR++] = queue.call_every(1, record);
    indexR = indexR % 32;
}

void stopRecord(void) {
    printf("---stop---\n");
    for (auto &i : idR)
        queue.cancel(i);
}

void flagWrong(void) {printf("---delete---\n");}


int main() {
    printf("Start accelerometer init\n");
    BSP_ACCELERO_Init();
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    btnRecord.fall(queue.event(startRecord));
    // btnFlag.fall(queue.event(flagWrong));
    btnRecord.rise(queue.event(stopRecord));
}