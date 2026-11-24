#include <stdint.h>

// --- Definisi Alamat Register (STM32F411 Reference Manual) ---
#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30UL))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40UL))

#define GPIOA_BASE      0x40020000UL
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00UL))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14UL))

#define TIM2_BASE       0x40000000UL
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00UL))
#define TIM2_DIER       (*(volatile uint32_t *)(TIM2_BASE + 0x0CUL))
#define TIM2_SR         (*(volatile uint32_t *)(TIM2_BASE + 0x10UL))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28UL))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2CUL))

#define TIM3_BASE       0x40000400UL
#define TIM3_CR1        (*(volatile uint32_t *)(TIM3_BASE + 0x00UL))
#define TIM3_DIER       (*(volatile uint32_t *)(TIM3_BASE + 0x0CUL))
#define TIM3_SR         (*(volatile uint32_t *)(TIM3_BASE + 0x10UL))
#define TIM3_PSC        (*(volatile uint32_t *)(TIM3_BASE + 0x28UL))
#define TIM3_ARR        (*(volatile uint32_t *)(TIM3_BASE + 0x2CUL))

#define TIM4_BASE       0x40000800UL
#define TIM4_CR1        (*(volatile uint32_t *)(TIM4_BASE + 0x00UL))
#define TIM4_DIER       (*(volatile uint32_t *)(TIM4_BASE + 0x0CUL))
#define TIM4_SR         (*(volatile uint32_t *)(TIM4_BASE + 0x10UL))
#define TIM4_PSC        (*(volatile uint32_t *)(TIM4_BASE + 0x28UL))
#define TIM4_ARR        (*(volatile uint32_t *)(TIM4_BASE + 0x2CUL))

#define NVIC_ISER0      (*(volatile uint32_t *)0xE000E100UL)

// Dipanggil oleh startup_stm32f411ceux.s sebelum main()
void SystemInit(void) {}

int main(void) {
    // 1. Aktifkan clock GPIOA (bit 0)
    RCC_AHB1ENR |= (1U << 0);

    // 2. Set PA3, PA4, PA5 sebagai Output (01)
    GPIOA_MODER &= ~((3U << (3 * 2)) | (3U << (4 * 2)) | (3U << (5 * 2)));
    GPIOA_MODER |=  ((1U << (3 * 2)) | (1U << (4 * 2)) | (1U << (5 * 2)));

    // 3. Aktifkan clock TIM2 (bit 0), TIM3 (bit 1), TIM4 (bit 2)
    RCC_APB1ENR |= (1U << 0) | (1U << 1) | (1U << 2);

    // Konfigurasi TIM2 -> Blink 1 Hz (Toggle tiap 500 ms)
    TIM2_PSC   = 15999;
    TIM2_ARR   = 499;
    TIM2_DIER |= (1U << 0);       // UIE
    TIM2_CR1  |= (1U << 0);       // CEN

    // Konfigurasi TIM3 -> Blink 2 Hz (Toggle tiap 250 ms)
    TIM3_PSC   = 15999;
    TIM3_ARR   = 249;
    TIM3_DIER |= (1U << 0);
    TIM3_CR1  |= (1U << 0);

    // Konfigurasi TIM4 -> Blink 5 Hz (Toggle tiap 100 ms)
    TIM4_PSC   = 15999;
    TIM4_ARR   = 99;
    TIM4_DIER |= (1U << 0);
    TIM4_CR1  |= (1U << 0);

    // 4. Aktifkan IRQ TIM2 (28), TIM3 (29), TIM4 (30) di NVIC
    NVIC_ISER0 |= (1U << 28) | (1U << 29) | (1U << 30);

    while (1) {
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2_SR & (1U << 0)) {
        TIM2_SR &= ~(1U << 0);
        GPIOA_ODR ^= (1U << 3);   // Toggle PA3
    }
}

void TIM3_IRQHandler(void) {
    if (TIM3_SR & (1U << 0)) {
        TIM3_SR &= ~(1U << 0);
        GPIOA_ODR ^= (1U << 4);   // Toggle PA4
    }
}

void TIM4_IRQHandler(void) {
    if (TIM4_SR & (1U << 0)) {
        TIM4_SR &= ~(1U << 0);
        GPIOA_ODR ^= (1U << 5);   // Toggle PA5
    }
}
