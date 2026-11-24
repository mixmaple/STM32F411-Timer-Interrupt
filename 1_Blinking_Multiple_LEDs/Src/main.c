#include "stm32f4xx.h"

void GPIO_Init(void);
void TIM2_Init(void);
void TIM3_Init(void);
void TIM4_Init(void);

int main(void) {
    GPIO_Init();
    TIM2_Init();
    TIM3_Init();
    TIM4_Init();

    while (1) {
        // kosong, semua kedipan udah di-handle sama interrupt timer
    }
}

void GPIO_Init(void) {
    // nyalain clock GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // set pin PA3, PA4, PA5 jadi output
    GPIOA->MODER &= ~((3U << (3 * 2)) | (3U << (4 * 2)) | (3U << (5 * 2)));
    GPIOA->MODER |=  ((1U << (3 * 2)) | (1U << (4 * 2)) | (1U << (5 * 2)));
}

void TIM2_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;   // nyalain clock TIM2
    TIM2->PSC   = 15999;                  // 16 MHz / 16000 = 1 kHz (1 tick = 1 ms)
    TIM2->ARR   = 499;                    // toggle tiap 500 ms -> kedip 1 Hz
    TIM2->DIER |= TIM_DIER_UIE;           // enable update interrupt
    TIM2->CR1  |= TIM_CR1_CEN;            // mulai timer
    NVIC_EnableIRQ(TIM2_IRQn);            // aktifin interrupt TIM2 di NVIC
}

void TIM3_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;   // nyalain clock TIM3
    TIM3->PSC   = 15999;                  // 1 kHz (1 tick = 1 ms)
    TIM3->ARR   = 249;                    // toggle tiap 250 ms -> kedip 2 Hz
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM4_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;   // nyalain clock TIM4
    TIM4->PSC   = 15999;                  // 1 kHz (1 tick = 1 ms)
    TIM4->ARR   = 99;                     // toggle tiap 100 ms -> kedip 5 Hz
    TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1  |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM4_IRQn);
}

// --- Interrupt Handlers ---
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR   = ~TIM_SR_UIF;         // clear flag interrupt
        GPIOA->ODR ^= (1U << 3);          // toggle LED PA3
        (void)TIM2->SR;
    }
}

void TIM3_IRQHandler(void) {
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR   = ~TIM_SR_UIF;
        GPIOA->ODR ^= (1U << 4);          // toggle LED PA4
        (void)TIM3->SR;
    }
}

void TIM4_IRQHandler(void) {
    if (TIM4->SR & TIM_SR_UIF) {
        TIM4->SR   = ~TIM_SR_UIF;
        GPIOA->ODR ^= (1U << 5);          // toggle LED PA5
        (void)TIM4->SR;
    }
}
