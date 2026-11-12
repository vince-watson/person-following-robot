# person-following autonomous robot

a 4WD robot that detects a person and follows them at a set distance — built from the ground up in C++ on Arduino, with a Raspberry Pi vision layer planned.

**Status:** In progress — distance-holding (zone controller) working; proportional control and camera tracking next.


<!-- Add demo video link here -->

---

## Approach

Instead of building "person following" all at once, the project is broken into small, measurable sub-goals. Each piece is built, then **tested and characterized** before the next layer goes on top:

1. Motor control with variable speed (PWM) ✅
2. Characterize motors and sensor ✅
3. Non-blocking distance control ✅
4. Drive-straight trim + proportional control ⏳
5. Differential-drive kinematics (smooth turning)
6. Camera-based steering (color first, then person detection)

---

## Hardware

| Component | Role |
|---|---|
| OSOYOO FlexiRover 4WD chassis | Frame, 4 geared DC motors |
| Arduino Uno | Real-time motor control + sensing (C++) |
| L298N H-bridge motor driver | Drives motors from battery power, controlled by Arduino PWM |
| HC-SR04 ultrasonic sensor | Distance to target |
| 2× 18650 Li-ion cells | Motor power |
| Raspberry Pi 4 + Arducam OV5647 | Vision layer (planned) |

### Wiring

| Signal | Arduino pin |
|---|---|
| Left motor ENA (PWM speed) | D9 |
| Left motor IN1 / IN2 (direction) | D12 / D11 |
| Right motor ENB (PWM speed) | D6 |
| Right motor IN3 / IN4 (direction) | D7 / D8 |
| HC-SR04 TRIG | D4 |
| HC-SR04 ECHO | D3 |
| HC-SR04 VCC / GND | 5V / GND |

---

## Software

Object-oriented C++, one class per part:

- **`Motor`** — stores its 3 pins; `forward(speed)`, `backward(speed)`, `stop()`. Direction set by IN1/IN2 (H-bridge), speed set by PWM on the enable pin.
- **`Sensor`** — HC-SR04 driver. `readCM()` fires a 10 µs trigger pulse, times the echo with `pulseIn()`, and converts to distance: $d = \frac{t \times 0.0343}{2}$. `readAverage()` averages multiple readings.
- **`Robot`** — owns a left and right `Motor` (built via initializer list) and coordinates both sides.

The main loop is **non-blocking** (`millis()`-based) with two independent timers:
- **60 ms** — read sensor and update motors (control loop)
- **300 ms** — log distance over serial

### Current controller: zone-based

| Distance | Action |
|---|---|
| No echo | Stop |
| > 40 cm | Drive toward target |
| 20–40 cm | Hold (stop) |
| < 20 cm | Back up |

---

## Test Results

Full write-ups: [motor thresholds](docs/tests/motor-pwm-min.md) · [sensor accuracy](docs/tests/sensor-accuracy.md) · [zone overshoot](docs/tests/zone-overshoot.md)

### 1. Motor PWM start thresholds

Ramped PWM from 0 upward in steps of 10 and recorded where each side began turning.

| Side | PWM to start moving |
|---|---|
| Left | 100 |
| Right | 80 |

**Takeaway:** the right side starts more easily, so at equal PWM the robot drifts left (observed). A per-side trim is being added to correct this.

### 2. Ultrasonic sensor accuracy

Flat wall/door target, sensor level, 10-sample averages at each distance.

| Actual (cm) | Measured (cm) | Error (cm) | Error (%) | Spread |
|---|---|---|---|---|
| 10.2 | 9.33 | −0.87 | 8.5% | ±0.02 |
| 20.3 | 19.52 | −0.78 | 3.8% | ±0.02 |
| 30.5 | 29.85 | −0.65 | 2.1% | ±0.03 |
| 50.8 | 49.4 | −1.4 | 2.8% | ±0.2 |
| 71.1 | 68.3 | −2.8 | 3.9% | ±0.07 |
| 101.6 | 98.60 | −3.0 | 3.0% | ±0.03 |
| 152.4 | 148.53 | −3.9 | 2.5% | ±0.03 |

**Takeaways:**
- **Highly repeatable** (±0.2 cm or better with averaging).
- **Consistently reads ~3% short** beyond ~50 cm — a systematic bias, not random noise. Correctable with a calibration factor of ~1.03 (to be applied and re-tested).

### 3. Zone controller stopping overshoot

Target at ~80 cm; robot drives in at PWM 150 and stops when it crosses 40 cm. Overshoot = 40 − final distance.

| Trial | Start (cm) | Stopped at (cm) | Overshoot (cm) |
|---|---|---|---|
| 1 | 78.8 | 33.79 | 6.2 |
| 2 | 79.5 | 35.09 | 4.9 |
| 3 | 79.8 | 32.20 | 7.8 |
| 4 | 80.0 | 33.51 | 6.5 |
| 5 | 79.6 | 34.04 | 6.0 |

**Average overshoot: 6.3 ± 1.0 cm** at an approach speed of **~40 cm/s**.

**Takeaways:**
- The robot coasts ~6 cm after the motors cut — momentum the on/off zone controller doesn't account for.
- Run-to-run variation comes from the 60 ms sample rate: at 40 cm/s the robot travels up to 2.4 cm between readings.
- Estimated deceleration from the data: $a = \frac{v^2}{2d} \approx 1.2\ \text{m/s}^2$.
- **This is the baseline** for proportional control, which should slow the robot *before* the target instead of stopping abruptly.

---

## Roadmap

- [ ] Per-side trim so the robot drives straight
- [ ] Proportional distance control ($\text{speed} = K_p \cdot (d - d_{\text{target}})$) — re-run overshoot test and compare
- [ ] Apply sensor calibration factor and re-test accuracy
- [ ] Differential-drive kinematics: command forward speed $v$ and turn rate $\omega$, convert to wheel speeds
- [ ] Raspberry Pi + camera: follow a colored target, then a person (MediaPipe)
- [ ] Pi ↔ Arduino serial link, then ROS2 node architecture
- [ ] 3D-printed sensor/camera mounts

---

## Author

**Vince Watson Jr.** — Computer Engineering, Florida State University
