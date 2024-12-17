#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define PWM_BASE "/sys/class/pwm"
#define PWM_CHIP "pwmchip0"

#define PWM0 0  // GPIO for Motor 1
#define PWM1 1  // GPIO for Motor 2
#define PWM2 2  // GPIO for Motor 3
#define PWM3 3  // GPIO for Motor 4

#define MIN_THROTTLE 1000000
#define MAX_THROTTLE 2000000
#define STEP 50000

// Global throttle values
int throttle_m1 = MIN_THROTTLE, throttle_m2 = MIN_THROTTLE;
int throttle_m3 = MIN_THROTTLE, throttle_m4 = MIN_THROTTLE;

// PWM Functions
void pwmInit(int pwm) {
    char path[64];
    snprintf(path, sizeof(path), "%s/%s/export", PWM_BASE, PWM_CHIP);
    int fd = open(path, O_WRONLY);
    dprintf(fd, "%d", pwm);
    close(fd);

    snprintf(path, sizeof(path), "%s/%s/pwm%d/period", PWM_BASE, PWM_CHIP, pwm);
    fd = open(path, O_WRONLY);
    dprintf(fd, "%d", 20000000); // 20ms period
    close(fd);

    snprintf(path, sizeof(path), "%s/%s/pwm%d/enable", PWM_BASE, PWM_CHIP, pwm);
    fd = open(path, O_WRONLY);
    dprintf(fd, "%d", 1);
    close(fd);
}

void pwmSetDutyCycle(int pwm, int duty_us) {
    char path[64];
    snprintf(path, sizeof(path), "%s/%s/pwm%d/duty_cycle", PWM_BASE, PWM_CHIP, pwm);
    int fd = open(path, O_WRONLY);
    dprintf(fd, "%d", duty_us);
    close(fd);
}

void processCommand(const char *command) {
    if (strcmp(command, "up") == 0) {
        throttle_m1 += STEP; throttle_m2 += STEP;
        throttle_m3 += STEP; throttle_m4 += STEP;
    } else if (strcmp(command, "down") == 0) {
        throttle_m1 -= STEP; throttle_m2 -= STEP;
        throttle_m3 -= STEP; throttle_m4 -= STEP;
    } else if (strcmp(command, "left") == 0) {
        throttle_m1 -= STEP; throttle_m3 -= STEP;
        throttle_m2 += STEP; throttle_m4 += STEP;
        sleep(1); // Simulate delay for smooth movement
    } else if (strcmp(command, "right") == 0) {
        throttle_m1 += STEP; throttle_m3 += STEP;
        throttle_m2 -= STEP; throttle_m4 -= STEP;
        sleep(1); // Simulate delay for smooth movement
    }

    // Clamp values
    throttle_m1 = throttle_m1 > MAX_THROTTLE ? MAX_THROTTLE : throttle_m1 < MIN_THROTTLE ? MIN_THROTTLE : throttle_m1;
    throttle_m2 = throttle_m2 > MAX_THROTTLE ? MAX_THROTTLE : throttle_m2 < MIN_THROTTLE ? MIN_THROTTLE : throttle_m2;
    throttle_m3 = throttle_m3 > MAX_THROTTLE ? MAX_THROTTLE : throttle_m3 < MIN_THROTTLE ? MIN_THROTTLE : throttle_m3;
    throttle_m4 = throttle_m4 > MAX_THROTTLE ? MAX_THROTTLE : throttle_m4 < MIN_THROTTLE ? MIN_THROTTLE : throttle_m4;

    // Apply to motors
    pwmSetDutyCycle(PWM0, throttle_m1);
    pwmSetDutyCycle(PWM1, throttle_m2);
    pwmSetDutyCycle(PWM2, throttle_m3);
    pwmSetDutyCycle(PWM3, throttle_m4);
    printf("Command: %s -> M1:%d, M2:%d, M3:%d, M4:%d\n", command, throttle_m1, throttle_m2, throttle_m3, throttle_m4);
}

static int callback_server(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE:
            printf("Received command: %s\n", (char *)in);
            processCommand((char *)in);
            break;
        default:
            break;
    }
    return 0;
}

static struct lws_protocols protocols[] = {
    { "motor-control-protocol", callback_server, 0, 128 },
    { NULL, NULL, 0, 0 }
};

int main() {
    struct lws_context_creation_info info = {0};
    info.port = 8000;
    info.protocols = protocols;

    struct lws_context *context = lws_create_context(&info);
    if (!context) {
        printf("Failed to create WebSocket context\n");
        return -1;
    }

    // Initialize PWM
    pwmInit(PWM0); pwmInit(PWM1);
    pwmInit(PWM2); pwmInit(PWM3);

    printf("WebSocket server listening on ws://localhost:8000\n");

    while (1) {
        lws_service(context, 1000);
    }

    lws_context_destroy(context);
    return 0;
}

