## Compiling the Example

### Install Tooling

#### wasi-sdk

Install wasi-sdk from [https://github.com/WebAssembly/wasi-sdk?tab=readme-ov-file#install](https://github.com/WebAssembly/wasi-sdk?tab=readme-ov-file#install)

### Generate wit bindings

```shell
wit-bindgen c wit
```

### Compile the Example

```shell
[WASI_SDK_PATH]/bin/clang main.c my_world.c my_world_component_type.o -o main.module.wasm  -mexec-model=reactor
```

Convert to wasm component
```shell
wasm-tools component new ./main.module.wasm -o main.component.wasm
```

Now run `main.component.wasm` with a wasm runtime that supports wasi-gfx.
