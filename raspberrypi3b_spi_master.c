#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <gpiod.h>

// SPI setup
#define SPI_CHANNEL    0     // SPI bus 0
#define SPI_SPEED      50000 // SPI speed (adjust as necessary)
#define SPI_MODE       0     // SPI Mode 0 (CPOL=0, CPHA=0)
#define SPI_BITS       8     // 8 bits per SPI transaction

// GPIO setup
#define CE_PIN         8   // CE0 pin (GPIO 8)

// Function to initialize SPI
int init_spi() {
    int spi_fd = open("/dev/spidev0.0", O_RDWR | O_SYNC);
    if (spi_fd < 0) {
        perror("SPI device open failed");
        return -1;
    }

    // Set SPI mode
    int spi_mode=0;
    int spi_bits=8;
    int spi_speed=50000;
    if (ioctl(spi_fd, SPI_IOC_WR_MODE, &spi_mode) == -1) {
        perror("Unable to set SPI mode");
        return -1;
    }

    // Set SPI bits per word
    if (ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bits) == -1) {
        perror("Unable to set bits per word");
        return -1;
    }

    // Set SPI speed
    if (ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) == -1) {
        perror("Unable to set SPI speed");
        return -1;
    }

    return spi_fd;
}

// Function to send data over SPI
int send_data(int spi_fd, struct gpiod_line *ce_pin, unsigned char data) {
    unsigned char tx_buffer[1] = {data};
    unsigned char rx_buffer[1];

    // Set CE0 low to start communication
    gpiod_line_set_value(ce_pin, 0);

    // Transfer data over SPI
    if (write(spi_fd, tx_buffer, 1) != 1) {
        perror("SPI write failed");
        return -1;
    }

    // Read response
    if (read(spi_fd, rx_buffer, 1) != 1) {
        perror("SPI read failed");
        return -1;
    }

    // Set CE0 high to end communication
    gpiod_line_set_value(ce_pin, 1);

    return rx_buffer[0];  // Return the received response
}

int main(void) {
    struct gpiod_chip *chip;
    struct gpiod_line *ce_pin;
    int spi_fd;
    unsigned char data_to_send = 0x39;

    // Initialize the gpiod chip and line for CE0 (GPIO 8)
    chip = gpiod_chip_open("/dev/gpiochip0");
    if (!chip) {
        perror("Failed to open GPIO chip");
        return 1;
    }

    ce_pin = gpiod_chip_get_line(chip, CE_PIN);
    if (!ce_pin) {
        perror("Failed to get GPIO line for CE0");
        gpiod_chip_close(chip);
        return 1;
    }

    // Request the line as output and set initial value to low (0)
    //if (gpiod_line_request_output(ce_pin, "SPI Communication", 0) < 0) {
    //    perror("Failed to request output on GPIO line");
    //    gpiod_chip_close(chip);
    //    return 1;
    //}
    

    // Initialize SPI
    spi_fd = init_spi();
    if (spi_fd < 0) {
        gpiod_chip_close(chip);
        return 1;
    }

    // Main loop
    while (1) {
        printf("Sending data: 0x%x\n", data_to_send);

        // Send data over SPI and receive the response
        unsigned char response = send_data(spi_fd, ce_pin, data_to_send);
        printf("Received response: 0x%x\n", response);

        sleep(1);  // Wait for 1 second before sending next data
    }

    // Clean up and close SPI and GPIO
    close(spi_fd);
    gpiod_chip_close(chip);

    return 0;
}


