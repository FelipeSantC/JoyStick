#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"

#define VRX_PIN 26
#define VRY_PIN 27
#define SW_PIN 22
#define LED1_PIN 11
#define LED2_PIN 13
#define LED3_PIN 12
#define button_A 5

uint32_t last_print_time = 0;

// Variáveis globais para controlar o estado do botão e o LED
bool led1_state = false;   // Estado do LED1
bool last_sw_value = true; // Estado anterior do botão (inicialmente pressionado)
bool last_button_A_state = true;
bool led2_enabled = false; // Controle do LED2 (PWM)
bool led3_enabled = false; // Controle do LED3 (PWM)

// Prototipação da função de interrupção
static void gpio_irq_button(uint gpio, uint32_t events);

void eixo_x()
{
    if (led3_enabled == false)
    {
        adc_select_input(1);
        uint16_t vrx_value = adc_read();

        float duty_cycle = (vrx_value / 4095.0) * 100;

        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        if (current_time - last_print_time >= 1000)
        {
            printf("VRX: %u\n", vrx_value);
            printf("Duty Cycle LED: %.2f%%\n", duty_cycle);
            last_print_time = current_time;
        }

        // Controle do brilho do LED
        int led_brightness = 0;

        // Se o joystick se mover para a esquerda (vrx_value < 2048), o LED acende
        if (vrx_value < 1300)
        {
            // O brilho aumenta à medida que o joystick se move para a esquerda
            led_brightness = (2048 - vrx_value) * 4095 / 2048; // Brilho proporcional, máximo no extremo esquerdo
        }
        // Se o joystick se mover para a direita (vrx_value > 2048), o LED também acende
        else if (vrx_value > 2500)
        {
            // O brilho aumenta à medida que o joystick se move para a direita
            led_brightness = (vrx_value - 2048) * 4095 / 2048; // Brilho proporcional, máximo no extremo direito
        }

        // O brilho é limitado entre 0 e 4095
        if (led_brightness > 4095)
        {
            led_brightness = 4095;
        }
        else if (led_brightness < 0)
        {
            led_brightness = 0;
        }

        // Define o nível do PWM para o LED com base no cálculo de brilho
        pwm_set_gpio_level(LED2_PIN, led_brightness);

        printf("VRX: %u", vrx_value);
    }
}

void eixo_y()
{
    if (led3_enabled == false)
    {
        adc_select_input(0);
        uint16_t vry_value = adc_read();

        // Controle do brilho do LED para o eixo Y
        int led_brightness_y = 0;

        // Se o joystick se mover para cima (vry_value < 1300), o LED acende
        if (vry_value < 1300)
        {
            // O brilho aumenta à medida que o joystick se move para cima
            led_brightness_y = (2048 - vry_value) * 4095 / 2048; // Brilho proporcional, máximo no extremo superior
        }
        // Se o joystick se mover para baixo (vry_value > 2500), o LED também acende
        else if (vry_value > 2500)
        {
            // O brilho aumenta à medida que o joystick se move para baixo
            led_brightness_y = (vry_value - 2048) * 4095 / 2048; // Brilho proporcional, máximo no extremo inferior
        }

        // O brilho é limitado entre 0 e 4095
        if (led_brightness_y > 4095)
        {
            led_brightness_y = 4095;
        }
        else if (led_brightness_y < 0)
        {
            led_brightness_y = 0;
        }

        // Define o nível do PWM para o LED correspondente ao eixo Y (LED3_PIN)
        pwm_set_gpio_level(LED3_PIN, led_brightness_y);

        printf("VRY: %u\n", vry_value);
    }
}

uint pwm_init_gpio(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

int main()
{
    stdio_init_all();
    adc_init();
    adc_gpio_init(VRX_PIN);
    adc_gpio_init(VRY_PIN);
    gpio_init(SW_PIN);
    gpio_set_dir(SW_PIN, GPIO_IN);
    gpio_pull_up(SW_PIN);
    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, false);
    gpio_init(LED2_PIN);
    gpio_set_dir(LED2_PIN, GPIO_OUT);
    gpio_put(LED2_PIN, false);
    gpio_init(LED3_PIN);
    gpio_set_dir(LED3_PIN, GPIO_OUT);
    gpio_put(LED3_PIN, false);
    gpio_init(button_A);
    gpio_set_dir(button_A, GPIO_IN);
    gpio_pull_up(button_A);

    uint pwm_wrap = 4096;
    uint pwm_slice_x = pwm_init_gpio(LED2_PIN, pwm_wrap);
    uint pwm_slice_y = pwm_init_gpio(LED3_PIN, pwm_wrap);

    bool sw_value = gpio_get(SW_PIN) == 0;

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_button);
    gpio_set_irq_enabled_with_callback(SW_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_button);

    while (true)
    {
        eixo_x();
        eixo_y();

        printf(" SW: %d\n", sw_value);
        sleep_ms(500);
    }
}

// Função de interrupção com debouncing
void gpio_irq_button(uint gpio, uint32_t events)
{
    if (gpio == button_A)
    {

        // Obtém o tempo atual em microssegundos
        uint32_t current_time = to_ms_since_boot(get_absolute_time());

        if (current_time - last_print_time >= 200)
        {
            last_print_time = current_time;
           
            // Alterna o estado dos LEDs PWM (LED2 e LED3)
            led2_enabled = !led2_enabled;
            led3_enabled = !led3_enabled;

            printf("Button  A Pressed, LED2 state: %d\n", led2_enabled);
        }
    }
    else if (gpio == SW_PIN)
    {
        // Obtém o tempo atual em microssegundos
        uint32_t current_time = to_us_since_boot(get_absolute_time());

        // Verifica se passou tempo suficiente desde o último evento
        if (current_time - last_print_time >= 200000) // 200 ms de debouncing
        {
            last_print_time = current_time;

            bool sw_value = gpio_get(SW_PIN) == 0;

            // Verifica a transição de pressionamento do botão
            if (sw_value == true && last_sw_value == true)
            {
                // Alterna o estado do LED1
                led1_state = !led1_state;
                gpio_put(LED1_PIN, led1_state); // Acende ou apaga o LED1

                printf("SW Button Pressed, LED1 state: %d\n", led1_state);
            }
            // Atualiza o estado anterior do botão
            last_sw_value = sw_value;
        }
    }
}
