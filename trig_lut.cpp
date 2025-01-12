#include "global_includes.h"

#define PI 3.14159265358979323846
#define SIN8_SCALE 256 // Scale factor for 8-bit fixed-point arithmetic

// Function to calculate sin(x) for fixed-point 8-bit (scaled)
uint8_t sin8(uint8_t angle)
{
    // Convert angle from fixed-point to floating point
    double radians = (double)angle / SIN8_SCALE * 2 * PI;

    // Using the first few terms of the Taylor series for sine
    double result = radians;
    double term = radians;
    int n = 1;
    int sign = -1;

    // Calculate sine using 5 terms for higher accuracy
    for (n = 1; n < 5; n++)
    {
        term *= radians * radians / (2 * n * (2 * n + 1));
        result += sign * term;
        sign *= -1; // alternate signs
    }

    // Scale the result back to 8-bit fixed-point
    return (int16_t)(result * SIN8_SCALE);
}

uint8_t cos8(uint8_t angle)
{
    // Convert angle from fixed-point to floating point
    double radians = (double)angle / SIN8_SCALE * 2 * PI;

    // Using the first few terms of the Taylor series for cosine
    double result = 1.0; // Cosine starts with 1
    double term = 1.0;   // First term is 1
    int sign = -1;       // Alternating signs start with '-'
    int n;

    // Calculate cosine using 5 terms for higher accuracy
    for (n = 1; n < 5; n++)
    {
        term *= radians * radians / (2 * n - 1) / (2 * n);
        result += sign * term;
        sign *= -1; // alternate signs
    }

    // Scale the result back to 8-bit fixed-point
    return (int16_t)(result * SIN8_SCALE);
}

#define SIN16_SCALE 65536 // Scale factor for 16-bit fixed-point

// Function to calculate sin(x) for fixed-point 16-bit (scaled)
int16_t sin16(int16_t angle)
{
    // Convert angle from fixed-point to floating point
    double radians = (double)angle / SIN16_SCALE * 2 * PI;

    // Using the first few terms of the Taylor series for sine
    double result = radians;
    double term = radians;
    int n = 1;
    int sign = -1;

    // Calculate sine using 5 terms for higher accuracy
    for (n = 1; n < 5; n++)
    {
        term *= radians * radians / (2 * n * (2 * n + 1));
        result += sign * term;
        sign *= -1; // alternate signs
    }

    // Scale the result back to 16-bit fixed-point
    return (int16_t)(result * SIN16_SCALE);
}

int16_t cos16(int16_t angle)
{
    // Convert angle from fixed-point to floating point
    double radians = (double)angle / SIN16_SCALE * 2 * PI;

    // Using the first few terms of the Taylor series for cosine
    double result = 1.0; // Cosine starts with 1
    double term = 1.0;   // First term is 1
    int sign = -1;       // Alternating signs start with '-'
    int n;

    // Calculate cosine using 5 terms for higher accuracy
    for (n = 1; n < 5; n++)
    {
        term *= radians * radians / (2 * n - 1) / (2 * n);
        result += sign * term;
        sign *= -1; // alternate signs
    }

    // Scale the result back to 16-bit fixed-point
    return (int16_t)(result * SIN16_SCALE);
}

int16_t scale16(int16_t i, int16_t scale)
{
    int16_t result = ((uint32_t)(i) * (1 + (uint32_t)(scale))) / 65536;
    return result;
}

uint8_t scale8(uint8_t i, uint8_t scale)
{
    return (((uint16_t)i) * (1 + (uint16_t)(scale))) >> 8;
}

uint16_t beat88(uint16_t beats_per_minute_88, uint32_t timebase)
{
    // BPM is 'beats per minute', or 'beats per 60000ms'.
    // To avoid using the (slower) division operator, we
    // want to convert 'beats per 60000ms' to 'beats per 65536ms',
    // and then use a simple, fast bit-shift to divide by 65536.
    //
    // The ratio 65536:60000 is 279.620266667:256; we'll call it 280:256.
    // The conversion is accurate to about 0.05%, more or less,
    // e.g. if you ask for "120 BPM", you'll get about "119.93".
    return (((get_current_time_millis()) - timebase) * beats_per_minute_88 * 280) >> 16;
}

