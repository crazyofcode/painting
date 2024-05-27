OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS

{
  /* rust-sbi 在 M-mode 下初始化相关的寄存器后进入 kernel 的地址是 0x80200000 */
  . = 0x80200000;

  .text : {
    *(.text .text.*)
    . = ALIGN(0x1000);
    *(trampvec);
    . = ALIGN(0x1000);
    PROVIDE(etext = .);
  }

  .rodata : {
    *(.srodata .srodata.*)
    . = ALIGN(16);
    *(.rodata .rodata.*)
    . = ALIGN(16);
  }

  .data : {
    *(.sdata .sdata.*)
    . = ALIGN(16);
    *(.data .data.*)
    . = ALIGN(16);
  }

  .bss : {
    *(.sbss .sbss.*)
    . = ALIGN(16);
    *(.bss .bss.*)
    . = ALIGN(16);
  }

  PROVIDE(end = .);
}
