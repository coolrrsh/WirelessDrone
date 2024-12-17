#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

#define SPI_DEVICE   "/dev/spidev0.0" // SPI device file
#define SPI_SPEED    4000000          // SPI speed in Hz (4 MHz)

#define RST_PIN      17   // Reset pin (GPIO 17)
#define DC_PIN       22   // Data/Command pin (GPIO 22)
#define CHIP_NAME    "gpiochip0"      // GPIO chip device name

// File descriptor for SPI
int spi_fd;

// libgpiod GPIO line pointers
struct gpiod_chip *chip;
struct gpiod_line *rst_line;
struct gpiod_line *dc_line;

// Function to initialize GPIO lines using libgpiod
void initGPIO() {
    chip = gpiod_chip_open_by_name(CHIP_NAME);
    if (!chip) {
        perror("Failed to open GPIO chip");
        exit(EXIT_FAILURE);
    }

    // Get GPIO lines for RST and DC
    rst_line = gpiod_chip_get_line(chip, RST_PIN);
    dc_line = gpiod_chip_get_line(chip, DC_PIN);
    if (!rst_line || !dc_line) {
        perror("Failed to get GPIO lines");
        gpiod_chip_close(chip);
        exit(EXIT_FAILURE);
    }

    // Request RST and DC lines as outputs
    if (gpiod_line_request_output(rst_line, "nokia_rst", 0) < 0) {
        perror("Failed to request RST line as output");
        gpiod_chip_close(chip);
        exit(EXIT_FAILURE);
    }

    if (gpiod_line_request_output(dc_line, "nokia_dc", 0) < 0) {
        perror("Failed to request DC line as output");
        gpiod_line_release(rst_line);
        gpiod_chip_close(chip);
        exit(EXIT_FAILURE);
    }
}

// Function to write GPIO values
void gpioWrite(struct gpiod_line *line, int value) {
    if (gpiod_line_set_value(line, value) < 0) {
        perror("Failed to write GPIO value");
        exit(EXIT_FAILURE);
    }
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
    gpioWrite(dc_line, 0); // Command mode
    spiWriteByte(cmd);
}

void nokiaWriteData(uint8_t data) {
    gpioWrite(dc_line, 1); // Data mode
    spiWriteByte(data);
}

// Nokia 5110 initialization
void nokiaInit() {
    gpioWrite(rst_line, 0);
    usleep(10000); // 10ms delay
    gpioWrite(rst_line, 1);

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

// Cleanup GPIO and SPI
void cleanup() {
    gpiod_line_release(rst_line);
    gpiod_line_release(dc_line);
    gpiod_chip_close(chip);
    close(spi_fd);
}

// Main program
int main() {
    // Initialize GPIO and SPI
    initGPIO();
    if (initSPI() < 0) {
        fprintf(stderr, "SPI initialization failed.\n");
        cleanup();
        return 1;
    }

    // Initialize Nokia 5110
    nokiaInit();

    // Clear display
    nokiaClear();

    // Write some data (e.g., simple pattern or a basic message)
    for (int i = 65; i < 114; i++) {
        nokiaWriteData(i); // Alternating bits pattern
    }

    printf("Nokia 5110 initialized and data sent successfully.\n");

    // Cleanup resources
    cleanup();
    return 0;
}

