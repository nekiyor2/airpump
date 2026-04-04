#include "control.h"
#include "valve.h"
#include "compressor.h"
#include "battery.h"

static ControlConfig_t _cfg = {
    .pressure_min    = PRESSURE_MIN_DEFAULT,
    .pressure_max    = PRESSURE_MAX_DEFAULT,
    .pressure_relief = PRESSURE_RELIEF_DEFAULT
};

static uint8_t _compressor_on = 0;
static uint8_t _system_state  = SYS_OK;

void Control_Init(void)
{
    Valve_Init();
    // Compressor_Init викликається в main.c раніше (потребує htim1)
    _compressor_on = 0;
    _system_state  = SYS_OK;
}

void Control_Update(float pressure, float battery_voltage)
{
    // --- Захист від помилки датчика ---
    if (pressure < 0.0f)
    {
        _system_state = SYS_ERROR;
        Compressor_Stop();
        Valve_Close();
        _compressor_on = 0;
        return;
    }

    _system_state = SYS_OK;

    // --- Аварійний скид тиску ---
    if (pressure >= _cfg.pressure_relief)
    {
        Valve_Open();
        Compressor_Stop();
        _compressor_on = 0;
        return;
    }

    // Закриваємо клапан коли тиск впав до безпечного рівня
    if (Valve_IsOpen() && pressure <= PRESSURE_RELIEF_CLOSE)
    {
        Valve_Close();
    }

    // --- Основна логіка компресора ---
    if (!Valve_IsOpen())
    {
        if (pressure < _cfg.pressure_min && !_compressor_on)
        {
            Compressor_SetSpeed(COMPRESSOR_SPEED_DEFAULT);
            _compressor_on = 1;
        }
        else if (pressure >= _cfg.pressure_max && _compressor_on)
        {
            Compressor_Stop();
            _compressor_on = 0;
        }
    }

    // --- Захист батареї ---
    if (battery_voltage > 0.0f && Battery_IsLow(battery_voltage))
    {
        Compressor_Stop();
        _compressor_on = 0;
        _system_state  = SYS_ERROR;
    }
}

void Control_SetConfig(ControlConfig_t *cfg)
{
    if (cfg == NULL) return;
    _cfg = *cfg;
}

void Control_GetTelemetry(TelemetryPacket_t *pkt)
{
    if (pkt == NULL) return;
    // Заповнюється в main.c перед відправкою
    pkt->compressor_on  = _compressor_on;
    pkt->valve_on       = Valve_IsOpen();
    pkt->system_state   = _system_state;
}
