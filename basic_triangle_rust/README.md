## Compiling the Example

### Install Tooling

#### wasm-tools
```shell
cargo install wasm-tools
```

### Compile the Example

```shell
cargo build --target wasm32-unknown-unknown --release
wasm-tools component new ./target/wasm32-unknown-unknown/release/basic_triangle.wasm -o ./component.wasm
```
