#include <TFT_eSPI.h>
#include <SPI.h>
#include <arduinoFFT.h>
#include <driver/i2s.h>

#define SAMPLES 2048 //bigger samples allow for more frequencies to be detected
#define SAMPLING_FREQ 48000

#define MIC_BITS_PER_SAMPLE  I2S_BITS_PER_SAMPLE_32BIT
#define I2S_MIC_SERIAL_CLOCK    GPIO_NUM_26
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA    GPIO_NUM_33

TFT_eSPI tft = TFT_eSPI();

static float vReal[SAMPLES];
static float vImag[SAMPLES];

static int32_t raw_samples[SAMPLES];

const int numOfBars = 41;

// const float pinkGain[41] = { // additional boosting for higher frequency. not needed for now
//   // Bands 0–24: flat
//   1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
//   1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0,
//   1.0, 1.0, 1.0, 1.0, 1.0,

//   // Bands 25–40: gentle ramp to ~1.6
//   1.05, 1.1, 1.15, 1.2, 1.25,
//   1.3, 1.35, 1.4, 1.45, 1.5,
//   1.55, 1.6, 1.6, 1.6, 1.6, 1.6
// };

// float sinLookup[numOfBars];
// float cosLookup[numOfBars];
const float angleStep = 2.0 * PI / numOfBars; // 360 degrees / numOfBars (in radians)

const uint16_t colors[6] = {
  TFT_MAGENTA,    // strong pink-purple
  TFT_YELLOW,     // strong bright yellow
  TFT_ORANGE,     // custom bright orange
  TFT_GREEN,      // vivid green
  TFT_CYAN,       // bright blue-green
  TFT_WHITE,      // super bright
};

static int colorIndex = 0;

float bandValues[41];
size_t bytes_read = 0;

int barwidth;

float smoothedBands[41];   // Match numOfBars
float smoothingFactor = 0.6;  // Adjust for smoothness

static const float noiseFloor = -17.0;  // quietest noise that will be displayed- adjust if needed depending on your noise environment
static const float maxNoise   = 0.0;   // highest noise to be displayed


ArduinoFFT<float> FFT = ArduinoFFT<float>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

// I2S Configuration
const i2s_config_t i2s_config = {
  .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
  .sample_rate = SAMPLING_FREQ,
  .bits_per_sample = MIC_BITS_PER_SAMPLE,
  .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  .communication_format = I2S_COMM_FORMAT_I2S,
  .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
  .dma_buf_count = 4,
  .dma_buf_len = 1024,
  .use_apll = false,
  .tx_desc_auto_clear = false,
  .fixed_mclk = 0
};

const i2s_pin_config_t i2s_mic_pins = {
  .bck_io_num = I2S_MIC_SERIAL_CLOCK,
  .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
  .data_out_num = I2S_PIN_NO_CHANGE,
  .data_in_num = I2S_MIC_SERIAL_DATA
};

void i2s_initialization() {
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
}

// // precompute the angles array so it is consistent for drawing each time (for even faster speed but not needed for now)
// void precomputeAngles() {
//   for (int i = 0; i < numOfBars; i++) {
//     float angle = i * angleStep;
//     sinLookup[i] = sin(angle);
//     cosLookup[i] = cos(angle);
//   }
// }

