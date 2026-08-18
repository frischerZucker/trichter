# Hardware Design

The funnel should include the following functions:
- Measurement of fluid volume passing through the funnel
- Measurement of time it takes to empty the funnel
- Display for user interface
- Power supply via USB-C

## Power Supply

Power should be supplied via USB-C.
The easiest way would be to provide power directly via a USB-C receptable without PD at 5V.
If this isn't sufficient, a PD-IC can be used to negotiate more power.
In this case the device could be powered by using a USB power supply or an external powerbank for mobile use.

An other option would be to include a battery and charging circuitry.
This enable stand alone mobile use without an external powerbank or power supply.
Additional components of the PD and charging circuit would complicate the build, so I would stick with the first option for now.

## Flow Sensor

A hall effect flow sensor ([link to digikey](https://www.digikey.de/de/products/detail/seeed-technology-co-ltd/114991176/7387421)) is used for measuring the fluid passing through the funnel.
It requires connections to GND and 5V and provides an output that generates pulses.
To measure the flow through the sensor the pulses can be counted by a microcontroller.
This can also be used to measure the time it took to empty the funnel.
According to the datasheet it is suitable for flow detection in water dispensers and coffee machines.
Therefor it should be safe to drink through it.

## Parts
| Part Name | Manufacturer | Description | Use Case |
|---|---|---|---|
| 114991176 | Seeed Technology Co., Ltd | flow sensor | Measuring volume of fluid & time |
| STM32C051K8T6 | STMicroelectronics | microcontroller | microcontroller for the device |
| https://de.aliexpress.com/item/1005008358892824.html?spm=a2g0o.productlist.main.36.63e72685jsu3TG | ? | display | showing user interface |
| AP7361C-33E-13 | Diodes Incorporated | LDO | 5V to 3V3 |
