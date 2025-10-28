# ⚙️⚡ BareMetalPLL

A bare-metal C simulation of a wideband digital Phase-Locked Loop (PLL), designed for learning, analysis, and future hardware emulation. This project models the core components of a modern RF synthesizer from the ground up using fixed-point arithmetic and real-time control principles.

---

## ✨ Features

* **Integer-N DPLL Architecture:** Simulates a Type-II Integer-N PLL capable of locking to integer multiples of the reference frequency.
* **Wideband Simulation:** Models a synthesizer architecture covering a wide output range (e.g., 30 MHz - 30 GHz) using a high-frequency NCO core (e.g., 7.5 - 15 GHz) combined with output dividers and a multiplier (doubler).
* **Bare-Metal C (C17):** Implemented in standard C with a focus on deterministic execution suitable for embedded environments.
    * **Fixed-Point Math (Q16.16):** Uses scaled integers for loop filter calculations, avoiding floating-point dependency.
    * **No Dynamic Memory:** Relies solely on static and stack allocation for predictable performance.
* **PI Loop Controller:** Implements a stable Proportional-Integral loop filter.
* **Modular Design:** Separated into core PLL logic (`pll.c`, `pll.h`) and math helpers (`helpers.h`).

---

## 🚀 Roadmap

This project is actively being developed. Future enhancements include:

* **Fractional-N PLL:** Implementing a Delta-Sigma Modulator (DSM) to enable fine frequency resolution.
* **Modular Loop Filters:** Allowing selection between the current PI filter and models of analog filters.
    
* **Noise Injection:** Adding models for reference noise and VCO phase noise to analyze their impact on performance.
    
* **Performance Analysis:** Outputting simulation data (phase error, frequency vs. time) for plotting and analysis in MATLAB or Python.
    
* **Lock Detection:** Refining the lock detection mechanism for robustness.

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🧑🏽‍💻 Developer

* Genaro Salazar Ruiz
