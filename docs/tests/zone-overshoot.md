# Test: Zone controller stopping overshoot

**Goal:** measure how far past the 40 cm stop line the robot coasts.

**Setup:** target ~80 cm away. Robot drives in at PWM 150 and stops when the reading drops below 40 cm. Final distance read from Serial Monitor (300 ms logging). Overshoot = 40 − final distance.

## Results

| Trial | Start (cm) | Stopped at (cm) | Overshoot (cm) |
|---|---|---|---|
| 1 | 78.8 | 33.79 | 6.2 |
| 2 | 79.5 | 35.09 | 4.9 |
| 3 | 79.8 | 32.20 | 7.8 |
| 4 | 80.0 | 33.51 | 6.5 |
| 5 | 79.6 | 34.04 | 6.0 |

**Average overshoot: 6.3 ± 1.0 cm.** Approach speed ≈ 40 cm/s (from consecutive readings 300 ms apart).

## Takeaways

- The robot coasts ~6 cm after the motors cut, because the zone controller goes straight from full speed to stop.
- Variation comes from the 60 ms sample rate: at 40 cm/s the robot moves up to 2.4 cm between readings.
- Estimated deceleration: a = v² / 2d ≈ 1.2 m/s².
- **Baseline for proportional control** — re-run this exact test after adding it.
