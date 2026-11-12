# Test: HC-SR04 ultrasonic accuracy

**Goal:** measure how accurate and how repeatable the distance sensor is.

**Setup:** sensor level, aimed at a flat wall/door. Distances marked on the floor with a ruler (inches, converted with 1 in = 2.54 cm). Each reading is a 10-sample average (`readAverage()`).

## Results

| Actual (cm) | Measured (cm) | Error (cm) | Error (%) | Spread |
|---|---|---|---|---|
| 10.2 | 9.33 | −0.87 | 8.5% | ±0.02 |
| 20.3 | 19.52 | −0.78 | 3.8% | ±0.02 |
| 30.5 | 29.85 | −0.65 | 2.1% | ±0.03 |
| 50.8 | 49.4 | −1.4 | 2.8% | ±0.2 |
| 71.1 | 68.3 | −2.8 | 3.9% | ±0.07 |
| 101.6 | 98.60 | −3.0 | 3.0% | ±0.03 |
| 152.4 | 148.53 | −3.9 | 2.5% | ±0.03 |

## Takeaways

- **Repeatable:** readings stayed within ±0.2 cm or better.
- **Biased:** consistently ~3% short beyond ~50 cm. That's systematic error, not noise, so averaging can't fix it but a calibration factor (~1.03) can.
- Next: apply the correction and re-test.
