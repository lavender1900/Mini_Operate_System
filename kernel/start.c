#include	"const.h"
#include	"type.h"
#include	"protect.h"
#include	"proto.h"
#include	"global.h"

void	initializeGlobalParameters();

PUBLIC void cstart() {
 initializeGlobalParameters();

 disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n-----------\"cstart\" begins-----------\n");
disp_str("Cursor pos At ");
disp_int((u32) &cursor_pos);

 kmemcpy((void*) (*((u32*) (&gdt_ptr[2]))), &gdt,
	*((u16*) (&gdt_ptr[0])) + 1
 );
 
 u16* p_gdt_limit = (u16*) (&gdt_ptr[0]);
 u32* p_gdt_base = (u32*) (&gdt_ptr[2]);
 *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
 *p_gdt_base = (u32) &gdt; 

 u16* p_idt_limit = (u16*) (&idt_ptr[0]);
 u32* p_idt_base = (u32*) (&idt_ptr[2]);
 *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
 *p_idt_base = (u32) &idt;

 init_prot();

 disp_str("--------------\"cstart\" ends----------------\n");
}
