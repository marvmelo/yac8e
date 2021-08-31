mod chip8;

fn main() {
    println!("Hello, world!");
    let a = chip8::Chip8 {opcode: 8};
    println!("{}", a.opcode);
}
