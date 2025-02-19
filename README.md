# Joystick LED Control - ReadMe

Este código foi desenvolvido para um sistema baseado no microcontrolador Raspberry Pi Pico, utilizando um joystick analógico de dois eixos (X e Y) e botões para controlar LEDs com modulação de largura de pulso (PWM). O código usa a linguagem C com a biblioteca `pico-sdk` para interação com o hardware da placa, como ADC (Conversores Analógicos para Digitais) para leitura do joystick e GPIO para controle de LEDs e botões.

## Funcionalidade Principal

- **Eixo X (VRX)**: O movimento horizontal do joystick controla o brilho de um LED (LED2) usando PWM. A posição do joystick na direção esquerda ou direita altera o brilho do LED de forma proporcional.
  
- **Eixo Y (VRY)**: O movimento vertical do joystick controla o brilho de um segundo LED (LED3), funcionando de maneira similar ao LED2.
  
- **Botões**:
  - **Botão A (GPIO 5)**: Alterna os LEDs 2 e 3 entre ligado e desligado, ativando ou desativando o controle de PWM.
  - **Botão SW (GPIO 22)**: Alterna o estado de um terceiro LED (LED1), acendendo ou apagando o LED.

## Hardware

- **Joystick**:
  - VRX (eixo X) conectado ao pino GPIO 26.
  - VRY (eixo Y) conectado ao pino GPIO 27.
  
- **Botões**:
  - Botão SW conectado ao pino GPIO 22.
  - Botão A conectado ao pino GPIO 5.

- **LEDs**:
  - LED1 conectado ao pino GPIO 11.
  - LED2 conectado ao pino GPIO 13 (Controle de PWM).
  - LED3 conectado ao pino GPIO 12 (Controle de PWM).

## Descrição do Código

### Inclusões de Bibliotecas

```c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
```

Essas bibliotecas permitem a configuração básica do sistema, leitura do ADC para os joysticks, e controle de PWM para os LEDs.

### Definições de Pinos

```c
#define VRX_PIN 26
#define VRY_PIN 27
#define SW_PIN 22
#define LED1_PIN 11
#define LED2_PIN 13
#define LED3_PIN 12
#define button_A 5
```

Aqui são definidos os pinos dos diferentes componentes conectados ao Raspberry Pi Pico.

### Variáveis Globais

Variáveis que controlam o estado dos LEDs e o tempo de debouncing dos botões.

### Funções para o Controle dos Eixos X e Y

As funções `eixo_x()` e `eixo_y()` são responsáveis por ler os valores do joystick nos eixos X e Y, respectivamente, e controlar o brilho dos LEDs 2 e 3 com base no movimento do joystick.

### Controle de PWM

A função `pwm_init_gpio()` inicializa os pinos de PWM para controle do brilho dos LEDs 2 e 3.

### Função Principal

Na função `main()`, o código inicializa os componentes de hardware, configura os pinos de entrada/saída, e entra em um loop contínuo que monitora os movimentos do joystick e os estados dos botões. Dentro do loop, as funções `eixo_x()` e `eixo_y()` são chamadas repetidamente.

### Interrupções e Debouncing

A função `gpio_irq_button()` é uma função de interrupção que monitora os botões A e SW. Ela é usada para detectar pressionamentos de botões e alternar os estados dos LEDs com debouncing de 200 ms para evitar múltiplos acionamentos indesejados.

## Fluxo do Programa

1. **Leitura do Joystick**: O joystick é lido continuamente nos eixos X e Y, e o valor lido controla o brilho dos LEDs correspondentes.
2. **Controles de LEDs**: O botão A alterna a habilitação do controle PWM para LED2 e LED3, permitindo desligar o controle de PWM desses LEDs. O botão SW alterna o estado de LED1.
3. **Debouncing**: O tempo entre eventos de pressionamento dos botões é verificado para evitar múltiplos acionamentos rápidos (debouncing).
4. **Interrupções**: As interrupções permitem que o código reaja rapidamente aos pressionamentos dos botões, alterando o estado dos LEDs de maneira eficiente.

## Como Usar

1. **Conectar os componentes**:
   - Conecte o joystick nos pinos VRX e VRY.
   - Conecte os LEDs nos pinos correspondentes.
   - Conecte os botões SW e A aos pinos definidos.
   
2. **Compilação**:
   - Compile o código usando o `pico-sdk` com o CMake.
   
3. **Execução**:
   - Execute o código no Raspberry Pi Pico. O LED1 alternará quando o botão SW for pressionado, e os LEDs 2 e 3 alternarão com o botão A. O movimento do joystick controlará o brilho de LED2 e LED3.

## Conclusão

Este código oferece uma maneira de controlar LEDs com base na leitura de um joystick analógico e botões, utilizando as funcionalidades de ADC e PWM do Raspberry Pi Pico. Ele pode ser expandido ou modificado para controlar outros dispositivos ou implementar novos comportamentos.