/// Generates a 16-bit "sawtooth" wave at a given BPM
/// @param beats_per_minute the frequency of the wave, in decimal
/// @param timebase the time offset of the wave from the millis() timer
uint16_t beat16(uint16_t beats_per_minute, uint32_t timebase)
{
    // Convert simple 8-bit BPM's to full Q8.8 uint16_t's if needed
    if (beats_per_minute < 256)
        beats_per_minute <<= 8;
    return beat88(beats_per_minute, timebase);
}

/// Generates an 8-bit "sawtooth" wave at a given BPM
/// @param beats_per_minute the frequency of the wave, in decimal
/// @param timebase the time offset of the wave from the millis() timer
uint8_t beat8(uint16_t beats_per_minute, uint32_t timebase)
{
    return beat16(beats_per_minute, timebase) >> 8;
}

/// Generates a 16-bit sine wave at a given BPM that oscillates within
/// a given range.
/// @param beats_per_minute_88 the frequency of the wave, in Q8.8 format
/// @param lowest the lowest output value of the sine wave
/// @param highest the highest output value of the sine wave
/// @param timebase the time offset of the wave from the millis() timer
/// @param phase_offset phase offset of the wave from the current position
/// @warning The BPM parameter **MUST** be provided in Q8.8 format! E.g.
/// for 120 BPM it would be 120*256 = 30720. If you just want to specify
/// "120", use beatsin16() or beatsin8().
uint16_t beatsin88(uint16_t beats_per_minute_88, uint16_t lowest, uint16_t highest,
                   uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat88(beats_per_minute_88, timebase);
    uint16_t beatsin = (sin16(beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16(beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

/// Generates a 16-bit sine wave at a given BPM that oscillates within
/// a given range.
/// @param beats_per_minute the frequency of the wave, in decimal
/// @param lowest the lowest output value of the sine wave
/// @param highest the highest output value of the sine wave
/// @param timebase the time offset of the wave from the millis() timer
/// @param phase_offset phase offset of the wave from the current position
uint16_t beatsin16(uint16_t beats_per_minute, uint16_t lowest, uint16_t highest,
                   uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat16(beats_per_minute, timebase);
    uint16_t beatsin = (sin16(beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16(beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

/// Generates an 8-bit sine wave at a given BPM that oscillates within
/// a given range.
/// @param beats_per_minute the frequency of the wave, in decimal
/// @param lowest the lowest output value of the sine wave
/// @param highest the highest output value of the sine wave
/// @param timebase the time offset of the wave from the millis() timer
/// @param phase_offset phase offset of the wave from the current position
uint8_t beatsin8(uint16_t beats_per_minute, uint8_t lowest, uint8_t highest,
                 uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat8(beats_per_minute, timebase);
    uint8_t beatsin = sin8(uint16_t(beat + phase_offset));
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8(beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

uint8_t mapsin8(uint8_t val, uint8_t minscale, uint8_t maxscale)
{
    uint8_t out = sin8(val);
    return scale8(out, maxscale - minscale) + minscale;
}

uint16_t mapsin16(uint16_t val, uint16_t minscale, uint16_t maxscale)
{
    uint8_t out = sin16(val);
    return scale16(out, maxscale - minscale) + minscale;
}

uint8_t mapcos8(uint8_t val, uint8_t minscale, uint8_t maxscale)
{
    uint8_t out = cos8(val);
    return scale8(out, maxscale - minscale) + minscale;
}

uint16_t mapcos16(uint16_t val, uint16_t minscale, uint16_t maxscale)
{
    uint8_t out = cos16(val);
    return scale16(out, maxscale - minscale) + minscale;
}