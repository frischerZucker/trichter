source ~/.venv/bin/activate

pyocd flash --target stm32c011f6px Debug/breadboard-prototype.elf

deactivate