void performFFT() { //Fast fourier transform to convert wave form into actual bins to be converted into band displays
  memset(bandValues, 0, sizeof(bandValues));  //empty all the bandvalues so it doesnt add up

  int32_t peak = 1;
  for (int i = 0; i < SAMPLES; i++) {
    int32_t shifted = raw_samples[i] >> 8;

    if (abs(shifted) > peak) peak = abs(shifted); // find the loudest sample
    if (peak < 1) peak = 1;  // Avoid divide-by-zero
  }

  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = (float)(raw_samples[i] >> 8) / float(peak); //normalize by dividing all by loudest sample
    vImag[i] = 0.0;
  }

  static double dcOffset = 0;
  for (int i = 0; i < SAMPLES; i++) {
    dcOffset = 0.999 * dcOffset + 0.001 * vReal[i];  // Slow DC tracking
    vReal[i] -= dcOffset;
  }
  
  // FFT.dcRemoval();
  FFT.windowing(FFT_WIN_TYP_HANN, FFT_FORWARD); 
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  for (int i = 0; i < SAMPLES / 2; i++) {
    vReal[i] /= (SAMPLES / 2); // Normalize again the produced values by FFT
  }

    // Logarithmic band mapping with RMS + smoothing
  int barIndex = 0;
  for (int i = 2; i < SAMPLES / 2; ) {
    int bandSize = pow(1.148017, barIndex) + 1; //1.148017 calculating how many bins go into each succession band
    // +1 ensures each band gets at least one bin

    float energy = 0;
    for (int j = 0; j < bandSize && i < SAMPLES / 2; j++, i++) {
      energy += vReal[i] * vReal[i]; // Use power for RMS
    }

    float gain = 25.0;                // Make bars more reactive
    float rms = sqrt(energy / bandSize) * gain; //squareroot the average

    if (rms > 1.0) rms = 1.0;        // prevent crazy spikes
    if (rms < 1e-4) rms = 0;

    // Smoothing
    smoothedBands[barIndex] = smoothingFactor * smoothedBands[barIndex] + 
                              (1.0 - smoothingFactor) * rms; //parts of the old band values+some of current bands

    bandValues[barIndex] = smoothedBands[barIndex]; //* pinkGain[barIndex];

    barIndex++;
    if (barIndex >= numOfBars) break; //limit to 41 bars
  }
}

// converting bin values to actual bar height
int barHeight(float val) {
  if (val <= 1e-6) return 0;
  float dB = 20.0 * log10(val);
  if (dB < noiseFloor) return 0;
  dB = constrain(dB, noiseFloor, maxNoise);
  int maxBarHeight = (int)(tft.height() * 0.90); //change here to constrain max bar height
  return map(dB, noiseFloor, maxNoise, 0, maxBarHeight);
}

// // for linear display
// void showSpectrum() {
//   tft.startWrite();

//   for (int j = 0; j < numOfBars; j++) {
//     int h = barHeight(bandValues[j]);
//     int x = j * barwidth;
//     int y = tft.height() - h;

//     // Clear the whole bar column first
//     tft.drawFastVLine(x, 0, tft.height(), TFT_BLACK);
//     tft.drawFastVLine(x, y, h, colors[colorIndex]); //tft.color565(255,105,180) 
//   }

//   tft.endWrite();
//   colorIndex = (colorIndex + 1) % 7;
// }


// for moving circular display like cd rotating
void showSpectrum() { 
  tft.startWrite();

  int centerX = tft.width() / 2; // center of screen
  int centerY = tft.height() / 2;
  int baseRadius = 30;
  int maxBarHeight = 170;  
  int clearRadius = baseRadius + maxBarHeight;

  // Clear the whole spectrum area
  tft.fillCircle(centerX, centerY, clearRadius, TFT_BLACK);

  float rotation = millis() * 0.0005;  // controls spin speed based on real run time

  for (int j = 0; j < numOfBars; j++) {
    int h = barHeight(bandValues[j]); //calling bar height here resets the max height to set percentage

    // finding the angles, rotating 
    float angle = j * angleStep + rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);

    int x1 = centerX + baseRadius * cosA;
    int y1 = centerY + baseRadius * sinA;
    int x2 = centerX + (baseRadius + h) * cosA;
    int y2 = centerY + (baseRadius + h) * sinA;

    tft.drawLine(x1, y1, x2, y2, colors[colorIndex]);
  }

  tft.endWrite();
  colorIndex = (colorIndex + 1) % 7; //cycle through the colors
}

void setup() {
  i2s_initialization();

  tft.init();
  delay(100);
  tft.setRotation(3); //landscape mode
  tft.fillScreen(TFT_BLACK);

  barwidth = tft.width() / numOfBars;
  //precomputeAngles(); //precalculate the angles array (not needed for now)
}

void loop() {
  esp_err_t result = i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLES, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);

  if (result == ESP_OK && samples_read == SAMPLES) {
    performFFT();

    int maxIndex = 0;
    float maxVal = 0;

    for (int i = 0; i < numOfBars; i++) {
      if (bandValues[i] > maxVal) {
        maxVal = bandValues[i];
        maxIndex = i;
      }
    }
    showSpectrum();

  delay(20); // small delay to reduce flicker
  }
}
