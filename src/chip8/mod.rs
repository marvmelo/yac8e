use std::fs;
pub mod gfx;

pub struct Chip8 {
    pub opcode: u16,
    memory: [u8; 4096],
    V: [u8; 16],
    I: u16,
    pc: u16,
    gfx: [bool; 64*32],
    delay_timer: u8,
    delay_sound: u8,
    stack: [u16; 16],
    sp: u16,
    key: [bool; 16],
    draw_flag: bool,
}

impl Chip8 {

    pub fn initialize() -> Chip8 {

        let font_set: [u8; 80]  =
        [
          0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
          0x20, 0x60, 0x20, 0x20, 0x70, // 1
          0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
          0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
          0x90, 0x90, 0xF0, 0x10, 0x10, // 4
          0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
          0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
          0xF0, 0x10, 0x20, 0x40, 0x40, // 7
          0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
          0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
          0xF0, 0x90, 0xF0, 0x90, 0x90, // A
          0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
          0xF0, 0x80, 0x80, 0x80, 0xF0, // C
          0xE0, 0x90, 0x90, 0x90, 0xE0, // D
          0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
          0xF0, 0x80, 0xF0, 0x80, 0x80  // F
        ];

        let mut cpu: Chip8 = Chip8  {
            opcode: 0,
            memory: [0; 4096],
            V: [0; 16],
            I: 0,
            pc: 0x200,
            gfx: [false; 64*32],
            delay_timer: 0,
            delay_sound: 0,
            stack: [0; 16],
            sp: 0,
            key: [false; 16],
            draw_flag: false,
        };

        for n in 1..80 {
            cpu.memory[n] = font_set[n];
        }

        return cpu;
    }

    pub fn load(&mut self, path: String) {

        let rom = fs::read(path).expect("Unexisting rom file!");

        for byte in 0..rom.len() {
            self.memory[0x200 + byte] = rom[byte];
        }

    }

}

