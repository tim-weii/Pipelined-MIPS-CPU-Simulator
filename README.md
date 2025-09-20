#  Pipelined-MIPS-CPU-Simulator

##  Overview  
It reads assembly-like instructions from `memory.txt`, simulates their execution through the pipeline, and writes detailed cycle-by-cycle states into `result.txt`.  

The simulator models **data hazards**, **control hazards**, and **pipeline stalls**, providing a practical illustration of how real processors handle instruction-level parallelism.  

---

##  Pipeline Background  
The classic MIPS pipeline consists of 5 stages:  
1. **IF (Instruction Fetch)** – fetch the instruction  
2. **ID (Instruction Decode)** – decode and read registers  
3. **EXE (Execute)** – perform ALU operations or address calculation  
4. **MEM (Memory Access)** – load/store instructions access memory  
5. **WB (Write Back)** – write results back to registers  

Key challenges:  
- **Data Hazards** – when a later instruction needs a value not yet written back (e.g., `lw` followed by `add`).  
- **Control Hazards** – branch instructions (`beq`) require resolved conditions before deciding the next instruction.  
- **Stalling** – the pipeline inserts bubbles when hazards cannot be avoided.  

---

##  Main Features  
- **Instruction Parsing (`cut`, `cut1`)**  
  Splits instructions into opcode and operands, e.g., `add $1, $2, $3 → ["add", "$1", "$2", "$3"]`.  

- **Hazard & Stall Detection (`stalling`)**  
  Detects potential data/control hazards and introduces stalls when necessary.  

- **Pipeline Simulation (`cycling`)**  
  Models transitions across pipeline stages and executes instruction logic:  
  - Arithmetic (`add`, `sub`)  
  - Memory access (`lw`, `sw`)  
  - Branch (`beq`)  

- **Hazard Handling**  
  - **Load-use hazard**: stalls when a `lw` result is needed by the next instruction.  
  - **Store hazard**: checks conflicts between `sw` and pending loads/writes.  
  - **Branch hazard**: stalls until register values are ready; updates PC if branch taken.  

- **Result Output**  
  - Logs **pipeline stage states** (IF/ID/EXE/MEM/WB) for each cycle into `result.txt`.  
  - Dumps final **registers** and **memory contents** at the end of execution.  

---

##  Files  
- `memory.txt` – input instruction set  
- `result.txt` – simulation results (pipeline states, registers, memory)  

---

##  Usage  
Compile and run:  
```bash
g++ pipeline.cpp -o pipeline
./pipeline
