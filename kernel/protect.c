#include	"const.h"
#include	"type.h"
#include	"proto.h"
#include	"global.h"
#include	"protect.h"

PUBLIC void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
{
	int i;
	int text_color = 0x74; // Gray back, Red chars

	char* err_msg[] = {
	"#DE Divide Error",
	"#DB RESERVED",
	"--  NMI  Interrupt",
	"#BP Breakpoint",
	"#OF Overflow",
	"#BR Bound Range Exceeded",
	"#UD Invalid Opcode (Undefined Opcode)",
	"#NM Device Not Available (No Match Coprocessor)",
	"#DF Double Fault",
	"    Coprocessor Segment Overrun (reserved)",
	"#TS Invalid TSS",
	"#NP Segment Not Present",
	"#SS Stack Segment Fault",
	"#GP General Protection",
	"#PF Page Fault",
	"--  (Intel Reserved...)",
	"#MF x87 FPU Floating Point Error (Math Fault)",
	"#AC Alignment Check",
	"#MC Machine Check",
	"#XF SIMD Floating Point Exception"
	};

	cursor_pos = 0;
	for (int i = 0; i < 80 * 5; i++)
	 disp_str(" ");

	cursor_pos = 0;

	disp_color_str("Exception: ---> ", text_color);
	disp_color_str(err_msg[vec_no], text_color);
	disp_color_str("\n\n", text_color);
	disp_color_str("EFLAGS:", text_color);
	disp_int(eflags);
	disp_color_str("CS:", text_color);
	disp_int(cs);
	disp_color_str("EIP:", text_color);
	disp_int(eip);

	if (err_code != 0xFFFFFFFF) {
		disp_color_str("Error code:", text_color);
		disp_int(err_code);
	}
} 

PRIVATE void init_idt_descriptor(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege)
{
	GATE*	p_gate = &idt[vector];
	u32	base = (u32) handler;
	p_gate->offset_low = base & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->dcount = 0;
	p_gate->attr = desc_type | (privilege << 5);
	p_gate->offset_high = (base >> 16) & 0xFFFF;	
}

PUBLIC void init_prot()
{
	init_8259A();
	init_idt_descriptor(INT_VECTOR_DIVIDE, DA_386IGate, divide_error, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_DEBUG, DA_386IGate, single_step_exception, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_exception, PRIVILEGE_USER);
	init_idt_descriptor(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_USER);
	init_idt_descriptor(INT_VECTOR_BOUNDS, DA_386IGate, bounds_check, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_INVAL_OP, DA_386IGate, inval_opcode, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_NOT, DA_386IGate, copr_not_available, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_SEG, DA_386IGate, copr_seg_overrun, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_INVAL_TSS, DA_386IGate, inval_tss, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_exception, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_PROTECTION, DA_386IGate, general_protection, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_ERR, DA_386IGate, copr_error, PRIVILEGE_KERNEL);
}
