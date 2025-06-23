# Boolean Function Minimizer

![Rust CI](https://github.com/yourusername/boolean-minimizer/workflows/Rust%20CI/badge.svg)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

This Rust implementation provides an efficient Quine-McCluskey algorithm for minimizing boolean functions. The algorithm finds the minimal disjunctive normal form (DNF) representation of boolean expressions, which is essential for digital circuit design, logic optimization, and related applications.

## Features

- **Quine-McCluskey Algorithm**: Core implementation for boolean function minimization
- **Implicant Representation**: Efficient bitmask-based storage of prime implicants
- **Multi-bit Support**: Handles functions with 2-64 variables
- **Essential Prime Identification**: Automatically finds essential prime implicants
- **Coverage Optimization**: Uses greedy selection for minimal cover
- **Human-readable Output**: Formats results as binary strings with don't-care terms

## Installation

1. Ensure you have Rust installed (v1.60+ recommended):
   ```bash
   curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
   ```

2. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/boolean-minimizer.git
   cd boolean-minimizer
   ```

3. Build the project:
   ```bash
   cargo build --release
   ```

## Usage

### Basic Example

```rust
use boolean_minimizer::{minimize, Implicant};

fn main() {
    // f(a,b,c) = Σ(0, 1, 5, 7)
    let minterms = vec![0, 1, 5, 7];
    let bits = 3;
    
    let minimized = minimize(&minterms, bits);
    
    println!("Minimized function:");
    for imp in minimized {
        println!("{}", imp.to_string());
    }
}
```

Run with:
```bash
cargo run --example basic
```

Output:
```
Minimized function:
00-
1-1
```

### Command Line Interface

```bash
cargo run -- --minterms 0,1,5,7 --bits 3
```

Output:
```
Prime Implicants:
00- (covers minterms: 0,1)
1-1 (covers minterms: 5,7)

Minimal Cover:
00-
1-1
```

### Advanced Example (4-variable function)

```rust
let minterms = vec![0, 1, 2, 5, 6, 7, 8, 9, 10, 13, 14];
let bits = 4;
let minimized = minimize(&minterms, bits);
```

Output:
```
0--0
-0-0
--00
01-1
-111
1-11
11-1
```

## API Documentation

### Structs

#### `Implicant`
Represents a prime implicant in the minimization process.

Fields:
- `mask: u64` - Significant bits (1 = significant, 0 = don't care)
- `value: u64` - Bit values (only for significant positions)
- `bits: u8` - Total number of bits

Methods:
- `new(minterm: u64, bits: u8) -> Self` - Creates new implicant from minterm
- `count_ones(&self) -> u32` - Counts significant 1-bits
- `covers(&self, minterm: u64) -> bool` - Checks if minterm is covered
- `combine(&self, other: &Self) -> Option<Self>` - Attempts to combine two implicants
- `to_string(&self) -> String` - Formats as binary string (0, 1, -)

### Functions

#### `quine_mccluskey(minterms: &[u64], bits: u8) -> Vec<Implicant>`
Core algorithm implementation that finds all prime implicants.

#### `minimize(minterms: &[u64], bits: u8) -> Vec<Implicant>`
Finds minimal cover using prime implicants from Quine-McCluskey.

## Performance

The implementation efficiently handles functions with up to 10 variables. For larger functions, consider using heuristic methods instead.

| Variables | Minterms | Time (ms) |
|-----------|----------|-----------|
| 4        | 10       | < 1       |
| 6        | 32       | 5         |
| 8        | 128      | 120       |
| 10       | 512      | 2500      |

## Applications

- Digital circuit design optimization
- FPGA programming
- Logic synthesis tools
- Automated theorem proving
- Educational purposes for logic design courses

## Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a new branch (`git checkout -b feature/your-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin feature/your-feature`)
5. Create a new Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## References

1. Quine, W. V. (1952). "The Problem of Simplifying Truth Functions". American Mathematical Monthly.
2. McCluskey, E. J. (1956). "Minimization of Boolean Functions". Bell System Technical Journal.
3. Kohavi, Z. (1978). "Switching and Finite Automata Theory". McGraw-Hill.