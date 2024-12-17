#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE   "/dev/spidev0.0" // SPI device file
#define SPI_SPEED    4000000          // SPI speed in Hz (4 MHz)
#define GPIO_BASE    "/sys/class/gpio"

// GPIO pins
#define RST_PIN      17   // Reset pin (GPIO 17)
#define DC_PIN       22   // Data/Command pin (GPIO 22)

// File descriptor for SPI
int spi_fd;

// Function to export GPIO pins
void gpioExport(int pin) {
    char buffer[64];
    int fd = open(GPIO_BASE "/export", O_WRONLY);
    if (fd < 0) {
        perror("gpio/export");
        exit(1);
    }
    snprintf(buffer, sizeof(buffer), "%d", pin);
    write(fd, buffer, strlen(buffer));
    close(fd);
}

// Function to set GPIO direction
void gpioDirection(int pin, const char *direction) {
    char path[64];
    snprintf(path, sizeof(path), GPIO_BASE "/gpio%d/direction", pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("gpio/direction");
        exit(1);
    }
    write(fd, direction, strlen(direction));
    close(fd);
}

// Function to write value to GPIO
void gpioWrite(int pin, int value) {
    char path[64];
    snprintf(path, sizeof(path), GPIO_BASE "/gpio%d/value", pin);
    int fd = open(path, O_WRONLY);
    if (fd < 0) {
        perror("gpio/write");
        exit(1);
    }
    char val = value ? '1' : '0';
    write(fd, &val, 1);
    close(fd);
}

// SPI initialization
int initSPI() {
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    spi_fd = open(SPI_DEVICE, O_RDWR);
    if (spi_fd < 0) {
        perror("Failed to open SPI device");
        return -1;
    }

    // Set SPI mode
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &mode) == -1) {
        perror("Failed to set SPI mode");
        return -1;
    }

    // Set bits per word
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        perror("Failed to set bits per word");
        return -1;
    }

    // Set max speed
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        perror("Failed to set SPI speed");
        return -1;
    }

    return 0;
}

// Function to send a byte over SPI
void spiWriteByte(uint8_t data) {
    if (write(spi_fd, &data, 1) != 1) {
        perror("SPI write failed");
    }
}

// Nokia 5110 commands and data
void nokiaWriteCommand(uint8_t cmd) {
    gpioWrite(DC_PIN, 0); // Command mode
    spiWriteByte(cmd);
}

void nokiaWriteData(uint8_t data) {
    gpioWrite(DC_PIN, 1); // Data mode
    spiWriteByte(data);
}

// Nokia 5110 initialization
void nokiaInit() {
    gpioWrite(RST_PIN, 0);
    usleep(10000); // 10ms delay
    gpioWrite(RST_PIN, 1);

    // Send initialization commands
    nokiaWriteCommand(0x21); // Extended commands
    nokiaWriteCommand(0xB1); // Set Vop (contrast)
    nokiaWriteCommand(0x04); // Set temp coefficient
    nokiaWriteCommand(0x14); // Bias mode 1:48
    nokiaWriteCommand(0x20); // Basic commands
    nokiaWriteCommand(0x0C); // Normal display mode
}

// Nokia 5110 clear display
void nokiaClear() {
    for (int i = 0; i < 6 * 84; i++) { // 6 rows, 84 columns
        nokiaWriteData(0x00);
    }
}

// Main program
int main() {
    // Export and configure GPIO pins
    gpioExport(RST_PIN);
    gpioExport(DC_PIN);
    gpioDirection(RST_PIN, "out");
    gpioDirection(DC_PIN, "out");

    // Initialize SPI
    if (initSPI() < 0) {
        fprintf(stderr, "SPI initialization failed.\n");
        return 1;
    }

    // Initialize Nokia 5110
    nokiaInit();

    // Clear display
    nokiaClear();

    // Write some data (e.g., simple pattern or a basic message)
    for (int i = 0; i < 84; i++) {
        nokiaWriteData(0xAA); // Alternating bits pattern
    }

    printf("Nokia 5110 initialized and data sent successfully.\n");

    // Close SPI
    close(spi_fd);

    return 0;
}

