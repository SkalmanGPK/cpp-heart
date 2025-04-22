#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>  // for sleep_for
#include <chrono>  // for milliseconds
#include <conio.h> // for _kbhit() and _getch() on Windows

const int ROWS = 8;
const int COLS = 12;

// Convert 3x uint32_t into a flat 96-bit vector (MSB to LSB)
std::vector<uint8_t> unpackBits(uint32_t p0, uint32_t p1, uint32_t p2) {
    std::vector<uint8_t> bits(96);
    uint32_t parts[3] = {p0, p1, p2};

    for (int i = 0; i < 3; ++i) {
        for (int b = 0; b < 32; ++b) {
            int index = i * 32 + b;
            bits[index] = (parts[i] >> (31 - b)) & 1;
        }
    }
    return bits;
}

// Convert flat 96-bit vector back into 3x uint32_t (MSB to LSB)
void packBits(const std::vector<uint8_t>& bits, uint32_t& p0, uint32_t& p1, uint32_t& p2) {
    p0 = p1 = p2 = 0;
    for (int i = 0; i < 96; ++i) {
        if (bits[i]) {
            if (i < 32)
                p0 |= (1 << (31 - i));
            else if (i < 64)
                p1 |= (1 << (63 - i));
            else
                p2 |= (1 << (95 - i));
        }
    }
}

// Row-wise offset tracking (like shiftMatrixRight)
int offsets[ROWS] = {0};

// Increment each row's offset by 1
void shiftMatrixRight() {
    for (int y = 0; y < ROWS; y++) {
        offsets[y] = (offsets[y] + 1) % COLS;
    }
}
void shiftMatrixLeft() {
    for (int y = 0; y < ROWS; y++) {
        // Add COLS to avoid negative values, then take modulo
        offsets[y] = (offsets[y] - 1 + COLS) % COLS;
    }
}

// Draw shifted matrix
void printMatrix(const std::vector<uint8_t>& bits) {
    std::cout << "\nMatrix:\n";
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            int originalX = (x - offsets[y] + COLS) % COLS;
            int bitIndex = y * COLS + originalX;
            std::cout << (bits[bitIndex] ? " * " : "   ");
        }
        std::cout << "\n";
    }
}

int main() {
    uint32_t part0 = 0x00000005;
    uint32_t part1 = 0x00A80880;
    uint32_t part2 = 0x50020000;

    std::vector<uint8_t> bits = unpackBits(part0, part1, part2);

    // Initial matrix
    std::cout << "Original matrix:\n";
    printMatrix(bits);

    // Shift and redraw 10 times
    std::string userInput;
    while (true) {
        // Check if a key was pressed
        if (_kbhit()) {
            char ch = _getch();  // get character (non-blocking)
            if (ch == 's' || ch == 'S') {
                std::cout << "\nUser stopped the animation.\n";
                break;
            }
        }
        system("cls");
        shiftMatrixLeft();
        std::cout << "\nAfter shift:\n";
        printMatrix(bits);

        std::this_thread::sleep_for(std::chrono::milliseconds(300)); // 100ms delay
    }

    // Pack back into integers (optional)
    packBits(bits, part0, part1, part2);
    std::cout << "\nNew uint32_t values:\n";
    std::cout << "part0: 0x" << std::hex << part0 << "\n";
    std::cout << "part1: 0x" << std::hex << part1 << "\n";
    std::cout << "part2: 0x" << std::hex << part2 << "\n";

    std::cout << "\nPress Enter to exit...";
    std::cin.get();
    return 0;
}
