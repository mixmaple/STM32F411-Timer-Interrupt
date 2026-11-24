#include <stdint.h>

// --- Definisi Alamat Register STM32F411 ---
#define RCC_BASE        0x40023800UL
#define RCC_AHB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x30UL))
#define RCC_APB1ENR     (*(volatile uint32_t *)(RCC_BASE + 0x40UL))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x44UL))

#define GPIOA_BASE      0x40020000UL
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00UL))
#define GPIOA_PUPDR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0CUL))
#define GPIOA_IDR       (*(volatile uint32_t *)(GPIOA_BASE + 0x10UL))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14UL))

#define SYSCFG_BASE     0x40013800UL
#define SYSCFG_EXTICR1  (*(volatile uint32_t *)(SYSCFG_BASE + 0x08UL))

#define EXTI_BASE       0x40013C00UL
#define EXTI_IMR        (*(volatile uint32_t *)(EXTI_BASE + 0x00UL))
#define EXTI_RTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x08UL))
#define EXTI_FTSR       (*(volatile uint32_t *)(EXTI_BASE + 0x0CUL))
#define EXTI_PR         (*(volatile uint32_t *)(EXTI_BASE + 0x14UL))

#define TIM2_BASE       0x40000000UL
#define TIM2_CR1        (*(volatile uint32_t *)(TIM2_BASE + 0x00UL))
#define TIM2_DIER       (*(volatile uint32_t *)(TIM2_BASE + 0x0CUL))
#define TIM2_SR         (*(volatile uint32_t *)(TIM2_BASE + 0x10UL))
#define TIM2_CNT        (*(volatile uint32_t *)(TIM2_BASE + 0x24UL))
#define TIM2_PSC        (*(volatile uint32_t *)(TIM2_BASE + 0x28UL))
#define TIM2_ARR        (*(volatile uint32_t *)(TIM2_BASE + 0x2CUL))

#define NVIC_ISER0      (*(volatile uint32_t *)0xE000E100UL)

void SystemInit(void) {}

int main(void) {
    // 1. Aktifkan clock GPIOA, TIM2, dan SYSCFG
    RCC_AHB1ENR |= (1U << 0);
    RCC_APB1ENR |= (1U << 0);
    RCC_APB2ENR |= (1U << 14);

    // 2. Konfigurasi PA3 sebagai Output (LED)
    GPIOA_MODER &= ~(3U << (3 * 2));
    GPIOA_MODER |=  (1U << (3 * 2));
    GPIOA_ODR   &= ~(1U << 3);          // Kondisi awal LED mati

    // 3. Konfigurasi PA1 sebagai Input dengan Pull-Up
    GPIOA_MODER &= ~(3U << (1 * 2));
    GPIOA_PUPDR &= ~(3U << (1 * 2));
    GPIOA_PUPDR |=  (1U << (1 * 2));

    // 4. Hubungkan EXTI1 ke PA1, trigger Falling Edge (ditekan) & Rising Edge (dilepas)
    SYSCFG_EXTICR1 &= ~(0xFU << 4);
    EXTI_IMR       |=  (1U << 1);
    EXTI_FTSR      |=  (1U << 1);
    EXTI_RTSR      |=  (1U << 1);

    // 5. Konfigurasi TIM2 untuk kedip saat tombol ditahan (toggle tiap 200 ms)
    TIM2_PSC   = 15999;
    TIM2_ARR   = 199;
    TIM2_DIER |= (1U << 0);

    // 6. Aktifkan EXTI1 (IRQ 7) dan TIM2 (IRQ 28) di NVIC
    NVIC_ISER0 |= (1U << 7) | (1U << 28);

    while (1) {
    }
}

void EXTI1_IRQHandler(void) {
    if (EXTI_PR & (1U << 1)) {
        EXTI_PR = (1U << 1);            // Clear pending flag EXTI1

        if ((GPIOA_IDR & (1U << 1)) == 0) {
            // Tombol PA1 DITEKAN -> nyalakan timer kedip
            TIM2_CNT  = 0;
            TIM2_CR1 |= (1U << 0);
        } else {
            // Tombol PA1 DILEPAS -> matikan timer dan padamkan LED PA3
            TIM2_CR1  &= ~(1U << 0);
            GPIOA_ODR &= ~(1U << 3);
        }
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2_SR & (1U << 0)) {
        TIM2_SR = ~(1U << 0);           // Clear update flag TIM2
        (void)TIM2_SR;

        if ((GPIOA_IDR & (1U << 1)) == 0) {
            GPIOA_ODR ^= (1U << 3);     // Toggle LED PA3 selama tombol ditahan
        } else {
            TIM2_CR1  &= ~(1U << 0);
            GPIOA_ODR &= ~(1U << 3);
        }
    }
}
