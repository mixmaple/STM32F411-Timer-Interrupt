# Tugas Mandiri: STM32 Timer & Interrupt (Bare-Metal Register)

Implementasi konfigurasi Hardware Timer (`TIM2`, `TIM3`, `TIM4`), External Interrupt (`EXTI`), dan penanganan *pushbutton* berbasis register langsung (tanpa HAL Library) pada mikrokontroler **STM32F411CEU6 (Black Pill)**.

## Spesifikasi Hardware & Pinout
* **Mikrokontroler:** STM32F411CEU6 (System Clock HSI = 16 MHz)
* **LED 1 (TIM2):** `PA3` (Active-High + Resistor)
* **LED 2 (TIM3):** `PA4` (Active-High + Resistor)
* **LED 3 (TIM4):** `PA5` (Active-High + Resistor)
* **Pushbutton Eksternal:** `PA1` (Input dengan Internal Pull-Up, terhubung ke `GND`)

## Struktur & Daftar Program

1. **`1_Blinking_Multiple_LEDs/main.c`**
   Mengedipkan 3 LED secara bersamaan dengan frekuensi berbeda menggunakan tiga hardware timer independen (`PSC = 15999` sehingga clock timer menjadi 1 kHz):
   * `PA3` (`TIM2`, `ARR = 499`): Toggle setiap 500 ms (Frekuensi kedip **1 Hz**)
   * `PA4` (`TIM3`, `ARR = 249`): Toggle setiap 250 ms (Frekuensi kedip **2 Hz**)
   * `PA5` (`TIM4`, `ARR = 99`): Toggle setiap 100 ms (Frekuensi kedip **5 Hz**)

2. **`2_Debouncing_Pushbutton_Semaphore/main.c`**
   Membaca *pushbutton* eksternal pada pin `PA1` melalui jalur interupsi `EXTI1` dan meredam *bouncing* mekanis selama **50 ms** menggunakan `TIM2` (`PSC = 15999`, `ARR = 49`) serta variabel flag *semaphore* (`btn_semaphore`) agar eksekusi pada `main()` berjalan secara *non-blocking*.

3. **`3_Push_to_Blink/main.c`**
   Mendeteksi *falling edge* (saat ditekan) dan *rising edge* (saat dilepas) pada `PA1` (`EXTI1`) untuk menjalankan `TIM2` (`PSC = 15999`, `ARR = 199`), sehingga LED `PA3` berkedip terus-menerus selama tombol ditahan dan langsung berhenti/padam saat tombol dilepas.

4. **`Startup/startup_stm32f411ceux.s`**
   Berkas startup dan tabel vektor interupsi (*vector table*) untuk STM32F411CEU6.
