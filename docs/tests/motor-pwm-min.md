# Test: Motor PWM start thresholds

**Goal:** find the lowest PWM value (0–255) that makes each side of the robot move.

**Setup:** robot on the floor, both sides driven at the same PWM, starting at 0 and increasing by 10 every second (`pwmRampTest()` in the firmware). Recorded the value where each side's wheels began turning.

## Results

| Side | PWM to start moving |
|---|---|
| Left | 100 |
| Right | 80 |

## Takeaway

Below these values the motor hums but can't overcome static friction in the gearbox. The right side starts more easily, so at equal PWM it spins faster and the robot drifts left (confirmed on the floor). Next: add a per-side trim, and never command a speed below these thresholds.
