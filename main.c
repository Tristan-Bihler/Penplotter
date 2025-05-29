#include <stdio.h>
#include <stdlib.h>

// Structure to represent the BMP header
typedef struct {
    unsigned short type;        // Magic identifier (BM)
    unsigned int size;          // File size in bytes
    unsigned short reserved1;   // Reserved
    unsigned short reserved2;   // Reserved
    unsigned int offset;        // Offset to image data in bytes
} BMPHeader;

// Structure to represent the DIB header (Information Header)
typedef struct {
    unsigned int headerSize;     // Size of this header in bytes
    int width;                   // Image width in pixels
    int height;                  // Image height in pixels
    unsigned short planes;       // Number of color planes (must be 1)
    unsigned short bitsPerPixel; // Bits per pixel (e.g., 24 for RGB)
    unsigned int compression;    // Compression type (0 for none)
    unsigned int imageSize;      // Image size in bytes (can be 0 for uncompressed)
    int xPixelsPerMeter;        // Horizontal pixels per meter
    int yPixelsPerMeter;        // Vertical pixels per meter
    unsigned int colorsUsed;     // Number of color indexes in the color table (0 if no color table)
    unsigned int importantColors; // Number of important color indexes (0 if all are important)
} DIBHeader;

int main() {
    const char *filename = "test.bmp"; // Replace with your BMP file name
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    BMPHeader bmpHeader;
    DIBHeader dibHeader;

    // Read the BMP header
    fread(&bmpHeader, sizeof(BMPHeader), 1, file);

    // Check if it's a BMP file
    if (bmpHeader.type != 0x4D42) { // "BM" in hexadecimal
        fprintf(stderr, "Not a valid BMP file.\n");
        fclose(file);
        return 1;
    }

    // Read the DIB header
    fread(&dibHeader, sizeof(DIBHeader), 1, file);

    int width = dibHeader.width;
    int height = dibHeader.height;
    unsigned short bitsPerPixel = dibHeader.bitsPerPixel;

    printf("Image Width: %d\n", width);
    printf("Image Height: %d\n", height);
    printf("Bits per Pixel: %d\n", bitsPerPixel);

    // Calculate the number of bytes per pixel
    int bytesPerPixel = bitsPerPixel / 8;

    // Calculate the row padding (BMP rows are padded to be a multiple of 4 bytes)
    int rowPadding = (4 - (width * bytesPerPixel) % 4) % 4;

    // Allocate memory for the image array
    unsigned char *imageArray = (unsigned char *)malloc(height * width * bytesPerPixel);
    if (imageArray == NULL) {
        perror("Error allocating memory for image array");
        fclose(file);
        return 1;
    }

    // Read the image data
    fseek(file, bmpHeader.offset, SEEK_SET); // Move to the start of the image data
    for (int i = 0; i < height; i++) {
        fread(imageArray + i * width * bytesPerPixel, bytesPerPixel, width, file);
        // Skip the row padding
        fseek(file, rowPadding, SEEK_CUR);
    }

    fclose(file);

    // Now the image data is stored in the 'imageArray'
    // You can access individual pixel components like this (assuming 24-bit RGB):
    // Pixel at (x, y) where x is the column (0 to width-1) and y is the row (0 to height-1)
    // Red component:   imageArray[(y * width + x) * 3 + 2]
    // Green component: imageArray[(y * width + x) * 3 + 1]
    // Blue component:  imageArray[(y * width + x) * 3 + 0]

    // Example: Print the RGB values of the first 10 pixels (if it's a 24-bit image)
    if (bitsPerPixel == 24 && width >= 10 && height >= 1) {
        printf("\nFirst 10 pixels (RGB):\n");
        for (int i = 0; i < 10; i++) {
            unsigned char blue = imageArray[i * 3 + 0];
            unsigned char green = imageArray[i * 3 + 1];
            unsigned char red = imageArray[i * 3 + 2];
            printf("Pixel %d: R=%u, G=%u, B=%u\n", i, red, green, blue);
        }
    }

    // Remember to free the allocated memory when you're done
    free(imageArray);

    return 0;
}