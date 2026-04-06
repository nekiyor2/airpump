# AirPump — Wheel Module (STM32C011F6P6)

## Про проект
Автоматична система підкачки шини електромоноколеса.
Складається з двох модулів:
- Wheel module — STM32C011F6P6 (цей репозиторій)
- Body module  — ESP32-C3 SuperMini (окремий репозиторій)

---

## Залізо (Wheel module)

| Компонент                  | Підключення                              |
|----------------------------|------------------------------------------|
| Датчик тиску XGZP6847D500KPG | I2C — SDA: PB7, SCL: PB6              |
| Батарея 2S Li-Ion (7.4V)   | ADC — PA0, дільник R4=100k / R8=47k     |
| nRF24L01                   | SPI — SCK:PA5 MISO:PA6 MOSI:PA7 CSN:PA4 CE:PA3 |
| ESC (BLDC компресор)       | PWM — PA2 (TIM1_CH3, 50Hz)              |
| Клапан (соленоїд)          | GPIO — PA1, через Q2 AO3400A + R7 220Ом |
| LDO 3.3V                   | MCP1703T-3302E (вхід до 16V)            |
| Захист ESC                 | Діод SS54 + 100мкФ + 100нФ              |

---

## Структура файлів

```
Core/
  Inc/
    battery.h        — вимірювання напруги батареї
    compressor.h     — керування ESC (PWM)
    control.h        — логіка тиску, пороги, стан системи
    error_log.h      — логування помилок
    flash_config.h   — збереження конфігу у Flash
    nrf24l01.h       — драйвер nRF24L01
    valve.h          — керування клапаном
    xgzp6847d.h      — драйвер датчика тиску
  Src/
    battery.c
    compressor.c
    control.c
    error_log.c
    flash_config.c
    nrf24l01.c
    valve.c
    xgzp6847d.c
    main.c
```

---

## Логіка керування тиском

| Умова                                      | Дія                              |
|--------------------------------------------|----------------------------------|
| Тиск < pressure_min (1.8 bar)              | Вмикаємо компресор (80%)         |
| Тиск >= pressure_max (2.2 bar)             | Вимикаємо компресор              |
| Тиск >= pressure_relief (3.5 bar)          | Відкриваємо клапан (аварія)      |
| Тиск <= pressure_relief_close (3.0 bar)    | Закриваємо клапан                |
| Батарея < 6.0V                             | Вимикаємо компресор, SYS_ERROR   |
| Датчик помилка 5 разів поспіль             | Вимикаємо все, SYS_ERROR         |
| ESP32 мовчить > 5с                         | Повертаємось на дефолтні пороги  |
| Компресор працює 10с без приросту тиску    | WARNING (компресор не вимикаємо) |

---

## Обмін даними з ESP32 (nRF24L01)

STM32 -> ESP32 (телеметрія, кожні 500мс):
```c
typedef struct {
    float    pressure;          // тиск, бари
    float    battery_voltage;   // напруга, В
    uint8_t  compressor_on;     // 0/1
    uint8_t  valve_on;          // 0/1
    uint8_t  system_state;      // 0=OK, 1=ERROR
    uint8_t  pressure_warning;  // 1=тиск не зростає
} TelemetryPacket_t;
```

ESP32 -> STM32 (конфігурація порогів):
```c
typedef struct {
    float pressure_min;     // мінімальний тиск
    float pressure_max;     // максимальний тиск
    float pressure_relief;  // аварійний поріг
} ControlConfig_t;
```

Канал: 76 (0x4C)
Адреса: 0xE7E7E7E7E7
Швидкість: 1Mbps

---

## Логування помилок

Коди помилок:
```
ERR_SENSOR_FAIL   — датчик не відповідає
ERR_COMMS_LOST    — зв'язок з ESP32 втрачено
ERR_BATTERY_LOW   — батарея розряджена
ERR_PRESSURE_HIGH — аварійний тиск
ERR_COMPRESSOR    — компресор не качає
```

Зберігаються останні 8 помилок в RAM з часовою міткою.
Виводяться через UART кожні 10 секунд (115200 baud).

---

## Збереження конфігурації у Flash

Сторінка 15, адреса 0x08007C00.
Захист: magic number (0xA55A0001) + checksum.
Записується тільки при отриманні нової конфігурації від ESP32.
Максимум ~10000 циклів запису (при зміні раз на день = 27 років).

---

## Watchdog (IWDG)

Prescaler: 64
Reload: 1999
Таймаут: ~4 секунди
HAL_IWDG_Refresh викликається кожні 10мс в while(1).

---

## Sleep mode

МК переходить в Sleep mode між ітераціями циклу
тільки коли компресор вимкнений.
Периферія (TIM1 PWM, SPI, I2C) продовжує працювати в Sleep.
Економія енергії: ~40%.

---

## Вимкнення системи

При отриманні команди вимкнення від ESP32 (маркер 0xFF):
1. Зупиняємо компресор
2. Закриваємо клапан
3. МК переходить в Stop mode
4. Прокидання тільки від RESET

---

## Налаштування тактування

Джерело: HSI / 4 = 12 MHz
TIM1 PWM: Prescaler=47, Period=19999 -> 50Hz
ADC: 12-біт, одиночне перетворення
I2C: Standard mode
SPI: Prescaler/16, CPOL=0, CPHA=0

---

## Інструменти розробки

STM32CubeIDE 2.1.0
STM32CubeMX (генерація HAL коду)
Компілятор: arm-none-eabi-gcc 14.3.1
Оптимізація: -Os (розмір)
