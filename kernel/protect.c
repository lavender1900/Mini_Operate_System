#include	"global.h"
#include	"protect.h"
#include	"interrupts.h"
#include	"i386macros.h"
#include	"8259A.h"
#include	"io.h"

void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();
void	sys_call();

PRIVATE void init_idt_descriptor(unsigned char vector, u8 desc_type, int_handler handler, unsigned char privilege);

PUBLIC void cpu_reserved_exception_handler(int vec_no, int err_code, int eip, int cs, int eflags)
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


PUBLIC void init_descriptor(DESCRIPTOR* p_desc, u32 base, u32 limit, u16 attribute)
{
	p_desc->limit_low = limit & 0x0FFFF;
	p_desc->base_low = base & 0x0FFFF;
	p_desc->base_mid = (base >> 16) & 0x0FF;
	p_desc->attr1 = attribute & 0xFF;
	p_desc->limit_high_attr2 = ((limit >> 16) & 0x0F | (attribute >> 8) & 0xF0);
	p_desc->base_high = (base >> 24) & 0x0FF;
}

PUBLIC void init_protect_mode_interrupt_mechanism()
{
	init_8259A();

	// ****************** Initilize Interrupt Descriptor Table *************************

	init_idt_descriptor(INT_VECTOR_DIVIDE, DA_386INT_GATE, divide_error, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_DEBUG, DA_386INT_GATE, single_step_exception, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_NMI, DA_386INT_GATE, nmi, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_BREAKPOINT, DA_386INT_GATE, breakpoint_exception, PRIVILEGE_USER);
	init_idt_descriptor(INT_VECTOR_OVERFLOW, DA_386INT_GATE, overflow, PRIVILEGE_USER);
	init_idt_descriptor(INT_VECTOR_BOUNDS, DA_386INT_GATE, bounds_check, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_INVAL_OP, DA_386INT_GATE, inval_opcode, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_NOT, DA_386INT_GATE, copr_not_available, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_DOUBLE_FAULT, DA_386INT_GATE, double_fault, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_SEG, DA_386INT_GATE, copr_seg_overrun, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_INVAL_TSS, DA_386INT_GATE, inval_tss, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_SEG_NOT, DA_386INT_GATE, segment_not_present, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_STACK_FAULT, DA_386INT_GATE, stack_exception, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_PROTECTION, DA_386INT_GATE, general_protection, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_PAGE_FAULT, DA_386INT_GATE, page_fault, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_COPROC_ERR, DA_386INT_GATE, copr_error, PRIVILEGE_KERNEL);

	init_idt_descriptor(INT_VECTOR_IRQ0 + 0, DA_386INT_GATE, hwint00, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 1, DA_386INT_GATE, hwint01, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 2, DA_386INT_GATE, hwint02, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 3, DA_386INT_GATE, hwint03, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 4, DA_386INT_GATE, hwint04, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 5, DA_386INT_GATE, hwint05, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 6, DA_386INT_GATE, hwint06, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ0 + 7, DA_386INT_GATE, hwint07, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 0, DA_386INT_GATE, hwint08, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 1, DA_386INT_GATE, hwint09, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 2, DA_386INT_GATE, hwint10, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 3, DA_386INT_GATE, hwint11, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 4, DA_386INT_GATE, hwint12, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 5, DA_386INT_GATE, hwint13, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 6, DA_386INT_GATE, hwint14, PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_VECTOR_IRQ8 + 7, DA_386INT_GATE, hwint15, PRIVILEGE_KERNEL);

	init_idt_descriptor(INT_VECTOR_SYS_CALL, DA_386INT_GATE, sys_call, PRIVILEGE_USER);
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
