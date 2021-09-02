mod chip8;

fn main() {
    println!("Hello, world!");
    let a = chip8::Chip8::initialize();
    println!("{}", a.opcode);
}
