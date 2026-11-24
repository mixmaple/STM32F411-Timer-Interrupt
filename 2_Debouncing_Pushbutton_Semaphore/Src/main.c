#include "stm32f4xx.h"

// flag semaphore buat ngasih sinyal dari ISR ke main loop
volatile uint8_t btn_semaphore = 0;

void GPIO_EXTI_Init(void);
void TIM2_Debounce_Init(void);

int main(void) {
    GPIO_EXTI_Init();
    TIM2_Debounce_Init();

    while (1) {
        // cek flag semaphore tanpa nge-block loop utama
        if (btn_semaphore == 1) {
            btn_semaphore = 0;            // reset flag
            GPIOA->ODR ^= (1U << 3);      // toggle LED PA3
        }
    }
}

void GPIO_EXTI_Init(void) {
    // nyalain clock GPIOA sama SYSCFG
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // set PA3 jadi output buat LED, kondisi awal mati
    GPIOA->MODER &= ~(3U << (3 * 2));
    GPIOA->MODER |=  (1U << (3 * 2));
    GPIOA->ODR   &= ~(1U << 3);

    // set PA1 jadi input pull-up buat button
    GPIOA->MODER &= ~(3U << (1 * 2));
    GPIOA->PUPDR &= ~(3U << (1 * 2));
    GPIOA->PUPDR |=  (1U << (1 * 2));

    // arahin EXTI1 ke PA1
    SYSCFG->EXTICR[0] &= ~(0xFU << 4);

    // aktifin interrupt pas falling edge (tombol ditekan ke GND)
    EXTI->IMR  |= (1U << 1);
    EXTI->FTSR |= (1U << 1);

    NVIC_EnableIRQ(EXTI1_IRQn);
}

void TIM2_Debounce_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC   = 15999;                  // 1 ms per tick
    TIM2->ARR   = 49;                     // delay debounce 50 ms
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1U << 1)) {
        EXTI->PR   = (1U << 1);           // hapus pending flag EXTI1
        EXTI->IMR &= ~(1U << 1);          // matiin dulu EXTI1 biar bouncing-nya ke-skip
        TIM2->CNT  = 0;                   // reset hitungan timer
        TIM2->CR1 |= TIM_CR1_CEN;         // mulai hitung 50 ms
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR   = ~TIM_SR_UIF;         // clear flag TIM2
        TIM2->CR1 &= ~TIM_CR1_CEN;        // stop timer (sekali jalan aja)
        (void)TIM2->SR;

        // cek lagi setelah 50 ms, beneran masih ditekan atau cuma noise
        if ((GPIOA->IDR & (1U << 1)) == 0) {
            btn_semaphore = 1;            // valid ditekan, naikin semaphore
        }

        EXTI->PR   = (1U << 1);           // bersihin sisa flag selama bouncing
        EXTI->IMR |= (1U << 1);           // nyalain lagi EXTI1
    }
}
