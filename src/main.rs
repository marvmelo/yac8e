mod chip8;

fn main() {
    println!("Hello, world!");
    let a = chip8::Chip8::init();
    println!("{}", a.opcode);
}
