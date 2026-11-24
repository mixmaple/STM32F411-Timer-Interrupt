#include "stm32f4xx.h"

void GPIO_EXTI_Init(void);
void TIM2_Blink_Init(void);

int main(void) {
    GPIO_EXTI_Init();
    TIM2_Blink_Init();

    while (1) {
        // kosong, dikontrol lewat EXTI1 & TIM2
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

    // aktifin trigger pas ditekan (falling) & dilepas (rising)
    EXTI->IMR  |= (1U << 1);
    EXTI->FTSR |= (1U << 1);
    EXTI->RTSR |= (1U << 1);

    NVIC_EnableIRQ(EXTI1_IRQn);
}

void TIM2_Blink_Init(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC   = 15999;                  // 1 ms per tick
    TIM2->ARR   = 199;                    // toggle tiap 200 ms (kedip 2.5 Hz)
    TIM2->DIER |= TIM_DIER_UIE;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void EXTI1_IRQHandler(void) {
    if (EXTI->PR & (1U << 1)) {
        EXTI->PR = (1U << 1);             // hapus pending flag EXTI1

        if ((GPIOA->IDR & (1U << 1)) == 0) {
            // tombol ditekan -> reset counter terus nyalain timer kedip
            TIM2->CNT  = 0;
            TIM2->CR1 |= TIM_CR1_CEN;
        } else {
            // tombol dilepas -> matiin timer & matiin LED PA3
            TIM2->CR1  &= ~TIM_CR1_CEN;
            GPIOA->ODR &= ~(1U << 3);
        }
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;           // clear flag TIM2
        (void)TIM2->SR;

        // pastiin tombol masih ditahan sebelum toggle LED
        if ((GPIOA->IDR & (1U << 1)) == 0) {
            GPIOA->ODR ^= (1U << 3);      // toggle LED PA3
        } else {
            TIM2->CR1  &= ~TIM_CR1_CEN;
            GPIOA->ODR &= ~(1U << 3);
        }
    }
}
