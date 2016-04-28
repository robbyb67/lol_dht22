#include <jni.h>
#include <stdio.h>
#include <wiringPi.h>

#include "dht22.h"
#include "jnicall.h"
#include "locking.h"
#include "dht22.h"

JNIEXPORT jdoubleArray JNICALL Java_de_bausdorf_smarthome_mqttgpio_sensor_impl_Dht22Am2302TemperatureAndHumiditySensor_executeNative(
                JNIEnv *env, jobject thisObject, jint pin) {

        float temp, humi;
        jdoubleArray result;

        int callResult = 0;
        int lockfd;
        int tries = 100;

        lockfd = open_lockfile(LOCKFILE);

        if (wiringPiSetup () == -1)
          return NULL ;

        if (setuid(getuid()) < 0)
        {
          perror("Dropping privileges failed\n");
          return NULL ;
        }

        float temp, humi;
        for (int attempt = 1; callResult != 1 && attempt<=10; attempt++) {
                if (attempt > 1) {
                        sleep_milliseconds(500);
                }
                callResult = read_dht22_dat(&temp, &humi, pin);
        }

        delay(1500);
        close_lockfile(lockfd);

        if (callResult == 1) {
                double cArray[2] = { temperature, humidity };
                result = (*env)->NewDoubleArray(env, 2);
                (*env)->SetDoubleArrayRegion(env, result, 0, 2, cArray);
                return result;
        } else {
                printf("Error reading value from sensor, error code: %d", callResult);
        }
        return NULL;
}
